#include "s3c2440_usb_scsi.h"
#include "s3c2440_usb.h"
#include "s3c2440_error.h"

#define CALCSIZE(a,b)	((a)/(b))
/*refer to scsi primary commands -3(spc-3)*/
const uint8_t scsi_inquiry[RESP_INQURIY_COUNT]= 
{ 
 0x00,0x80,0x00,0x01,0x1f,0x00,0x00,0x00, 
 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 
 0x43,0x48,0x45,0x4e,0x43,0x48,0x49,0x20, 
 0x41,0x56,0x52,0x20,0x44,0x49,0x53,0x4b, 
 0x31,0x2e,0x30,0x30  
 };
 
 const uint8_t scsi_datareadformatcapacities[12]= 
 { 
  0x00,//reserved 
  0x00,//reserved 
  0x00,//reserved 
   
  0x08,//Capacity List Length 
   
  0x00, //number of block	MSB 
  0x00, //number of block	 | 
  0xff, //number of block	 | 
  0xff, //number of block	LSB // 256MBYTE 
   
  0x03, //no cartridge in drive 
   
  0x00,  // length of block// 10 
  0x02, // 
  0x00//   
  }; 
  
 const uint8_t scsidata_readcapacity[8]= 
 { 
  0x00, //last logic block address	 MSB 
  0x00, //last logic block address	  | 
  0xf9, //last logic block address	  | 
  0xff,//last logic block address	LSB   
   
  0x00, //length of block	MSB 
  0x00, //length of block	 | 
  0x02, //length of block	 | 
  0x00//length of block   LSB 
 }; 
 const uint8_t scsidata_requestsense[18]= 
 { 
	0x70, 
	  
	0x00, 
	0x05, 
	 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	 
	0x0a,  
	 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	 
	0x20, 
	0x00, 
	 
	0x00, 
	0x00, 
	0x00, 
	0x00 
 }; 
 
 const uint8_t scsidata_sense[48]= 
 { 
	0x03, 
	0x00, 
	0x00, 
	0x00, 
   /*  
	0x00, 
	0x00, 
	0x00, 
	0x00,	 
	0x00, 
	0x00, 
	0x02, 
	0x00, 
	*/ 
	0x01, 
	0x0a, 
	0x00, 
	0x10,	 
	0x00, 
	0x00, 
	0x00, 
	0x00,	 
	0x02, 
	0x00, 
	0x00, 
	0x00, //page 1	 
	0x05,//page 2 
	0x1e, 
	0x0f, 
	0xa0,	 
	0x04, 
	0x10, 
	0x02, 
	0x00,	 
	0x00, 
	0xfa, 
	0x00, 
	0x00,	 
	0x00, 
	0x00, 
	0x00, 
	0x00,	 
	0x00, 
	0x00, 
	0x00, 
	0x00,	 
	0x00, 
	0x00, 
	0x00, 
	0x00,	
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00 
 }; 


void resp_test_uint_ready()
{

}
void  resp_requset_sense()
{
	usb_set_fifo((uint8_t *)&scsidata_requestsense,CALCSIZE(sizeof(scsidata_requestsense),sizeof(scsidata_requestsense[0])));
}
void resp_inquiry(void)
{
	usb_set_fifo((uint8_t *)&scsi_inquiry,CALCSIZE(sizeof(scsi_inquiry),sizeof(scsi_inquiry[0])));

}
void resp_mode_select(void)
{

}
void resp_allow_memium_removal()
{

}
void resp_read_format_capacity()
{
	usb_set_fifo((uint8_t *)&scsi_datareadformatcapacities,CALCSIZE(sizeof(scsi_datareadformatcapacities),sizeof(scsi_datareadformatcapacities[0])));
	USB_TRACE("farmat data trans done\r\n");
}
void resp_read_capacity()
{
	usb_set_fifo((uint8_t *)&scsidata_readcapacity,CALCSIZE(sizeof(scsidata_readcapacity),sizeof(scsidata_readcapacity[0])));
}

void resp_read10(void)
{
	//usb_buckonly_in();
}
void resp_write10(void)
{
	//usb_buckonly_out();
}
void resp_verify(void)
{

}
void resp_mode_sense10()
{
	usb_set_fifo((uint8_t *)&scsidata_sense,CALCSIZE(sizeof(scsidata_sense),sizeof(scsidata_sense[0])));
}
