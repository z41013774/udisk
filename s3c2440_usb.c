/**************************************************
author : zhaohuawei      

file   : s3c2440_usb.c

date   : 2015-09-09        
***************************************************/
#include "head.h"
#include "s3c2440_usb_util.h"
#include "s3c2440_usb.h"
#include "s3c2440_usb_scsi.h"
#include "s3c2440_usb_desc.h"
#include "testusb.h"
#include "s3c2440_ftl.h"
#include "s3c2440_error.h"

#define TRANS_PACKET_CNT_PER	8
#define PACKET_SIZE_PER		512

volatile uint8_t 	host_recv_flag = 0;
volatile uint8_t	host_trans_flag = 0;
volatile uint32_t	lba;
volatile uint32_t	sec_cnt;

//volatile uint8_t	data_cache[PAGE_SIZE];
volatile uint8_t 	buckonly ;
 usb_cbw_t	usb_cbw;
 usb_csw_t  usb_csw;
void clear_ep3_out_pkt_rdy(void)
{
  S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
  usb_device->OUT_CSR1_REG &= (~EPO_WR_BITS & (~(1<<0)));
}
void clear_ep3_sent_stall(void)
{
 S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
 usb_device->OUT_CSR1_REG &= (~EPO_WR_BITS & (~EPO_SENT_STALL));
}

void __irq usb_isr(void)
{
	uint8_t usbintsts,epintsts;
	S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	uint8_t saveIndexReg=usb_device->INDEX_REG;
	
	usbintsts = usb_device->USB_INT_REG;
	epintsts = usb_device->EP_INT_REG;
	switch(usbintsts)
	{
		case USB_DEV_SUSPEND:
			usb_device->USB_INT_REG = USB_DEV_SUSPEND;
			USB_TRACE("sus");
			break;
		case USB_DEV_RESUME:
			usb_device->USB_INT_REG = USB_DEV_RESUME;
			USB_TRACE("rsm");
			break;
		case USB_DEV_RESET:
			//USB_TRACE("rst\r\n");
			usb_reg_config();
			usb_device->USB_INT_REG = USB_DEV_RESET;
			break;
		default:
			break;
	}
	switch(epintsts)
	{
		case 1:
			//usb_setup_desc();
			Ep0Handler();
			usb_device->EP_INT_REG = (1<<0);
			break;
		case 2:
			//usb_d2h();
			/*dev to host*/
			host_recv_flag = 1;
			usb_device->EP_INT_REG = (1<<1);
			break;
		case 8:
			//usb_h2d();
			/*host to dev*/
			host_trans_flag = 1;
			usb_device->EP_INT_REG = (1<<3);
			break;
		default:
			break;
			
	}
	intr->SRCPND |=(BIT_USBD);
	intr->INTPND |=(BIT_USBD);
	usb_device->INDEX_REG = saveIndexReg;
}

void usb_buckonly_wait_cmd()
{
	if((buckonly == BUCK_ONLY_IDLE)& host_trans_flag)
	{
		/*clear host trans flag*/
		host_trans_flag = 0;
		buckonly = BUCK_ONLY_CMD_STAGE;
	}
	
}
void usb_buckonly_cmd()
{
	uint8_t cnt;
	uint32_t data_len;
	uint8_t cbw[CBW_SIZE];
	#if 1
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	/*select ep3*/
	usb_device->INDEX_REG = ENDPOINT3;

	/*cbw cmd*/	
	for(cnt = 0; cnt < CBW_SIZE; cnt++)
	{
		cbw[cnt] = usb_device->fifo[ENDPOINT3].EP_FIFO_REG;
	}
	#endif
	//usb_get_fifo((uint8_t *)&cbw,CBW_SIZE);
	usb_mem_copy(cbw,(uint8_t *)&usb_cbw,CBW_SIZE);
	data_len = (usb_cbw.dCBWDataTransferLength[3]<<24)| \
			   (usb_cbw.dCBWDataTransferLength[2]<<16)| \
			   (usb_cbw.dCBWDataTransferLength[1]<<8)| \
			   (usb_cbw.dCBWDataTransferLength[0]);
	/*calculate sec cnt*/
	if((data_len > 0)&&(data_len <= PACKET_SIZE_PER))
	{
		sec_cnt = 1;
	}
	else if(data_len == 0)
	{
		sec_cnt = 0;
	}
	else
	{
		sec_cnt = (data_len / PACKET_SIZE_PER);
		if(data_len % PACKET_SIZE_PER)
			sec_cnt ++;
	}
	if(((usb_cbw.CBWCB[0] == 0x2a)||(usb_cbw.CBWCB[0] == 0x28))&&(sec_cnt))
	{
		lba = (usb_cbw.CBWCB[5]<<24)|(usb_cbw.CBWCB[4]<<16)| \
			(usb_cbw.CBWCB[3]<<8)|(usb_cbw.CBWCB[2]);
	}
//	USB_TRACE(" cmd 0x%2x\r\n",usb_cbw.CBWCB[0]);
	
	switch(usb_cbw.CBWCB[0])
	{
		case REQUEST_SENSE:
			resp_requset_sense();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case INQUIRY:
			resp_inquiry();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case MODE_SENSE:
			resp_mode_select();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case ALLOW_MEDIUM_REMOVAL:
			resp_allow_memium_removal();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case READ_FORMAT_CAPACITY:
			resp_read_format_capacity();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case READ_CAPACITY:
			resp_read_capacity();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case READ_10:
			buckonly = BUCK_ONLY_IN_STAGE;
			break;
		case WRITE_10:
			buckonly = BUCK_ONLY_OUT_STAGE;
			break;
		case VERIFY:
			resp_verify();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		case MODE_SENSE_10:
			resp_mode_sense10();
			buckonly = BUCK_ONLY_STS_STAGE;
			break;
		default:
			break;
			
	}
	//USB_TRACE("buckonly out cmd 0x%2x\r\n",usb_cbw.CBWCB[0]);

}
void usb_process_cmd()
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
		
	usb_device->INDEX_REG = ENDPOINT3;
	//USB_TRACE("Endpoint 3 ,output\r\n");
	if(usb_device->OUT_CSR1_REG & OUT_OUT_PKT_RDY)
	{
		usb_buckonly_cmd();
		//USB_TRACE("scan cmd done\r\n");
		clear_ep3_out_pkt_rdy();
	}
	else if(usb_device->OUT_CSR1_REG & EPO_SENT_STALL)
	{
		clear_ep3_sent_stall();
	}

}

//volatile uint8_t host_data[PACKET_SEIZE_PER];
static void usb_get_one_sector(uint8_t *host_data)
{
	uint8_t fifo_cnt;
	uint8_t cnt;
	uint16_t recv_data_cnt=0;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->INDEX_REG = ENDPOINT3;

	while(recv_data_cnt < PACKET_SIZE_PER)
	{
		while(!host_trans_flag)
			;
		host_trans_flag = 0;
		#if 1
		if(usb_device->OUT_CSR1_REG & OUT_OUT_PKT_RDY)
		{
			fifo_cnt = usb_device->OUT_FIFO_CNT1_REG;
			for(cnt = 0; cnt < fifo_cnt; cnt++)
			{
				host_data[recv_data_cnt] = usb_device->fifo[ENDPOINT3].EP_FIFO_REG;
				recv_data_cnt ++;
			}
			clear_ep3_out_pkt_rdy();
		}
		#endif
		//usb_get_fifo((uint8_t *)&host_data[recv_data_cnt * ENDPOINT3_SIZE],ENDPOINT3_SIZE);
		//recv_data_cnt++;
	}	

}
void usb_buckonly_out(void)
{
		
	uint32_t lpn;
	uint32_t offset;
	uint8_t host_data[PACKET_SIZE_PER];
	uint32_t trans_sec_cnt =0;
	uint32_t rev_data_len = 0;

	usb_fill_buf(w_buf,0xFFFFFFFF,PAGE_SIZE);
	while(sec_cnt != trans_sec_cnt)
	{
		/*512byte per*/		
		usb_get_one_sector((uint8_t *)&host_data);
		
		/*record transmitted  sec cnt*/
		trans_sec_cnt++;
		/*-------------------------------------------------------------
		1 lba0 sects 4 ,align and program.
		   sec0 sec1 sec2 sec3
		2 lba0 sect1,2,3,unaligned and program. remain data set FFFF.
		   sec0,FFFF,FFFF,FFF or sec0 sec1 FFFF FFFF
		3 lba1 sects 4 data will be program in two page.
		   FFFF sec1 sec2 sec3,sec4 FFFF FFFF FFFF
		4 lba1 sect 1 unallgned and program ,remain data set FFFF
		   FFFF sec0 FFFF FFFF
		---------------------------------------------------------------*/
		offset = (lba & 0x3);
		rev_data_len = offset;
		usb_mem_copy((uint8_t *)&host_data, (uint8_t *)&w_buf[rev_data_len*PACKET_SIZE_PER],PACKET_SIZE_PER);
		
		rev_data_len++;
		/*meet four LBA or trans finished*/
		if((rev_data_len == 4)||(sec_cnt == trans_sec_cnt))
		{
			/*mask 2bit*/
			lpn = (lba >>2);
			
			usb_write(lpn,(uint8_t *)w_buf[0]);
			rev_data_len = 0;
		}
		lba++;
	}
	/*trans finish or no data*/
	buckonly = BUCK_ONLY_STS_STAGE;
}
static void usb_put_one_sector(uint8_t *host_data)
{

	uint16_t cnt;
	uint16_t	trans_data_num=0;

	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->INDEX_REG = ENDPOINT1;
	/*transmit 512byte per*/
	do{
		#if 1
		for(cnt = 0;cnt < 64;cnt++)
		{
			usb_device->fifo[ENDPOINT1].EP_FIFO_REG = host_data[trans_data_num];
			trans_data_num++;
		}
				
		/*fifo ready*/
		usb_device->EP0_CSR_IN_CSR1_REG &= (~EPI_WR_BITS) ;
		usb_device->EP0_CSR_IN_CSR1_REG |= IN_IN_PKT_RDY;
		#endif
		//usb_set_fifo((uint8_t *)host_data[trans_data_num *ENDPOINT1_SIZE],ENDPOINT1_SIZE);
		//trans_data_num++;
		host_recv_flag = 0;
		while(!host_recv_flag)
			;
		if(trans_data_num == PACKET_SIZE_PER)
			break;				
	}while(1);

}

void usb_buckonly_in()
{
	
	uint8_t i=0;
	uint32_t lpn;
	uint8_t offset;
	uint32_t trans_data_cnt;
	uint8_t host_data[PACKET_SIZE_PER];
	uint32_t prev_lpn = 0xEfffffff;
	
	usb_fill_buf(r_buf,0xFFFFFFFF,PAGE_SIZE);
	while(sec_cnt != trans_data_cnt)
	{
		//init_512byte_data((char *)&host_data, 1, lba);
		lpn = (lba >>2);
		offset = (lba & 0x3);
		if(prev_lpn != lpn)
		{
			usb_read(lpn,(uint8_t *)&r_buf[0]);
			prev_lpn = lpn;
		}
		usb_mem_copy((uint8_t *)&r_buf[offset*PACKET_SIZE_PER],(uint8_t *)&host_data,PACKET_SIZE_PER);
		/*trans 512 byte*/	
		usb_put_one_sector((uint8_t *)&host_data);
		lba++;
		trans_data_cnt++;		
	}
	/*trans finish or no data*/
	buckonly = BUCK_ONLY_STS_STAGE;
}
void usb_buckonly_sts()
{
	
	usb_csw.dCSWSignature[0] = 0x55;   
	usb_csw.dCSWSignature[1] = 0x53;   
	usb_csw.dCSWSignature[2] = 0x42;   
	usb_csw.dCSWSignature[3] = 0x53;   
	usb_csw.dCSWTag[0] = usb_cbw.dCBWTag[0];   
	usb_csw.dCSWTag[1] = usb_cbw.dCBWTag[1];   
	usb_csw.dCSWTag[2] = usb_cbw.dCBWTag[2];   
	usb_csw.dCSWTag[3] = usb_cbw.dCBWTag[3];  
	usb_csw.dCSWDataResidue[0] = 0;
	usb_csw.dCSWDataResidue[1] = 0;
	usb_csw.dCSWDataResidue[2] = 0;
	usb_csw.dCSWDataResidue[3] = 0;
	usb_csw.bCSWStatus = 0; 
	usb_set_fifo((uint8_t *)&usb_csw,sizeof(usb_csw_t));
		
	buckonly = BUCK_ONLY_IDLE;

}
void usb_set_fifo(uint8_t *fifo,uint32_t size)
{
	uint16_t cnt;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->INDEX_REG = ENDPOINT1;
	for(cnt = 0;cnt < size;cnt++)
	{
		usb_device->fifo[ENDPOINT1].EP_FIFO_REG = fifo[cnt];
	}
	/*set in_fifo ready*/
	usb_device->EP0_CSR_IN_CSR1_REG &= ~(IN_CLR_DATA_TOGGLE|IN_FIFO_FLUSH|IN_IN_PKT_RDY);
	usb_device->EP0_CSR_IN_CSR1_REG |= IN_IN_PKT_RDY;

}
void usb_get_fifo(uint8_t *fifo,uint32_t size)
{
	uint32_t fifo_cnt,cnt;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->INDEX_REG = ENDPOINT3;
	
	if(usb_device->OUT_CSR1_REG & OUT_OUT_PKT_RDY)
	{
		//fifo_cnt = usb_device->OUT_FIFO_CNT1_REG;
		for(cnt = 0; cnt < size; cnt++)
		{
			fifo[cnt] = usb_device->fifo[ENDPOINT3].EP_FIFO_REG;
			cnt ++;
		}
		clear_ep3_out_pkt_rdy();
	}

}
void init_512byte_data(char *data,int sec_num,int lba)
{
	int j;
	int i;
	//for(i=0;i<sec_num;j++)
	{
		for(j=0;j<512;)
		{
			data[j++] = (lba & 0xff);
			data[j++] = ((lba >>8)&0xff);
			data[j++] = ((lba>>16) & 0xff);
			data[j++] = ((lba >>24)&0xff);
		}
	}
}

static void usb_cpu_init(void)
{
	S3C24X0_GPIO  *const gpio =S3C24X0_GetBase_GPIO();
	gpio->GPCCON &= ~(3<<10);
	gpio->GPCCON |= (1<<10);
	gpio->GPCUP |=(1<<5);
	gpio->GPCDAT |=(1<<5);

	gpio->MISCCR &=~(1<<3);  // USBD is selected instead of USBH1 
	gpio->MISCCR &=~(1<<13); // USB port 1 is enabled.

	gpio->DSC0 = 0x2aa;
	gpio->DSC1 = 0x2aaaaaaa;
	/*usb device intr*/	
	pISR_USBD = (unsigned)usb_isr;
	USB_TRACE("init usb gpio done\r\n");
}

static void usb_reg_config(void)
{

	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->PWR_REG = 0x0;
	/*sel endpoint0*/
	usb_device->INDEX_REG = ENDPOINT0;
	usb_device->MAXP_REG = 0x1;
	/*clear bit7,bit6*/
	usb_device->EP0_CSR_IN_CSR1_REG = EP0_SERVICED_OUTPUT_RDY | EP0_SERVICED_SETUP_END;

	/*sel endpoint1*/
	usb_device->INDEX_REG = ENDPOINT1;
	/*packet 64byte*/
	usb_device->MAXP_REG = 0x8;
	usb_device->EP0_CSR_IN_CSR1_REG = IN_FIFO_FLUSH | IN_CLR_DATA_TOGGLE;
	usb_device->IN_CSR2_REG = IN_MODE_IN | IN_DMA_INT_MASK | IN_BUCK_MODE;
	usb_device->OUT_CSR1_REG = OUT_CLR_DATA_TOGGLE;
	usb_device->OUT_CSR2_REG = OUT_BUCK_MODE | OUT_DMA_INT_MASK;

	/*sel endpoint3*/
	usb_device->INDEX_REG = ENDPOINT3;
	/*64byte per packet*/
	usb_device->MAXP_REG = 0x8;
	usb_device->EP0_CSR_IN_CSR1_REG = IN_FIFO_FLUSH | IN_CLR_DATA_TOGGLE;
	usb_device->IN_CSR2_REG = OUT_MODE_IN | IN_DMA_INT_MASK | IN_BUCK_MODE;
	usb_device->OUT_CSR1_REG = OUT_CLR_DATA_TOGGLE;
	usb_device->OUT_CSR2_REG = OUT_BUCK_MODE | OUT_DMA_INT_MASK;
	/*clear ep0..4 intr*/
	usb_device->EP_INT_REG = (1<<3)|(1<<1)|(1<<0);
	usb_device->USB_INT_REG = 0x07;
	/*enable ep0..4 intr*/
	usb_device->EP_INT_EN_REG = (1<<3)|(1<<1)|(1<<0);
	usb_device->USB_INT_EN_REG = 0x04;

}
void usb_init(void)
{
	usb_cpu_init();
	InitDescriptorTable();
	//usb_init_desc_table();
	//dump_usb_reg();
	usb_reg_config();
	usb_intr_enable();
		
	isUsbdSetConfiguration = 0;
	is_usb_set_interface = 0;

	host_recv_flag = 0;
	host_trans_flag = 0;
	
	buckonly = BUCK_ONLY_CMD_STAGE;
	ep0State=EP0_STATE_INIT;	
}


