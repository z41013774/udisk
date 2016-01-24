#ifndef _FTL_H__
#define _FTL_H__
#include "head.h"
/*2K mapping*/
/*current flash*/
/*2k page,64 page per block,1024 block per dev*/
/*4 lba per page,lba 512byte*/
/*4 lba per lpn */
#define INVAILD_SERIAL_NUMBER		0xFFFFFFFF
#define INVAILD_BLOCK_NUMBER		0xFFFF
#define INVAILD_PPN					0xFFFFFFFF
#define BBT_BLOCK0					0x8
#define BBT_BLOCK1					0x9
typedef enum {
	BLOCK_STATE_UNKNOWN = 0,

	BLOCK_STATE_SCANNING,
	BLOCK_STATE_EMPTY,
	BLOCK_STATE_ALLOCATING,
	BLOCK_STATE_FULL,
	BLOCK_STATE_DIRTY,
	BLOCK_STATE_COLLECTING,	
	BLOCK_STATE_DEAD
} block_state;

typedef struct
{
	uint32_t dirty_cnt : 8;/*64 * 4 lba*/
	uint32_t erase_cnt : 12;
	uint32_t block_state : 4;
}block_info_t;
typedef struct
{
	uint16_t block_num;
	uint16_t serial_number;
}block_index_t;
typedef struct
{
	 uint32_t allocation_block;
	 uint32_t allocation_page;
	 uint32_t finder_block;
	 uint32_t gc_block;
	 uint32_t gc_page;
	 uint32_t nerased_blocks;
	 uint32_t erased_block;
	 uint32_t start_block;
	 uint32_t end_block;
	 uint32_t bad_table_block;
	 uint32_t bad_table_page;
	 uint32_t nreserved_blocks;
}usb_dev_t;

extern volatile uint32_t serial_number;
//extern  dev_t dev;
void scan_storage(void);//recovery l2p table
void usb_read(uint32_t lpn,uint8_t *buf);
uint8_t usb_write(uint32_t lpn,uint8_t *buf);
static uint32_t usb_find_block_for_allocation(void);
static uint32_t usb_grabage_collection_block(uint32_t block);
block_info_t *get_block_info(uint32_t block);
static void insertion_sort(block_index_t *block_index,uint16_t block_num);
static void update_bbt(uint32_t block_num);
static void get_availd_block(void);

extern volatile block_info_t *blk_info;
extern volatile uint32_t *usb_l2p;


#endif
