#ifndef __NAND_FLASH_H__
#define __NAND_FLASH_H__
#include "head.h"
#define DISABLE_CHIP_SEL	1
#define ENABLE_CHIP_SEL		0
#define DISABLE_FLASH_CTL	0
#define ENABLE_FLASH_CTL	(1<<0)
#define INIT_ECC			(1<<4)
#define MAIN_ECC_LOCKED		(1<<5)
//#define MAIN_ECC_UNLOCKED	(0<<5)
#define SPARE_ECC_LOCKED	(1<<6)
//#define SPARE_ECC_UNLOCKED	(0<<6)
/*FLASH STATUS*/
#define RB_DETECTED			1
#define FLASH_MEM_BUSY		1
/* Status bits */
#define NAND_STATUS_FAIL		0x01
#define NAND_STATUS_FAIL_N1		0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY		0x40
#define NAND_STATUS_WP			0x80

/*5% bad block*/
#define MAX_BAD_BLOCK_NUM		51
#define CMD_ERASE			0
#define CMD_READ			1
#define CMD_PROGRAM			2
#define SUCCESS			0
#define FAIL			1

#define TACLS	1
#define TWRPH0	2
#define TWRPH1	0

typedef struct 
{
	uint16_t block;
	uint16_t page;
}nand_flash_addr_t;
typedef struct
{
	uint32_t serial_number;
	uint32_t lpn;
	uint16_t erase_cnt : 12;
	uint16_t block_state : 4;
	uint16_t finder_block;
}tag_t;
typedef struct{
	uint16_t reset;
	uint16_t readid;
	uint16_t erase;
	uint16_t read;
	uint16_t program;
	uint16_t sts;
	
}nand_cmd_set_t;
typedef struct
{
	uint8_t tacls : 2;
	uint8_t twrph0 : 3;
	uint8_t twrph1 : 3;
}nand_timing_t;
typedef struct
{
	//void(*nand_init)();
	void(*nand_readid)();
	uint8_t (*nand_read)(nand_flash_addr_t *addr,uint8_t * buf);
	uint8_t (*nand_program)(nand_flash_addr_t *addr,uint8_t *buf,tag_t *tag);
	uint8_t (*nand_erase)(nand_flash_addr_t *addr);
	void (*nand_reset)(void);
	void (*nand_read_oob)(nand_flash_addr_t *addr,tag_t *tag);
}flash_op_t;

typedef struct
{
	nand_cmd_set_t nand_cmd_set;
	nand_timing_t nand_timing;
	uint32_t block_num;
	uint32_t page_num;
	uint32_t page_size;
	uint8_t page_shift;
	uint8_t block_shift;
	flash_op_t *flash_op;
		
}flash_info_t;

#define PAGE_SHIFT		6
#define PAGE_SIZE		2048
#define BLOCK_NUM		1024
#define PAGE_NUM		64
void nand_init(void);
uint8_t samsung_nand_read(nand_flash_addr_t *addr,uint8_t *buf);
uint8_t samsung_nand_program(nand_flash_addr_t *addr,uint8_t *buf,tag_t *tag);
uint8_t samsung_nand_erase(nand_flash_addr_t *addr);
void samsung_nand_readid();
void samsung_nand_reset(void);
void scan_bad_block(uint8_t *buf);
uint8_t samsung_nand_program_oob(nand_flash_addr_t *addr,uint8_t *buf);
void samsung_nand_read_oob(nand_flash_addr_t *addr,tag_t *tag);
void  init_program_data();
void init_read_data();
void samsung_flash_cmd_set(nand_cmd_set_t * samsung_cmd_set);
void samsung_flash_timing(nand_timing_t * samsung_timing);

void flash_test(void);

__inline void nand_chip_sel(S3C2440_NAND  * const nand) 
{ 
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND(); 
	if(!nand)
		return;
	nand->NFCONT &= ~(DISABLE_CHIP_SEL<<1);
	
}
__inline void nand_reset_ecc(S3C2440_NAND  * const nand)
{
	if(!nand)
		return;
	nand->NFCONF |= INIT_ECC; 
}
__inline void nand_chip_disel(S3C2440_NAND  * const nand) 
{ 
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND(); 
	if(!nand)
		return;
	nand->NFCONT |= (DISABLE_CHIP_SEL<<1);
	
}

__inline void nand_detect_rb(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND(); 
	if(!nand)
		return;
	while(!(nand->NFSTAT & (RB_DETECTED<<2)))
			;
}
__inline void nand_clear_rb(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND(); 
	if(!nand)
		return;
	nand->NFSTAT |= (RB_DETECTED << 2);
}
__inline void nand_main_ecc_lock(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND();
	if(!nand)
		return;
	nand->NFCONT |= MAIN_ECC_LOCKED;

}
__inline void nand_main_ecc_unlock(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND();
	if(!nand)
		return;
	nand->NFCONT &= ~MAIN_ECC_LOCKED;
}
__inline void nand_spare_ecc_lock(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND();
	if(!nand)
		return;
	nand->NFCONT |= SPARE_ECC_LOCKED;

}
__inline void nand_spare_ecc_unlock(S3C2440_NAND  * const nand)
{
	//S3C2440_NAND  * const nand = S3C2410_GetBase_NAND();
	if(!nand)
		return;
	nand->NFCONT &= ~SPARE_ECC_LOCKED;
}
extern uint8_t r_buf[PAGE_SIZE];
extern uint8_t w_buf[PAGE_SIZE];
extern uint8_t r_oob_buf[12];
extern uint8_t w_oob_buf[12];
extern  flash_info_t flash_info;
extern uint8_t nand_bbt[MAX_BAD_BLOCK_NUM];
#endif
