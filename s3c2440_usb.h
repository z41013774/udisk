#ifndef __USB_H__
#define __USB_H__
#include "head.h"
//#include "testusb.h"



#define BUCK_ONLY_CMD_STAGE	0
#define BUCK_ONLY_IN_STAGE 	1
#define BUCK_ONLY_OUT_STAGE	2
#define BUCK_ONLY_STS_STAGE	3
#define BUCK_ONLY_IDLE		4


#define ENDPOINT0			0
#define ENDPOINT1			1
#define ENDPOINT2			2
#define ENDPOINT3			3

#define ENDPOINT1_SIZE		64
#define ENDPOINT3_SIZE		ENDPOINT1_SIZE

#define U_LINE	__inline
#define SECTOR			512
/*ep0 status*/
#define EP0_SERVICED_SETUP_END		(1<<7)
#define	EP0_SERVICED_OUTPUT_RDY		(1<<6)
#define EP0_SEND_STALL				(1<<5)
#define EP0_SETUP_END				(1<<4)
#define EP0_DATA_END				(1<<3)
#define EP0_SENT_STALL				(1<<2)
#define EP0_IN_PKT_RDY				(1<<1)
#define EP0_OUT_PKT_RDY				(1<<0)
 typedef enum {
		EP0 = 0,
		EP1,
		EP2,
		EP3
}epnum_t;
/*IN endpoint status*/
#define IN_CLR_DATA_TOGGLE			(1<<6)
#define IN_SENT_STALL				(1<<5)
#define IN_SEND_STALL				(1<<4)
#define IN_FIFO_FLUSH				(1<<3)
#define IN_IN_PKT_RDY				(1<<0)
#define IN_DMA_INT_MASK				(1<<4)
#define IN_MODE_IN					(1<<5)
#define OUT_MODE_IN					(0<<5)
#define IN_ISO						(1<<6)
#define IN_BUCK_MODE				(0<<6)
#define IN_AUTO_SET					(1<<7)
/*out endpoint sts*/
#define OUT_CLR_DATA_TOGGLE			(1<<7)
#define OUT_SENT_STALL				(1<<6)
#define OUT_SEND_STALL				(1<<5)
#define OUT_FIFO_FLUSH				(1<<4)
#define OUT_OUT_PKT_RDY				(1<<0)
#define OUT_AUTO_CLR				(1<<7)
#define OUT_ISO						(1<<6)
#define OUT_BUCK_MODE				(0<<6)
#define OUT_DMA_INT_MASK			(1<<5)


/*31 byte*/
#define CBW_SIZE					31
typedef  struct{
	uint8_t   dCBWSignature[4]; 
	uint8_t   dCBWTag[4]; 
	uint8_t   dCBWDataTransferLength[4]; /*byte */
	uint8_t   bmCBWFlags; 
	uint8_t   bCBWLUN; 
	uint8_t   bCBWCBLength; 
	uint8_t   CBWCB[16]; 
}usb_cbw_t;
/*13 byte*/
#define CSW_SIZE					13
typedef struct {
	uint8_t   dCSWSignature[4]; 
    uint8_t   dCSWTag[4]; 
    uint8_t   dCSWDataResidue[4]; 
    uint8_t   bCSWStatus;
	
}usb_csw_t;
typedef struct {
	uint8_t data[512];
	uint32_t lpn;
	uint8_t offset;
}data_node_t;

/*usb int sts*/
#define USB_DEV_RESET		(1<<2)
#define USB_DEV_RESUME		(1<<1)
#define USB_DEV_SUSPEND		(1<<0)
static void usb_cpu_init(void);
void usb_process(void);
/**/
 void usb_h2d(void); //ep3
 void usb_d2h(void);	//ep1
/*all usb irq*/
void __irq usb_isr(void);
static void usb_reg_config(void);
void usb_sel_endpoint(uint32_t index);

/*clear ep1 status*/
void clear_ep1_sent_stall(void);
/*set ep1 status*/
void set_ep1_in_pkt_rdy(void);
void set_ep1_send_stall(void);
/*clear ep3 status*/
void clear_ep3_sent_stall(void);
void clear_ep3_out_pkt_rdy(void);
/*set ep3 status*/
void set_ep3_send_stall(void);
void usb_init(void);
void dump_usb_reg();
void usb_intr_enable();
 void usb_rcv_cmd();
static void usb_rcv_data();
void usb_process_cmd();
void usb_send_csw(void);
void usb_send_data(void);
void usb_buckonly_wait_cmd(void);
void usb_buckonly_out(void);
void usb_buckonly_in(void);
void usb_buckonly_sts(void);
void usb_read_fifo(uint8_t *host_data);
static void usb_put_one_sector(uint8_t *host_data);
static void usb_get_one_sector(uint8_t *host_data);

void usb_set_fifo(uint8_t *fifo,uint32_t size);
void usb_get_fifo(uint8_t *fifo,uint32_t size);


void init_512byte_data(char *data,int sec_num,int lba);

extern volatile uint8_t 	host_recv_flag;
extern volatile uint8_t	host_trans_flag ;
extern usb_cbw_t	usb_cbw;
extern volatile uint8_t 	buckonly ;

//extern volatile uint8_t	data_cache[PAGE_SIZE];

__inline void usb_intr_enable()
{
	S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();
	intr->INTMSK &= ~(BIT_USBD);
}
#endif
