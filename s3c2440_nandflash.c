#include "head.h"
#include<stdlib.h>
#define NF_RDDATA8() 		((*(volatile unsigned char*)0x4E000010) )


uint8_t page_cache[PAGE_SIZE];
uint8_t r_buf[PAGE_SIZE];
uint8_t w_buf[PAGE_SIZE];
uint8_t r_oob_buf[12];
uint8_t w_oob_buf[12];

/*5% bad block*/
/*bbt record in block9*/
uint8_t nand_bbt[51];
flash_info_t flash_info;

void samsung_flash_cmd_set(nand_cmd_set_t * samsung_cmd_set)
{
	
	samsung_cmd_set->reset = 0x00ff;
	samsung_cmd_set->readid = 0x0090;
	samsung_cmd_set->erase= 0x60d0;
	samsung_cmd_set->read = 0x0030;
	samsung_cmd_set->program = 0x8010;
	samsung_cmd_set->sts = 0x0070;	
	
}
void samsung_flash_timing(nand_timing_t * samsung_timing)
{
	
	samsung_timing->tacls = TACLS;
	samsung_timing->twrph0 = TWRPH0;
	samsung_timing->twrph1 = TWRPH1;

	
}
void nand_init()
{
	int cnt = 0;
	S3C2440_NAND  * const nand = S3C2410_GetBase_NAND();
	samsung_flash_cmd_set(&flash_info.nand_cmd_set);
	samsung_flash_timing(&flash_info.nand_timing);
	
	nand->NFCONF = (flash_info.nand_timing.tacls<<12)|(flash_info.nand_timing.twrph0<<8)|(flash_info.nand_timing.twrph1<<4);
	nand->NFCONT |= (ENABLE_FLASH_CTL<<0);

	flash_info.flash_op->nand_erase = samsung_nand_erase;
	flash_info.flash_op->nand_program=samsung_nand_program;
	flash_info.flash_op->nand_read = samsung_nand_read;
	flash_info.flash_op->nand_readid = samsung_nand_readid;
	flash_info.flash_op->nand_read_oob = samsung_nand_read_oob;
	flash_info.flash_op->nand_reset = samsung_nand_reset;

	flash_info.block_num = BLOCK_NUM;
	flash_info.page_size = PAGE_SIZE;
	flash_info.page_shift = PAGE_SHIFT;
	flash_info.page_num = PAGE_NUM;
	
	/*init flash golbal var*/
	for(cnt = 0;cnt < sizeof(nand_bbt)/sizeof(nand_bbt[0]);cnt++)
		nand_bbt[cnt] = 0;
	/*test code*/
	//init_program_data();
	//init_read_data();
	
}
void samsung_nand_reset()
{
	int cnt=0;

	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	nand_chip_sel();
	nand_clear_rb();
	for(cnt = 0;cnt < 10;cnt++)
		;
	nand->NFCMD = (flash_info.nand_cmd_set.reset) & 0xff;
	nand_detect_rb();	
	nand_chip_disel();

}
void samsung_nand_readid()
{
	uint32_t nand_id[5];
	int cnt=0;
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	nand_chip_sel();
	nand->NFCMD = (flash_info.nand_cmd_set.readid) & 0xff;
	nand->NFADDR = 0;
	while(cnt < 2)
	{
		nand_id[cnt] = nand->NFDATA;
		cnt++;
	}
	nand_chip_disel();	
}
uint8_t samsung_nand_erase(nand_flash_addr_t *addr)
{
	uint32_t page;
	uint8_t nand_sts;
	uint8_t ret;
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	if(!addr)
		return FAIL;
	page = (addr->block <<flash_info.page_shift)+ addr->page;
	samsung_nand_reset();
	nand_chip_sel();
	nand_clear_rb();
	nand->NFCMD = (flash_info.nand_cmd_set.erase>>8)&0xff;
	/*row1 & row2*/
	nand->NFADDR = (page&0xff);
	nand->NFADDR = ((page>>8)&0xff);

	nand->NFCMD = (flash_info.nand_cmd_set.erase) & 0xff;
	nand_detect_rb();
	nand_sts = NF_RDDATA8();
	if(nand_sts & NAND_STATUS_FAIL)
	{
		ret = FAIL;
	}
	else
	{
		ret = SUCCESS;
	}
		
	nand_chip_disel();
	return ret;

}
uint8_t samsung_nand_read(nand_flash_addr_t *addr,uint8_t *buf)
{
		
	uint32_t cnt;
	uint32_t page;
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	if(!addr)
		return FAIL;
	page = (addr->block <<flash_info.page_shift)+ addr->page;
	samsung_nand_reset();
	nand_chip_sel();
	nand_clear_rb();
	nand->NFCMD = (flash_info.nand_cmd_set.read>>8)&0xff;
	nand->NFADDR = 0;
	nand->NFADDR = 0;
	nand->NFADDR = (page&0xff);
	nand->NFADDR = ((page>>8)&0xff);
	nand->NFADDR = ((page>>16)&0xff);
	nand->NFCMD = (flash_info.nand_cmd_set.read) & 0xff;
	nand_detect_rb();
	for(cnt=0;cnt<PAGE_SIZE;cnt++)
		r_buf[cnt] = NF_RDDATA8();
	nand_chip_disel();
	
}
uint8_t samsung_nand_program(nand_flash_addr_t *addr,uint8_t *buf,tag_t *tag)
{
	uint32_t cnt;
	uint32_t page;
	uint8_t nand_sts;
	uint8_t	ret;
	uint8_t size;
	uint8_t w_oob_buf[12];
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	//if(!addr || !buf || !tag)
	//	return FAIL;
	usb_mem_copy((uint8_t *)tag, (uint8_t *)&w_oob_buf[0], sizeof(tag_t));
	page = (addr->block <<flash_info.page_shift)+ addr->page;
	samsung_nand_reset();
	nand_chip_sel();
	nand_clear_rb();
	nand->NFCMD = (flash_info.nand_cmd_set.program>>8)&0xff;
	nand->NFADDR = 0;
	nand->NFADDR = 0;
	nand->NFADDR = (page&0xff);
	nand->NFADDR = ((page>>8)&0xff);
	//nand->NFADDR = ((page>>16)&0xff);

	for(cnt = 0;cnt < PAGE_SIZE;cnt++)
		NF_RDDATA8() = w_buf[cnt];
	for(cnt = 0;cnt < sizeof(tag_t); cnt++)
		NF_RDDATA8() = w_oob_buf[cnt];	
	nand->NFCMD = (flash_info.nand_cmd_set.program) & 0xff;
	nand_detect_rb();
	/*send sts*/
	nand->NFCMD = ((flash_info.nand_cmd_set.sts)&0xff);
	nand_sts = NF_RDDATA8();
	if(nand_sts & NAND_STATUS_FAIL)
	{
		ret = FAIL;
	}
	else
	{
		ret = SUCCESS;
	}
	nand_chip_disel();
	return ret;
	
}
void samsung_nand_read_oob(nand_flash_addr_t *addr,tag_t *tag)
{
	uint32_t cnt;
	uint32_t page;
	uint8_t size =sizeof(tag_t);
	uint16_t col = 2048;
	uint8_t buf[12];
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	
	page = (addr->block <<flash_info.page_shift)+ addr->page;
	samsung_nand_reset();
	nand_chip_sel();
	nand_clear_rb();
	nand->NFCMD = (flash_info.nand_cmd_set.read>>8)&0xff;
	nand->NFADDR = (col &0xff);
	nand->NFADDR = ((col>>8)&0xff);
	nand->NFADDR = (page&0xff);
	nand->NFADDR = ((page>>8)&0xff);
	
	nand->NFCMD = (flash_info.nand_cmd_set.read) & 0xff;
	nand_detect_rb();
	for(cnt=0;cnt<sizeof(tag_t);cnt++)
		buf[cnt] = NF_RDDATA8();
	nand_chip_disel();
	usb_mem_copy(buf, (uint8_t *)tag,sizeof(tag_t));

}
uint8_t samsung_nand_program_oob(nand_flash_addr_t *addr,uint8_t *buf)
{
	uint32_t cnt;
	uint32_t page;
	uint8_t nand_sts;
	uint8_t ret;
	uint16_t col = 2048;
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	page = (addr->block <<PAGE_SHIFT)+ addr->page;
	samsung_nand_reset();
	nand_chip_sel();
	nand_clear_rb();
	nand->NFCMD = (flash_info.nand_cmd_set.program>>8)&0xff;
	nand->NFADDR = (col & 0xff);
	nand->NFADDR = ((col >>8)&0xff);
	nand->NFADDR = (page&0xff);
	nand->NFADDR = ((page>>8)&0xff);
	
	for(cnt = 0;cnt < 2;cnt++)
		NF_RDDATA8() = w_oob_buf[cnt];
	nand->NFCMD = (flash_info.nand_cmd_set.program) & 0xff;
	nand_detect_rb();
	/*send sts*/
	nand->NFCMD = ((flash_info.nand_cmd_set.sts)&0xff);
	nand_sts = NF_RDDATA8();
	if(nand_sts & NAND_STATUS_FAIL)
	{
		ret = SUCCESS;
	}
	else
	{
		ret = FAIL;
	}	
	nand_chip_disel();
	return ret;

}

void scan_bad_block(uint8_t *buf)
{
	uint32_t page;
	uint16_t block;
	uint8_t ipage;
	uint8_t val;
	uint32_t cnt;
	uint8_t  stop_scan = 0;
	uint8_t  max_bad_blk_num = sizeof(nand_bbt)/sizeof(nand_bbt[0]);
	uint16_t col = 2048;
	
	S3C2440_NAND  *const nand = S3C2410_GetBase_NAND();
	if(!buf)
		return;
	samsung_nand_reset();
	nand_chip_sel();
	for(block = 0;((block < flash_info.block_num)&& !stop_scan);block++)
	{
		for(ipage = 0;ipage < 2;ipage++)
		{
			page = (block << flash_info.page_shift)+ ipage;
			nand_clear_rb();
			nand->NFCMD = (flash_info.nand_cmd_set.read>>8)&0xff;
			nand->NFADDR = (col & 0xff);
			nand->NFADDR = ((col >>8)&0xff);
			nand->NFADDR = (page&0xff);
			nand->NFADDR = ((page>>8)&0xff);
			nand->NFCMD = (flash_info.nand_cmd_set.read) & 0xff;
			nand_detect_rb();
			
			 val = NF_RDDATA8();
			 if((val & 0xff) != 0xff)
			 {
				nand_bbt[cnt++] = block;
				if(cnt > max_bad_blk_num)
				{
					stop_scan = 1;
				}
				break;
			 }
		}		

	}

	nand_chip_disel();
}
/*check wirte protected*/
uint8_t nand_check_wp(S3C2440_NAND  *const nand)
{
	uint8_t nand_sts;
	
	nand->NFCMD = (flash_info.nand_cmd_set.sts & 0xff);
	nand_sts = NF_RDDATA8();

	return (nand_sts & NAND_STATUS_WP);
		
}
/*test flash read program erase*/
void init_program_data()
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<512;)
		{
			w_buf[i*512+j++] = (i & 0xff);
			w_buf[i*512+j++] = ((i >>8)&0xff);
			w_buf[i*512+j++] = ((i>>16) & 0xff);
			w_buf[i*512+j++] = ((i>>24)&0xff);
		}
	for(i = 0;i<8;i++)
		w_oob_buf[i]=i;
}
void init_read_data()
{
	int i;
	for(i=0;i<2048;i++)
		r_buf[i] = 0;
	for(i=0;i<8;i++)
		r_oob_buf[i]=0;
}
void flash_test()
{
	uint32_t i;
	uint32_t page;
	nand_flash_addr_t addr;
	tag_t tag;
	
	addr.block = 10;
	addr.page = 0;
	//s3c2440_scan_bad_block(r_buf);
	samsung_nand_readid();
	for(i=0;i<PAGE_SIZE;i++)
		r_buf[i] = 0;
	#if 0
	samsung_nand_read((nand_flash_addr_t *)&addr,r_buf);
	samsung_nand_read_oob(&addr, &tag);
	addr.block = 10;
	addr.page = 1;
	samsung_nand_read((nand_flash_addr_t *)&addr,r_buf);
	samsung_nand_read_oob(&addr, &tag);
	addr.block = 10;
	addr.page = 2;
	samsung_nand_read((nand_flash_addr_t *)&addr,r_buf);
	samsung_nand_read_oob(&addr, &tag);
#endif

	
	samsung_nand_erase((nand_flash_addr_t *)&addr);
	for(i=0;i<8;i++)
		w_oob_buf[i]=i;
	#if 0
	s3c2440_nand_program_oob(&addr, w_oob_buf);
	s3c2440_nand_read_oob(&addr, &tag);
	
	s3c2440_nand_read((nand_flash_addr_t *)&addr,r_buf);
	#endif
	for(i=0;i<PAGE_SIZE;i++)
		w_buf[i] = i;
	flash_info.flash_op->nand_program((nand_flash_addr_t *)&addr,w_buf,&tag);
	for(i=0;i<PAGE_SIZE;i++)
		r_buf[i] = 0;
	flash_info.flash_op->nand_read((nand_flash_addr_t *)&addr,r_buf);
	flash_info.flash_op->nand_read_oob(&addr, &tag);
	
}
