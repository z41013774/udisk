#include "s3c2440_ftl.h"
#include "s3c2440_usb_util.h"

#define L2P_ADDR		0x30100000
#define L2P_SIZE		BLOCK_NUM*PAGE_NUM*4
#define BLOCK_INFO_SIZE	BLOCK_NUM*4
#define BLOCK_INFO_ADDR L2P_ADDR-BLOCK_INFO_SIZE
volatile uint32_t serial_number;
volatile uint8_t need_wait;
/*DDR allocate*/
/*code data bss seg*/
/*30000000------30100000*/
/*block info*/
/*300FF000------30100000*/
/*L2P*/
/*30100000------30140000*/
/*bad blk*/
/**************************************
*1 scan device
*2 
***************************************/
volatile block_info_t *blk_info;
volatile uint32_t *usb_l2p;
usb_dev_t dev;

static void insertion_sort(block_index_t *block_index,uint16_t block_num)
{
	uint16_t i;
	uint16_t j;
	block_index_t temp;
    for (i = 1; i < block_num; i++)
    {
    	
    	if (block_index[i - 1].serial_number > block_index[i].serial_number)
        {
            temp.block_num = block_index[i].block_num;
			temp.serial_number = block_index[i].serial_number;
			 
            j = i;
            while (j > 0 && block_index[j - 1].serial_number > temp.serial_number)
            {
                block_index[j].serial_number = block_index[j - 1].serial_number;
				block_index[j].block_num = block_index[j-1].block_num;
                j--;
            }
            block_index[j].block_num = temp.block_num;
			block_index[j].serial_number = temp.serial_number;
        }
    	
    }
}
block_index_t block_index[BLOCK_NUM];
/*----------------------------------------------------------
1 scan all block ignore bad block and read oob area.
2 sort according to block num low to high .
3 recovery L2P table and blk info table
-----------------------------------------------------------*/
void scan_storage(void)
{
	int block;
	int page;
	tag_t tag;
	uint32_t serial_num;
	nand_flash_addr_t flash_addr;
	
	uint16_t max_block_num;
	uint16_t min_block_num;
	uint8_t finder_allocating_page = 0;
	block_info_t *bi;
	uint16_t block_num = 0;
	flash_addr.page = 0;
	/*get serial number by reading oob area of the first page.*/
	for(block = dev.start_block;block < dev.end_block;block++)
	{
		flash_addr.block = block;
		flash_info.flash_op->nand_read_oob(&flash_addr,&tag);
		if(tag.block_state != BLOCK_STATE_DEAD)
		{
			block_index[block_num].block_num = block;
			block_index[block_num].serial_number =tag.serial_number;
			block_num++;
		}		
	}
	/*sort,according to serial number form low to high*/
	insertion_sort((block_index_t *)&block_index[0],block_num);
	/*recovery L2P,block info*/
	/*read oob area by serial number */

	max_block_num = 0;
	min_block_num = block_num;
	for(block=0;block >=0;block--)
	{
		for(page=flash_info.page_num;page>=0;page--)
		{
			flash_addr.block =block_index[block].block_num;
			flash_addr.page = page;
			flash_info.flash_op->nand_read_oob(&flash_addr,&tag);
		//	if(tag.serial_number != INVAILD_SERIAL_NUMBER)
			{
				if((tag.lpn >= 0) && (tag.lpn <= BLOCK_NUM*PAGE_NUM))
				{
					if(usb_l2p[tag.lpn] == 0xFFFFFFFF)
					{
						usb_l2p[tag.lpn] = ((block_index[block].block_num<<flash_info.page_shift)|page);
						
						if(finder_allocating_page == 0)
						{
							if(page < flash_info.page_num)
							{
								dev.allocation_block = block_index[block].block_num;
								dev.finder_block= tag.finder_block;
								dev.allocation_page = (page+1);
							}
							else
							{
								/*page == page num*/
								dev.allocation_page = flash_info.page_num;
								dev.finder_block = tag.finder_block;
								get_availd_block();
								
							}
							finder_allocating_page = 1;
							T(USB_TRACE_SCAN,("we are using block 0x%x,page 0x%x\r\n",dev.allocation_block,dev.allocation_page));

						}
						else
						{
							T(USB_TRACE_ERROR,("no find block we are using\r\n"));
						}
					}						
					else
					{
						blk_info[block].dirty_cnt++;
					}
				}	
			}				
		}
	}
	if(dev.allocation_block == 0xffff)
	{
		/*only one time*/
		dev.allocation_block = dev.start_block;
		dev.allocation_page = 0;
		blk_info[dev.allocation_block].block_state = BLOCK_STATE_ALLOCATING;
		dev.finder_block = dev.start_block+1;
		T(USB_TRACE_SCAN,("only one time, we use block and page\r\n"));
		
	}
	T(USB_TRACE_SCAN,("scan dev done\r\n"));
}
uint32_t usb_allocate_block(void)
{
	uint32_t block;
	//block = usb_find_block_for_allocation();
	#if 0
	blk_info[dev.allocation_block].block_state = BLOCK_STATE_FULL;
	dev.allocation_page = 0;
	dev.allocation_block = dev.finder_block;
	dev.finder_block = INVAILD_BLOCK_NUMBER;
	blk_info[dev.allocation_block].block_state = BLOCK_STATE_ALLOCATING;
	#endif
	//usb_check_grabage_collection();
}
block_info_t *get_block_info(uint32_t block)
{
	return (block_info_t *)&blk_info[block];
}
uint32_t usb_find_block_for_allocation()
{
	return dev.finder_block;
}
/*****************************************************
1 search block ,the erase cnt of block should be max .
2 erase cnt 70% + 30% dirty cnt
/*****************************************************/
#define MAXERASECNT		3000
#define MAXDIRTYCNT		64
#define MINERASECNT		0
#define MINDIRTYCNT		0
static uint32_t usb_find_block_grabage_collection()
{
	uint32_t block;
	block_info_t *bi;
	uint32_t block_num;
	uint32_t val;
	uint32_t min_val;
	/*serach block */
	bi = get_block_info(dev.start_block);
	min_val = bi->erase_cnt*0.7+bi->dirty_cnt*0.3;
	for(block=dev.start_block+2;block < dev.end_block;block++)
	{
		bi = get_block_info(block);
		if((bi->block_state != BLOCK_STATE_ALLOCATING) \
			&& (bi->block_state != BLOCK_STATE_DEAD) \
			&& (bi->block_state != BLOCK_STATE_COLLECTING))
		{
			val = bi->erase_cnt*0.7+bi->dirty_cnt*0.3;
			if(val < min_val)
			{
				min_val = val;
				block_num = block;
			}
			else
			{
				block_num = dev.start_block+2;
			}
		}			
	}
	T(USB_TRACE_GC_DETAIL,("find block 0x%x\r\n",block_num));
	return block_num;

}
/*-----------------------------------------
1 normal gc
2 power on ,do gc if don't Completed power
------------------------------------------*/
static void usb_check_grabage_collection()
{
	uint32_t block;
	/*contion*/	
 	while((dev.allocation_page == 0)&&(dev.finder_block ==INVAILD_BLOCK_NUMBER) )
	{
		block= usb_find_block_grabage_collection();
		usb_grabage_collection_block(block);
 	}
	
}
/*-------------------------------------------------
1 checking ppn in l2p table should be equ to current ppn
  (block<<page_shift+page)
2 lpn in flash oob area
3 do gc,one time,one page
--------------------------------------------------*/
static uint32_t usb_grabage_collection_block(uint32_t block)
{
	uint32_t page;
	uint8_t buf[4];
	
	uint32_t lpn;
	block_info_t *bi;
	tag_t tag;
	nand_flash_addr_t flash_addr_0;
	nand_flash_addr_t flash_addr_1;
	flash_addr_0.block = block;
	for(page = 0;page < flash_info.page_num;page++)
	{
		flash_addr_0.page = page;
		flash_info.flash_op->nand_read_oob(&flash_addr_0,&tag);
		
		if((usb_l2p[tag.lpn]==(block<<flash_info.page_shift+page)))
		{
			flash_info.flash_op->nand_read(&flash_addr_0,r_buf);
			flash_addr_1.block = dev.allocation_block;
			flash_addr_1.page = dev.allocation_page;
			usb_mem_copy(r_buf, w_buf, flash_info.page_size);
			flash_info.flash_op->nand_program(&flash_addr_1,w_buf,&tag);
			usb_l2p[lpn]=((flash_addr_1.block<<flash_info.page_shift)|flash_addr_1.page);
			dev.allocation_page++;
			
		}
	}
	if(flash_info.flash_op->nand_erase)
	{
		if(!flash_info.flash_op->nand_erase(&flash_addr_0))
		{
			bi = get_block_info(block);
			bi->dirty_cnt = 0;
			bi->erase_cnt++;
			bi->block_state = BLOCK_STATE_EMPTY;
			dev.finder_block = block;
			
		}
		else
		{
	
			T(USB_TRACE_GC,("gc done ,but erase %d block fail\r\n",flash_addr_0.block));
		
			update_bbt(flash_addr_0.block);
			/*used reserved block*/
		}

	}
	
	if(dev.allocation_page == flash_info.page_num)
	{
		//need_wait = 1;
		get_availd_block();
	
	}
	else
	{
		//need_wait = 0;
	}
}
void create_bbt(void)
{
	int page;
	uint32_t block;
	nand_flash_addr_t flash_addr;
	flash_addr.block = BBT_BLOCK;
	
	for(page = flash_info.page_num; page>=0; page--)
	{
		if(flash_info.flash_op->nand_read)
		{
			flash_info.flash_op->nand_read(&flash_addr,r_buf);
			usb_mem_copy(r_buf,(uint8_t *)&nand_bbt,51);
			if(((nand_bbt[1]<<8)|nand_bbt[0]) != 0xFFFF)
			{
				T(USB_TRACE_ALWAYS,("find page we will write\r\n"));
				break;
			}
		}

	}
	
}
void update_bbt(uint32_t block_num)
{

}
/*************************************************
1 read data of page0~pagen-1 of current block in r_buf.
2 find finder_block to instead of allocation_block
3 do gc, allocate a block to finder_block
4 program data of r_buf to allocation_block
5 update l2p table
6 update blocknum to btt
**************************************************/
static void usb_retire_block()
{
	uint16_t block,page;
	uint16_t tmp_page;
	uint16_t tmp_block;
	tag_t tag;
	nand_flash_addr_t flash_addr;
	tmp_block = dev.allocation_block;
	tmp_page = dev.allocation_page;
	
	/*set dev.allocation_page = flash_info.page_num*/
	/*in order to keep state machine*/
	dev.allocation_page = flash_info.page_num;
	get_availd_block();
	/*get a block by gc.*/
	//usb_check_grabage_collection();

	for(page=0;page < tmp_page;page++)
	{
		flash_addr.block = tmp_block;
		flash_addr.page = page;
		if(flash_info.flash_op->nand_read)
			flash_info.flash_op->nand_read(&flash_addr,r_buf);
		if(flash_info.flash_op->nand_read_oob)
			flash_info.flash_op->nand_read_oob(&flash_addr,&tag);
		if(flash_info.flash_op->nand_program)
		{
			usb_mem_copy(r_buf,w_buf,flash_info.page_size);
			flash_addr.block = dev.allocation_block;
			flash_addr.page = dev.allocation_page;
			if(flash_addr.page == 0)
			{
				tag.serial_number = serial_number++;
			}
			flash_info.flash_op->nand_program(&flash_addr,w_buf,&tag);
			usb_l2p[tag.lpn] = ((dev.allocation_block<<flash_info.page_shift)|dev.allocation_page);
			dev.allocation_page++;

			/**if dev.allocation_page == page_num*****/
			/*how can i do?*/
			/**/
		}
		
	}
	update_bbt(tmp_block);

}
static void get_availd_block(void)
{
	if(dev.allocation_page == flash_info.page_num)
	{
		blk_info[dev.allocation_block].block_state = BLOCK_STATE_FULL;
		serial_number++;
	}

	dev.allocation_page = 0;
	dev.allocation_block = dev.finder_block;
	dev.finder_block = INVAILD_BLOCK_NUMBER;
	blk_info[dev.allocation_block].block_state = BLOCK_STATE_ALLOCATING;
	blk_info[dev.allocation_block].erase_cnt++;	
	usb_check_grabage_collection();

}
void ftl_init(void)
{
	uint32_t block; 
	block_info_t *bi;
	uint32_t lpn;
	usb_l2p = (uint32_t *)L2P_ADDR;
	blk_info = (block_info_t *)BLOCK_INFO_ADDR;
	dev.allocation_page = 0;
	dev.allocation_block = 0xffff;
	dev.start_block = 10;
	dev.end_block = BLOCK_NUM;
	dev.nreserved_blocks = 5;
	need_wait = 0;
	for(block = dev.start_block;block < dev.end_block; block++)
	{
		bi = (block_info_t *)&blk_info[block];
		bi->dirty_cnt = 0;
		bi->erase_cnt = 0;
	}
	for(lpn = 0 ;lpn < BLOCK_NUM*PAGE_NUM;lpn++)
	{
		usb_l2p[lpn] = 0xffffffff;
	}
	serial_number = 20;
}

/*****************************************************************
  1 get a free block by grabage collection.if we use allocating_block,so data 
   of gc will be write this block.
  2 program w_data to allocation_block
  3 updata l2p table
******************************************************************/
uint8_t usb_write(uint32_t lpn,uint8_t *buf)
{
	uint32_t block;
	nand_flash_addr_t flash_addr;
	tag_t tag;

	
	/*after gc done*/
	/*here ,dev.finder_block can't equ to INVAILD_BLOCK_NUMBER*/
	tag.serial_number = serial_number;
	flash_addr.block = dev.allocation_block;
	flash_addr.page = dev.allocation_page;
	tag.lpn = lpn;
	tag.finder_block = dev.finder_block ;
	tag.block_state = BLOCK_STATE_ALLOCATING;
	tag.erase_cnt = blk_info[dev.allocation_block].erase_cnt;
	if(flash_info.flash_op->nand_program)
	{
		if((flash_info.flash_op->nand_program(&flash_addr,buf,&tag)))
		{
			#if USB_DEBUG
				//USB_TRACE("program 0x%x block,0x%x page\r\n",dev.allocation_block,dev.allocation_page);
			//#else if USB_LOG
			#endif
			usb_retire_block();
		}
	}
	if((usb_l2p[lpn]>=0)&&(usb_l2p[lpn]<=((dev.allocation_block<<flash_info.page_shift)|dev.allocation_page)))
	{
		blk_info[dev.allocation_block].dirty_cnt++;
		usb_l2p[lpn]=((dev.allocation_block<<flash_info.page_shift)|dev.allocation_page);
	}
	else
	{
		usb_l2p[lpn]=((dev.allocation_block<<flash_info.page_shift)|dev.allocation_page);
	}
	dev.allocation_page++;
	if(dev.allocation_page == flash_info.page_num)
	{
		get_availd_block();
	}
	return SUCCESS;	
	
}
void usb_read(uint32_t lpn,uint8_t *buf)
{
	uint32_t ppn;
	nand_flash_addr_t flash_addr;
	ppn = usb_l2p[lpn];
	flash_addr.block = (ppn >> flash_info.page_shift);
	flash_addr.page = (ppn & 0x3f);
	if(flash_info.flash_op->nand_read)
		flash_info.flash_op->nand_read(&flash_addr,r_buf);
}
