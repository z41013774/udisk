#include "s3c2440.h"
#define rFUNC_ADDR_REG     (*(volatile unsigned char *)0x52000140)	//Function address
#define rPWR_REG           (*(volatile unsigned char *)0x52000144)	//Power management
#define rEP_INT_REG        (*(volatile unsigned char *)0x52000148)	//EP Interrupt pending and clear
#define rUSB_INT_REG       (*(volatile unsigned char *)0x52000158)	//USB Interrupt pending and clear
#define rEP_INT_EN_REG     (*(volatile unsigned char *)0x5200015c)	//Interrupt enable
#define rUSB_INT_EN_REG    (*(volatile unsigned char *)0x5200016c)
#define rFRAME_NUM1_REG    (*(volatile unsigned char *)0x52000170)	//Frame number lower byte
#define rFRAME_NUM2_REG    (*(volatile unsigned char *)0x52000174)	//Frame number higher byte
#define rINDEX_REG         (*(volatile unsigned char *)0x52000178)	//Register index
#define rMAXP_REG          (*(volatile unsigned char *)0x52000180)	//Endpoint max packet
#define rEP0_CSR           (*(volatile unsigned char *)0x52000184)	//Endpoint 0 status
#define rIN_CSR1_REG       (*(volatile unsigned char *)0x52000184)	//In endpoint control status
#define rIN_CSR2_REG       (*(volatile unsigned char *)0x52000188)
#define rOUT_CSR1_REG      (*(volatile unsigned char *)0x52000190)	//Out endpoint control status
#define rOUT_CSR2_REG      (*(volatile unsigned char *)0x52000194)
#define rOUT_FIFO_CNT1_REG (*(volatile unsigned char *)0x52000198)	//Endpoint out write count
#define rOUT_FIFO_CNT2_REG (*(volatile unsigned char *)0x5200019c)
#define rEP0_FIFO          (*(volatile unsigned char *)0x520001c0)	//Endpoint 0 FIFO
#define rEP1_FIFO          (*(volatile unsigned char *)0x520001c4)	//Endpoint 1 FIFO
#define rEP2_FIFO          (*(volatile unsigned char *)0x520001c8)	//Endpoint 2 FIFO
#define rEP3_FIFO          (*(volatile unsigned char *)0x520001cc)	//Endpoint 3 FIFO
#define rEP4_FIFO          (*(volatile unsigned char *)0x520001d0)	//Endpoint 4 FIFO
#define rEP1_DMA_CON       (*(volatile unsigned char *)0x52000200)	//EP1 DMA interface control
#define rEP1_DMA_UNIT      (*(volatile unsigned char *)0x52000204)	//EP1 DMA Tx unit counter
#define rEP1_DMA_FIFO      (*(volatile unsigned char *)0x52000208)	//EP1 DMA Tx FIFO counter
#define rEP1_DMA_TTC_L     (*(volatile unsigned char *)0x5200020c)	//EP1 DMA total Tx counter
#define rEP1_DMA_TTC_M     (*(volatile unsigned char *)0x52000210)
#define rEP1_DMA_TTC_H     (*(volatile unsigned char *)0x52000214)
#define rEP2_DMA_CON       (*(volatile unsigned char *)0x52000218)	//EP2 DMA interface control
#define rEP2_DMA_UNIT      (*(volatile unsigned char *)0x5200021c)	//EP2 DMA Tx unit counter
#define rEP2_DMA_FIFO      (*(volatile unsigned char *)0x52000220)	//EP2 DMA Tx FIFO counter
#define rEP2_DMA_TTC_L     (*(volatile unsigned char *)0x52000224)	//EP2 DMA total Tx counter
#define rEP2_DMA_TTC_M     (*(volatile unsigned char *)0x52000228)
#define rEP2_DMA_TTC_H     (*(volatile unsigned char *)0x5200022c)
#define rEP3_DMA_CON       (*(volatile unsigned char *)0x52000240)	//EP3 DMA interface control
#define rEP3_DMA_UNIT      (*(volatile unsigned char *)0x52000244)	//EP3 DMA Tx unit counter
#define rEP3_DMA_FIFO      (*(volatile unsigned char *)0x52000248)	//EP3 DMA Tx FIFO counter
#define rEP3_DMA_TTC_L     (*(volatile unsigned char *)0x5200024c)	//EP3 DMA total Tx counter
#define rEP3_DMA_TTC_M     (*(volatile unsigned char *)0x52000250)
#define rEP3_DMA_TTC_H     (*(volatile unsigned char *)0x52000254)

/* MAXP Register */
#define FIFO_SIZE_0              0x00  /* 0x00 * 8 = 0  */
#define FIFO_SIZE_8              0x01  /* 0x01 * 8 = 8  */
#define FIFO_SIZE_16             0x02  /* 0x02 * 8 = 16 */
#define FIFO_SIZE_32             0x04  /* 0x04 * 8 = 32 */
#define FIFO_SIZE_64             0x08  /* 0x08 * 8 = 64 */

/* ENDPOINT0 CSR (Control Status Register) : Mapped to IN CSR1 */
#define EP0_OUT_PKT_READY        0x01  /* USB sets, MCU clears by setting SERVICED_OUT_PKT_RDY */
#define EP0_IN_PKT_READY         0x02  /* MCU sets, USB clears after sending FIFO */
//#define EP0_SENT_STALL           0x04  /* USB sets */       
//#define EP0_DATA_END             0x08  /* MCU sets */
//#define EP0_SETUP_END            0x10  /* USB sets, MCU clears by setting SERVICED_SETUP_END */
//#define EP0_SEND_STALL           0x20  /* MCU sets */
#define EP0_SERVICED_OUT_PKT_RDY 0x40  /* MCU writes 1 to clear OUT_PKT_READY */
//#define EP0_SERVICED_SETUP_END   0x80  /* MCU writes 1 to clear SETUP_END        */

#define EP0_WR_BITS              0xc0  

//EP_INT_REG / EP_INT_EN_REG
#define EP0_INT                	 0x01  // Endpoint 0, Control   
#define EP1_INT                  0x02  // Endpoint 1, (Bulk-In) 
#define EP2_INT                  0x04  // Endpoint 2 
#define EP3_INT			 0x08  // Endpoint 3, (Bulk-Out)   
#define EP4_INT			 0x10  // Endpoint 4

//USB_INT_REG / USB_INT_EN_REG
#define SUSPEND_INT            	 0x01  
#define RESUME_INT               0x02  
#define RESET_INT                0x04  

//IN_CSR1
#define EPI_IN_PKT_READY         0x01  
#define EPI_UNDER_RUN		 0x04
#define EPI_FIFO_FLUSH		 0x08
#define EPI_SEND_STALL           0x10  
#define EPI_SENT_STALL           0x20  
#define EPI_CDT			 0x40	
#define EPI_WR_BITS              (EPI_FIFO_FLUSH|EPI_IN_PKT_READY|EPI_CDT) 
					//(EPI_FIFO_FLUSH) is preferred  (???)
//IN_CSR2
#define EPI_IN_DMA_INT_MASK	(1<<4)
#define EPI_MODE_IN		(1<<5)
#define EPI_MODE_OUT		(0<<5)
#define EPI_ISO			(1<<6)
#define EPI_BULK		(0<<6)
#define EPI_AUTO_SET		(1<<7)

//OUT_CSR1
#define EPO_OUT_PKT_READY        0x01  
#define EPO_OVER_RUN		 0x04  
#define EPO_DATA_ERROR		 0x08  
#define EPO_FIFO_FLUSH		 0x10
#define EPO_SEND_STALL           0x20  
#define EPO_SENT_STALL           0x40
#define EPO_CDT			 0x80	
#define EPO_WR_BITS              (EPO_FIFO_FLUSH|EPO_SEND_STALL|EPO_CDT)
					//(EPO_FIFO_FLUSH) is preferred (???)
					 
					//OUT_CSR2
#define EPO_OUT_DMA_INT_MASK	(1<<5)
#define EPO_ISO		 	(1<<6)
#define EPO_BULK	 	(0<<6)
#define EPO_AUTO_CLR		(1<<7)
//************************
//       Endpoint 0      
//************************
#if 1
// Standard bmRequestTyje (Direction) 
#define HOST_TO_DEVICE              (0x00)
#define DEVICE_TO_HOST              (0x80)    

// Standard bmRequestType (Type) 
#define STANDARD_TYPE               (0x00)
#define CLASS_TYPE                  (0x20)
#define VENDOR_TYPE                 (0x40)
#define RESERVED_TYPE               (0x60)

// Standard bmRequestType (Recipient) 
#define DEVICE_RECIPIENT            (0)
#define INTERFACE_RECIPIENT         (1)
#define ENDPOINT_RECIPIENT          (2)
#define OTHER_RECIPIENT             (3)

// Feature Selectors 
#define DEVICE_REMOTE_WAKEUP        (1)
#define EP_STALL                    (0)

// Standard Request Codes 
#define GET_STATUS                  (0)
#define CLEAR_FEATURE               (1)
#define SET_FEATURE                 (3)
#define SET_ADDRESS                 (5)
#define GET_DESCRIPTOR              (6)
#define SET_DESCRIPTOR              (7)
#define GET_CONFIGURATION           (8)
#define SET_CONFIGURATION           (9)
#define GET_INTERFACE               (10)
#define SET_INTERFACE               (11)
#define SYNCH_FRAME                 (12)

// Class-specific Request Codes 
#define GET_DEVICE_ID               (0)
#define GET_PORT_STATUS             (1)
#define SOFT_RESET                  (2)

// Descriptor Types
#define DEVICE_TYPE                 (1)
#define CONFIGURATION_TYPE          (2)
#define STRING_TYPE                 (3)
#define INTERFACE_TYPE              (4)
#define ENDPOINT_TYPE               (5)

//configuration descriptor: bmAttributes 
#define CONF_ATTR_DEFAULT	    (0x80) //Spec 1.0 it was BUSPOWERED bit.
#define CONF_ATTR_REMOTE_WAKEUP     (0x20)
#define CONF_ATTR_SELFPOWERED       (0x40)

//endpoint descriptor
#define EP_ADDR_IN		    (0x80)	
#define EP_ADDR_OUT		    (0x00)

#define EP_ATTR_CONTROL		    (0x0)	
#define EP_ATTR_ISOCHRONOUS	    (0x1)
#define EP_ATTR_BULK		    (0x2)
#define EP_ATTR_INTERRUPT	    (0x3)	


//string descriptor
#define LANGID_US_L 		    (0x09)  
#define LANGID_US_H 		    (0x04)


#define EP0_STATE_INIT 			(0)  

//NOTE: The ep0State value in a same group should be added by 1.
#define EP0_STATE_GD_DEV_0	 	(10)  //10-10=0 
#define EP0_STATE_GD_DEV_1 		(11)  //11-10=1
#define EP0_STATE_GD_DEV_2 		(12)  //12-10=2

#define EP0_STATE_GD_CFG_0	 	(20)
#define EP0_STATE_GD_CFG_1 		(21)
#define EP0_STATE_GD_CFG_2 		(22)
#define EP0_STATE_GD_CFG_3 		(23)
#define EP0_STATE_GD_CFG_4 		(24)

#define EP0_STATE_GD_CFG_ONLY_0		(40)
#define EP0_STATE_GD_CFG_ONLY_1		(41)
#define EP0_STATE_GD_IF_ONLY_0 		(42)
#define EP0_STATE_GD_IF_ONLY_1 		(43)
#define EP0_STATE_GD_EP0_ONLY_0		(44)
#define EP0_STATE_GD_EP1_ONLY_0		(45)
#define EP0_INTERFACE_GET                        (46)


#define EP0_STATE_GD_STR_I0	 	(30)  
#define EP0_STATE_GD_STR_I1	 	(31)  
#define EP0_STATE_GD_STR_I2	 	(32)  

#define EP0_CONFIG_SET    (33)
#define EP0_GET_STATUS0  (35)
#define EP0_GET_STATUS1  (36)
#define EP0_GET_STATUS2  (37)
#define EP0_GET_STATUS3  (38)
#define EP0_GET_STATUS4  (39)




extern uint32_t ep0State;

struct USB_SETUP_DATA{
    uint8_t bmRequestType;    
    uint8_t bRequest;         
    uint8_t bValueL;          
    uint8_t bValueH;          
    uint8_t bIndexL;          
    uint8_t bIndexH;          
    uint8_t bLengthL;         
    uint8_t bLengthH;         
};


struct USB_DEVICE_DESCRIPTOR{
    uint8_t bLength;    
    uint8_t bDescriptorType;         
    uint8_t bcdUSBL;
    uint8_t bcdUSBH;
    uint8_t bDeviceClass;          
    uint8_t bDeviceSubClass;          
    uint8_t bDeviceProtocol;          
    uint8_t bMaxPacketSize0;         
    uint8_t idVendorL;
    uint8_t idVendorH;
    uint8_t idProductL;
    uint8_t idProductH;
    uint8_t bcdDeviceL;
    uint8_t bcdDeviceH;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
};


struct USB_CONFIGURATION_DESCRIPTOR{
    uint8_t bLength;    
    uint8_t bDescriptorType;         
    uint8_t wTotalLengthL;
    uint8_t wTotalLengthH;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t maxPower;          
};
    

struct USB_INTERFACE_DESCRIPTOR{
    uint8_t bLength;    
    uint8_t bDescriptorType;         
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
};


struct USB_ENDPOINT_DESCRIPTOR{
    uint8_t bLength;    
    uint8_t bDescriptorType;         
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSizeL;
    uint8_t wMaxPacketSizeH;
    uint8_t bInterval;
};

 struct USB_CONFIGURATION_SET{
     uint8_t ConfigurationValue;
 };

 struct USB_GET_STATUS{
     uint8_t Device;
     uint8_t Interface;
     uint8_t Endpoint0;
     uint8_t Endpoint1;
     uint8_t Endpoint3;
 };

 struct USB_INTERFACE_GET{
     uint8_t AlternateSetting;
 };
#endif
