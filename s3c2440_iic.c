#include "head.h"
#define ACKENABLE	(1<<7)
#define TXRXINTR	(1<<5)
#define MASTERTRASN (3<<6)
#define MASTERRECV	(2<<6)
#define SLAVEADDR	0xa0

void iic_init()
{
	S3C24X0_I2C  * const i2c = S3C24X0_GetBase_I2C();
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	gpio->GPEUP |= 0xc000;
	gpio->GPECON |= 0xa00000;
	i2c->IICCON =ACKENABLE|(0<<6)|TXRXINTR|0xf;
	i2c->IICADD = 0x10;
	i2c->IICSTAT = 0x10;
		 
}
/*IIC trans data*/
void iic_master_trans_one_char(struct _at24c08_addr_data at24c08_addrdata)
{
	int i;
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	
	//send iic slave addr
	iic_xfer_byte(0xa0);
	i2c->IICSTAT=0xF0;
	iic_check_sts();
	
	/*at24c08 eeprom addr*/
	iic_xfer_byte(at24c08_addrdata.addr);
	for(i=0;i<10;i++)
		;
	/*clear intr flag,write 1 to clear*/
	i2c->IICCON &= ~(1<<4); 
	iic_check_sts();

	iic_xfer_byte(at24c08_addrdata.data);
	for(i=0;i<10;i++)
		;
	i2c->IICCON &= ~(1<<4); 
	iic_check_sts();
	iic_master_trans_stop();
	
}
void iic_check_ack()
{
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	
	while(i2c->IICSTAT &(1<<0))
	{
		delay_ms(1);
	}
}
void iic_master_trans_stop()
{
	S3C24X0_I2C  * const i2c =S3C24X0_GetBase_I2C();

	i2c->IICSTAT = 0xD0;
	i2c->IICCON &= ~(1<<4); 
	delay_ms(1);
}
/*iic recv data*/
uint8_t iic_master_recv_one_char(uint8_t addr)
{
	uint8_t data;
	int i;
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	
	/*dummy write*/
	//send iic slave addr
	iic_xfer_byte(0xa0);
	i2c->IICSTAT=0xF0;
	iic_check_sts();
	//i2c->IICCON &= ~(1<<4);
	iic_xfer_byte(addr);
	
	//i2c->IICCON &= ~(1<<4); 
	for(i=0;i<10;i++)
		;
	//i2c->IICCON = 0xaf; 
	i2c->IICCON &= ~(1<<4);
	//iic_check_ack();
	iic_check_sts();
	/*clear intr flag,write 1 to clear*/
	
	
	/*begin read eepom data*/
	iic_xfer_byte(0xa0);

	i2c->IICSTAT=0xB0;
	//i2c->IICCON = 0xaf; 
	i2c->IICCON &= ~(1<<4);

	iic_check_sts();
	//fist addr is output
	
	data = iic_recv_byte();
	/*clear intr flag,write 1 to clear*/
	i2c->IICCON = 0x2f; 
	iic_check_sts();

	
	/*clear intr flag,write 1 to clear*/
	
	//data is output
	data = iic_recv_byte();
	/*stop*/
	iic_master_recv_stop();
	return data;
}
void iic_master_recv_stop()
{

	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	i2c->IICSTAT = 0x90;
	/*clear intr flag,write 1 to clear*/
	i2c->IICCON &= ~(1<<4); 
	delay_ms(1);
	
}
void iic_xfer_byte(uint8_t byte)
{
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	i2c->IICDS = byte;
	/*clear pending flag*/
	//i2c->IICCON &=~(1<<4); 
}
uint8_t iic_recv_byte()
{
	uint8_t data;	
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	data =i2c->IICDS; 
		/*clear pending flag*/
	//i2c->IICCON &=~(1<<4); 
	return data;
}
void iic_check_sts()
{
	S3C24X0_I2C * const i2c = S3C24X0_GetBase_I2C();
	while(!(i2c->IICCON &(1<<4)))
		;
	
}
void iic_test()
{
	uint8_t cnt;
	uint8_t data;
	struct _at24c08_addr_data  at24c08_addr_data;
	iic_init();
	for(cnt=0;cnt<4;cnt++)
	{
		at24c08_addr_data.addr =cnt;
		at24c08_addr_data.data = (cnt+2);
		iic_master_trans_one_char(at24c08_addr_data);
	}
	//read
	for(cnt=0;cnt<256;cnt++)
	{
		at24c08_addr_data.addr = cnt;
		data=iic_master_recv_one_char(cnt);
		dbg_printf("iic data :%d\r\n",data);
	}
	
}
