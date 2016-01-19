#ifndef __USB_DESC_H__
#define __USB_DESC_H__
#include "s3c2440.h"
#define IN	0x80
#define OUT	0x00
#if 0
#define GET_STATUS						0
#define CLEAR_FEATURE					1
#define	RESERVED						2
#define	SET_FEATURE						3
#define SET_ADDRESS						5
#define	GET_DESCRIPTOR					6
#define	SET_DESCRIPTOR					7
#define	GET_CONFIGURATION				8
#define SET_CONFIGURATION				9
#define GET_INTERFACE					10
#define SET_INTERFACE					11
#define SYNCH_FRAME						12

#define DEVICE_TYPE						1
#define CONFIGURATION_TYPE				2
#define STRING_TYPE						3
#define INTERFACE_TYPE					4
#define ENDPOINT_TYPE					5

//string descriptor
#define LANGID_US_L 		    (0x09)  
#define LANGID_US_H 		    (0x04)
#endif

/*setup data*/
struct usb_setup_data
{ 
	uint8_t bmRequestType;    
    uint8_t bRequest;         
    uint8_t bValueL;          
    uint8_t bValueH;          
    uint8_t bIndexL;          
    uint8_t bIndexH;          
    uint8_t bLengthL;         
    uint8_t bLengthH;    
};
/*ep0 status*/

struct usb_dev_desc
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bcdUSBL;
	uint8_t		bcdUSBH;
	uint8_t		bDeviceClass;
	uint8_t		bDeviceSubClass;
	uint8_t		bDeviceProtocol;
	uint8_t		bMaxPacketSize0;
	uint8_t		idVendorL;
	uint8_t		idVendorH;
	uint8_t		idProductL;
	uint8_t		idProductH;
	uint8_t		bcdDeviceL;
	uint8_t		bcdDeviceH;
	uint8_t		iManufacturer;
	uint8_t		iProduct;
	uint8_t		iSerialNumber;
	uint8_t		bNumConfigurations;
};
struct usb_configuration_desc
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		wTotalLengthL;
	uint8_t		wTotalLengthH;
	uint8_t		bNumInterfaces;
	uint8_t		bConfigurationValue;
	uint8_t		iConfiguration;
	uint8_t		bmAttributes;
	uint8_t		bMaxPower;
};
struct usb_interface_desc
{
	uint8_t	bLength;
	uint8_t	bDescriptorType;
	uint8_t	bInterfaceNumber;
	uint8_t	bAlternateSetting;
	uint8_t	bNumEndpoints;
	uint8_t	bInterfaceClass;
	uint8_t	bInterfaceSubClass;
	uint8_t	bInterfaceProtocol;
	uint8_t	iInterface;
};
struct usb_endpoint_desc
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bEndPointAddress;
	uint8_t		bmAttributes;
	uint8_t		wMaxPacketSizeL;
	uint8_t		wMaxPacketSizeH;
	uint8_t		bInterval;
};
struct usb_string_desc
{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	wLangid[5];
};
/*dev configure endpoint status*/
typedef struct _usb_status
{
	uint8_t device;
	uint8_t interface;
	uint8_t endpoint0;
	uint8_t endpoint1;
	uint8_t endpoint3;
}usb_status_t;
#define CONF_ATTR_DEFAULT	    	(0x80) //Spec 1.0 it was BUSPOWERED bit.
#define CONF_ATTR_REMOTE_WAKEUP     (0x20)
#define CONF_ATTR_SELFPOWERED       (0x40)
#define EP0_PKT_SIZE        8	
#define EP1_PKT_SIZE		64
#define EP3_PKT_SIZE		64

#define EP_ATTR_CONTROL		    (0x0)	
#define EP_ATTR_ISOCHRONOUS	    (0x1)
#define EP_ATTR_BULK		    (0x2)
#define EP_ATTR_INTERRUPT	    (0x3)

void usb_init_desc_table(void);
void usb_setup_desc(void);//ep0
void put_desc_pkt(uint8_t *desc,uint8_t size);
void get_desc_pkt(uint8_t *p);


/*desc func*/
void clear_feature(void);
void get_configuration(void);
void get_descriptor(void);
void get_interface(void);
void get_status(void);
void set_address(void);
void set_configuration(void);
void set_descriptor(void);
void set_feature(void);
void set_interface(void);
void synch_frame(void);
/*desc type*/
void get_dev_type(void);
void get_configuration_type(void);
void get_str_type(void);
void get_interface_type(void);
void get_endpoint_type(void);

/*clear ep0 status*/
void clear_ep0_out_pkt_rdy(void);
void clear_ep0_sent_stall(void);
void clear_ep0_outpkt_dataend(void);
void clear_ep0_setup_end(void);
/*set ep0 status*/
void set_ep0_in_pkt_rdy(void);
void set_ep0_inpkt_dataend(void);
void set_ep0_in_pkt_dataend(void);

void init_desc_ptr(void);
void WrPktEp0(uint8_t *buf,int num);
void InitDescriptorTable(void);
extern volatile int isUsbdSetConfiguration;
extern volatile int is_usb_set_interface;
void RdPktEp0(uint8_t *buf,int num);

#endif

