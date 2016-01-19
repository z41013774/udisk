#ifndef __IIC_H__
#define __IIC_H__
#include "head.h"

struct _at24c08_addr_data
{
	uint8_t addr;
	uint8_t data;
};
//extern struct _at24c08_addr_data slave_addr_data;
void iic_check_ack(void);
void iic_master_trans_stop(void);
void iic_master_recv_stop(void);


void iic_xfer_byte(uint8_t byte);
uint8_t iic_recv_byte(void);
void iic_init(void);
void iic_master_trans_start(void);
void iic_master_recv_start(void);
void iic_check_sts(void);
uint8_t iic_master_recv_one_char(uint8_t addr);
void iic_master_trans_one_char(struct _at24c08_addr_data at24c08_addrdata);

#endif
