/**************************************************
****
****author : zhaohuawe       
****
****file   : s3c2440_usb_desc.c
****
****date   : 2015-09-09        
***************************************************/
#include "head.h"
#include "s3c2440_usb.h"
#include "s3c2440_usb_desc.h"
#include "testusb.h"
#include "s3c2440_error.h"

#if 1
#define CLR_EP0_OUT_PKT_RDY() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						EP0_SERVICED_OUT_PKT_RDY )	 
#define CLR_EP0_OUTPKTRDY_DATAEND() 	rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_SERVICED_OUT_PKT_RDY|EP0_DATA_END) )	 
					
#define SET_EP0_IN_PKT_RDY() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_IN_PKT_READY) )	 
#define SET_EP0_INPKTRDY_DATAEND() 	rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_IN_PKT_READY|EP0_DATA_END) )	 
					
#define CLR_EP0_SETUP_END() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)| \
						(EP0_SERVICED_SETUP_END) )

#define CLR_EP0_SENT_STALL() 		rEP0_CSR=( ep0_csr & (~EP0_WR_BITS)& \
						(~EP0_SENT_STALL) )

#define FLUSH_EP0_FIFO() 		{while(rOUT_FIFO_CNT1_REG)rEP0_FIFO;}

uint32_t ep0State;
uint32_t ep0SubState;

extern volatile int isUsbdSetConfiguration;
extern volatile int is_usb_set_interface;
volatile uint8_t Rwuen;
volatile uint8_t Configuration=1;
volatile uint8_t AlterSetting;
volatile uint8_t Selfpwr=1;
volatile uint8_t device_status;
volatile uint8_t interface_status;
volatile uint8_t endpoint0_status;
volatile uint8_t endpoint1_status;
volatile uint8_t endpoint3_status;
volatile int isUsbdSetConfiguration;
volatile int is_usb_set_interface;

struct USB_SETUP_DATA descSetup;
struct USB_DEVICE_DESCRIPTOR descDev;
struct USB_CONFIGURATION_DESCRIPTOR descConf;
struct USB_INTERFACE_DESCRIPTOR descIf;
struct USB_ENDPOINT_DESCRIPTOR descEndpt0;
struct USB_ENDPOINT_DESCRIPTOR descEndpt1;
struct USB_CONFIGURATION_SET ConfigSet;
struct USB_INTERFACE_GET InterfaceGet;
struct USB_GET_STATUS StatusGet;   //={0,0,0,0,0};


static const uint8_t descStr0[]={
	4,STRING_TYPE,LANGID_US_L,LANGID_US_H,  //codes representing languages
    };

static const uint8_t descStr1[]={  //Manufacturer  
        (0x14+2),STRING_TYPE, 
        'S',0x0,'y',0x0,'s',0x0,'t',0x0,'e',0x0,'m',0x0,' ',0x0,'M',0x0,
        'C',0x0,'U',0x0,
    };
    
static const uint8_t descStr2[]={  //Product  
        (0x2a+2),STRING_TYPE, 
        'S',0x0,'E',0x0,'C',0x0,' ',0x0,'S',0x0,'3',0x0,'C',0x0,'2',0x0,
        '4',0x0,'1',0x0,'0',0x0,'X',0x0,' ',0x0,'T',0x0,'e',0x0,'s',0x0,
        't',0x0,' ',0x0,'B',0x0,'/',0x0,'D',0x0
    };


void Ep0Handler(void)
{
    static int ep0SubState;
    int i;
    uint8_t ep0_csr;

    rINDEX_REG=0;
    ep0_csr=rEP0_CSR;
    
   	
    //DATAEND interrupt(ep0_csr==0x0) will be ignored 
    //because ep0State==EP0_STATE_INIT when the DATAEND interrupt is issued.

    
    if(ep0_csr & EP0_SETUP_END)
    {   
    	 // Host may end GET_DESCRIPTOR operation without completing the IN data stage.
    	 // If host does that, SETUP_END bit will be set.
    	 // OUT_PKT_RDY has to be also cleared because status stage sets OUT_PKT_RDY to 1.
   		
		//Dbgfunc('s');
		//Dbgfunc('e');
		//Dbgfunc('t');
		//USB_TRACE("setup end \r\n");
		CLR_EP0_SETUP_END();
		if(ep0_csr & EP0_OUT_PKT_READY) 
		{
			//USB_TRACE("flush\r\n");
		    FLUSH_EP0_FIFO(); //(???)
		    	//I think this isn't needed because EP0 flush is done automatically.   
		    CLR_EP0_OUT_PKT_RDY();
		}
	
		ep0State=EP0_STATE_INIT;
		return;
    }	

    //I think that EP0_SENT_STALL will not be set to 1.
    if(ep0_csr & EP0_SENT_STALL)
    {   
    	USB_TRACE("STALL \r\n");
     	CLR_EP0_SENT_STALL();
	if(ep0_csr & EP0_OUT_PKT_READY) 
	{
	    CLR_EP0_OUT_PKT_RDY();
	}
	
	ep0State=EP0_STATE_INIT;
	return;
    }	



    if((ep0_csr & EP0_OUT_PKT_READY) && (ep0State==EP0_STATE_INIT))
    {	
		
		RdPktEp0((uint8_t *)&descSetup,EP0_PKT_SIZE);
		//PrintEp0Pkt((uint8_t *)(&descSetup)); //DEBUG
    	//USB_TRACE("req 0x%x\r\n",descSetup.bRequest);
		switch(descSetup.bRequest)
    	{
    		case GET_DESCRIPTOR:
            switch(descSetup.bValueH)        
            {
	            case DEVICE_TYPE:
		 	    	CLR_EP0_OUT_PKT_RDY();
				
			    	ep0State=EP0_STATE_GD_DEV_0;	        
			    	break;	
		   		case CONFIGURATION_TYPE:
		 	    	
		 	    	CLR_EP0_OUT_PKT_RDY();

		 	    	if((descSetup.bLengthL+(descSetup.bLengthH<<8))>0x9)
		 	    	  //bLengthH should be used for bLength=0x209 at WIN2K.    	
			    	   ep0State=EP0_STATE_GD_CFG_0; //for WIN98,WIN2K
		              else	    	    
		  		    	ep0State=EP0_STATE_GD_CFG_ONLY_0; //for WIN2K
			    	break;
	   	    	case STRING_TYPE:
		 	    	
		 	    	CLR_EP0_OUT_PKT_RDY();
			    	switch(descSetup.bValueL)
			    	{
			    	    case 0:
			    	    	ep0State=EP0_STATE_GD_STR_I0;
	
			    	    	break;
			    	    case 1:
		       	    	    ep0State=EP0_STATE_GD_STR_I1;

			    	    	break;
			    	    case 2:	
			    	    	ep0State=EP0_STATE_GD_STR_I2;
			    	    	break;
			    	    default:
			    		
			    		break;
			    	}
		    		ep0SubState=0;
		    		break;
			    case INTERFACE_TYPE:
		 	    	

		 	    	CLR_EP0_OUT_PKT_RDY();
			    	ep0State=EP0_STATE_GD_IF_ONLY_0; //for WIN98
			    	break;
			    case ENDPOINT_TYPE:	    	
		 	    	
		 	    	CLR_EP0_OUT_PKT_RDY();
					
		 	    	switch(descSetup.bValueL&0xf)
			    	{
			    	case 0:

			    	    ep0State=EP0_STATE_GD_EP0_ONLY_0;
			    	    break;
			    	case 1:

		       	    	ep0State=EP0_STATE_GD_EP1_ONLY_0;
			    	    break;
			    	default:
			    	    
			    	    break;
			    	}
			    	break;
			    default:
			    	
			    	break;
			    }	
		    	    break;

    	case SET_ADDRESS:
            
            rFUNC_ADDR_REG=descSetup.bValueL | 0x80;
		//	USB_TRACE("AD 0x%x\r\n",rFUNC_ADDR_REG);
	    	CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
			//USB_TRACE("AD 0x%x\r\n",rEP0_CSR);
			ep0State=EP0_STATE_INIT;
            break;
    	
		case SET_CONFIGURATION:
     
            ConfigSet.ConfigurationValue=descSetup.bValueL;
            CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.

			ep0State=EP0_STATE_INIT;

            isUsbdSetConfiguration=1; 
            break;

    	    //////////////////////// For chapter 9 test ////////////////////

    	  case CLEAR_FEATURE:
    	  	
    	  	switch (descSetup.bmRequestType)
    	  	{
    	  	  case DEVICE_RECIPIENT:
    	  	  	if (descSetup.bValueL == 1)
    	  	  		Rwuen = 0;
    	  	  	
    	  	  	break;

    	  	  case ENDPOINT_RECIPIENT:
    	  	  	 if (descSetup.bValueL == 0)
    	  	  	 {
                   if((descSetup.bIndexL & 0x7f) == 0x00)
				   {
                     StatusGet.Endpoint0= 0;    
                   }
                   if((descSetup.bIndexL & 0x8f) == 0x81)
				   {           // IN  Endpoint 1
                     StatusGet.Endpoint1= 0;           
                   }
                   if((descSetup.bIndexL & 0x8f) == 0x03)
				   {          // OUT Endpoint 3
                     StatusGet.Endpoint3= 0;      
                   }
                 }
    	  	  	 
    	  	  	 break;

    	  	  default:
    	  	  	break;
    	  	}
    	  	CLR_EP0_OUTPKTRDY_DATAEND();

    	  	ep0State=EP0_STATE_INIT;
    	  	break;

    	  case GET_CONFIGURATION:

                CLR_EP0_OUT_PKT_RDY();

	    	  ep0State=EP0_CONFIG_SET;
    	  	   
    	         break;


    	  case GET_INTERFACE:
    	  	
    	  	  CLR_EP0_OUT_PKT_RDY();
	    	  ep0State=EP0_INTERFACE_GET;
    	  	  
    	  	  break;

    	  case GET_STATUS:
    	  	switch(descSetup.bmRequestType)
    	  	{
    	  	      case  (0x80):

    	  	 		CLR_EP0_OUT_PKT_RDY();
    	  	 		StatusGet.Device=((uint8_t)Rwuen<<1)|(uint8_t)Selfpwr;
    	  	 		ep0State=EP0_GET_STATUS0;
    	  	 		    	  	 		
                     break;

                    case  (0x81):
                     CLR_EP0_OUT_PKT_RDY();
    	  	 		StatusGet.Interface=0;
    	  	 		ep0State=EP0_GET_STATUS1;
                          break;

                     case  (0x82):

                     	CLR_EP0_OUT_PKT_RDY();
    	  	 		if((descSetup.bIndexL & 0x7f) == 0x00){
	                          ep0State=EP0_GET_STATUS2;
    	  	 		  }
    	  	 		
	                       if((descSetup.bIndexL & 0x8f) == 0x81){
	                          ep0State=EP0_GET_STATUS3;
	                       }
	                       
	                       if((descSetup.bIndexL & 0x8f) == 0x03){
                                ep0State=EP0_GET_STATUS4;
	                       }
                          break;

                      default:
                          	break;
    	  	}
    	  	      
    	  	break;


    	  case SET_DESCRIPTOR:		  	
    	  	       CLR_EP0_OUTPKTRDY_DATAEND();
    	  	       ep0State=EP0_STATE_INIT;
    	  	break;


    	  case SET_FEATURE:
    	  
    	  	switch (descSetup.bmRequestType)
    	  	{
    	  	  case DEVICE_RECIPIENT:
    	  	  	if (descSetup.bValueL == 1)
    	  	  		Rwuen = 1;
    	  	  	
    	  	  	break;

    	  	  case ENDPOINT_RECIPIENT:
    	  	  	 if (descSetup.bValueL == 0)
    	  	  	 {
	                       if((descSetup.bIndexL & 0x7f) == 0x00){
	                        StatusGet.Endpoint0= 1;
	                       }
	                       if((descSetup.bIndexL & 0x8f) == 0x81){
	                         StatusGet.Endpoint1= 1;
	                       }
	                       if((descSetup.bIndexL & 0x8f) == 0x03){
	                         StatusGet.Endpoint3= 1;
	                       }
                       }
    	  	  	break;

    	  	  default:
    	  	  	break;
    	  	}
    	  	CLR_EP0_OUTPKTRDY_DATAEND();
    	  	ep0State=EP0_STATE_INIT;
    	  
    	  	break;


    	  case SET_INTERFACE:
		  	//	dbgStrFifo[dbgcnt]='D';
			//	dbgcnt++;
    	  	   InterfaceGet.AlternateSetting= descSetup.bValueL;
    	  	   CLR_EP0_OUTPKTRDY_DATAEND(); 
			   is_usb_set_interface = 1;
               ep0State=EP0_STATE_INIT;
    	  	break;

    	  case SYNCH_FRAME:
		  		//dbgStrFifo[dbgcnt]='E';
				//dbgcnt++;
    	  	ep0State=EP0_STATE_INIT;
    	  	break;

    	  //////////////////////////////////////////////////////////////

  	default:
	    
    	    CLR_EP0_OUTPKTRDY_DATAEND(); //Because of no data control transfers.
	    ep0State=EP0_STATE_INIT;
	    break;
    
        }
    }
    
    switch(ep0State)
    {	
	case EP0_STATE_INIT:
	    break; 

	//=== GET_DESCRIPTOR:DEVICE ===
    	case EP0_STATE_GD_DEV_0:
       
            WrPktEp0((uint8_t *)&descDev+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
			//USB_TRACE("S 0x%x\r\n",rEP0_FIFO);
            ep0State=EP0_STATE_GD_DEV_1;
            break;
            
    	case EP0_STATE_GD_DEV_1:
           
            WrPktEp0((uint8_t *)&descDev+0x8,8); 
            SET_EP0_IN_PKT_RDY();
           // USB_TRACE("S 0x%x\r\n",rEP0_FIFO);
            ep0State=EP0_STATE_GD_DEV_2;
            break;

    	case EP0_STATE_GD_DEV_2:
           
            WrPktEp0((uint8_t *)&descDev+0x10,2);   //8+8+2=0x12
            SET_EP0_INPKTRDY_DATAEND();
			//USB_TRACE("S 0x%x\r\n",rEP0_FIFO);
            ep0State=EP0_STATE_INIT;
            break;
    
#if 1
        //=== GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
        //Windows98 gets these 4 descriptors all together by issuing only a request.
        //Windows2000 gets each descriptor seperately.
    	case EP0_STATE_GD_CFG_0:
          
            WrPktEp0((uint8_t *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_1;
            break;
    
    	case EP0_STATE_GD_CFG_1:
          
            WrPktEp0((uint8_t *)&descConf+8,1); 
            WrPktEp0((uint8_t *)&descIf+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_2;
            break;

    	case EP0_STATE_GD_CFG_2:
           
            WrPktEp0((uint8_t *)&descIf+7,2); 
            WrPktEp0((uint8_t *)&descEndpt0+0,6); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_3;
            break;

    	case EP0_STATE_GD_CFG_3:
         
            WrPktEp0((uint8_t *)&descEndpt0+6,1); 
            WrPktEp0((uint8_t *)&descEndpt1+0,7); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_4;            
            break;

    	case EP0_STATE_GD_CFG_4:
       
             //zero length data packit 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
#endif
        //=== GET_DESCRIPTOR:CONFIGURATION ONLY===
    	case EP0_STATE_GD_CFG_ONLY_0:
           
            WrPktEp0((uint8_t *)&descConf+0,8); //EP0_PKT_SIZE
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_CFG_ONLY_1;
            break;
    
    	case EP0_STATE_GD_CFG_ONLY_1:
          
            WrPktEp0((uint8_t *)&descConf+8,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;

        //=== GET_DESCRIPTOR:INTERFACE ONLY===
    	case EP0_STATE_GD_IF_ONLY_0:
          
            WrPktEp0((uint8_t *)&descIf+0,8); 
            SET_EP0_IN_PKT_RDY();
            ep0State=EP0_STATE_GD_IF_ONLY_1;
            break;
    	case EP0_STATE_GD_IF_ONLY_1:
         
            WrPktEp0((uint8_t *)&descIf+8,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;

        //=== GET_DESCRIPTOR:ENDPOINT 0 ONLY===
    	case EP0_STATE_GD_EP0_ONLY_0:
          
            WrPktEp0((uint8_t *)&descEndpt0+0,7); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
            
        //=== GET_DESCRIPTOR:ENDPOINT 1 ONLY===
    	case EP0_STATE_GD_EP1_ONLY_0:
          
            WrPktEp0((uint8_t *)&descEndpt1+0,7); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;            
            break;
            
////////////////////////////////////////////

         case EP0_INTERFACE_GET:
            WrPktEp0((uint8_t *)&InterfaceGet+0,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

 
        //=== GET_DESCRIPTOR:STRING ===

    	case EP0_STATE_GD_STR_I0:
            
           WrPktEp0((uint8_t *)descStr0, 4 );  
	    SET_EP0_INPKTRDY_DATAEND();
	    ep0State=EP0_STATE_INIT;     
	    ep0SubState=0;
	    break;

	case EP0_STATE_GD_STR_I1:
           
             if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr1) )
            {
            	WrPktEp0((uint8_t *)descStr1+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
            	SET_EP0_IN_PKT_RDY();
            	ep0State=EP0_STATE_GD_STR_I1;
            	ep0SubState++;
            }
	    else
	    {
	    	WrPktEp0((uint8_t *)descStr1+(ep0SubState*EP0_PKT_SIZE),
	    		 sizeof(descStr1)-(ep0SubState*EP0_PKT_SIZE)); 
		SET_EP0_INPKTRDY_DATAEND();
		ep0State=EP0_STATE_INIT;     
		ep0SubState=0;
	    }
	    break;

	case EP0_STATE_GD_STR_I2:
          
             if( (ep0SubState*EP0_PKT_SIZE+EP0_PKT_SIZE)<sizeof(descStr2) )
             {
            	WrPktEp0((uint8_t *)descStr2+(ep0SubState*EP0_PKT_SIZE),EP0_PKT_SIZE); 
            	SET_EP0_IN_PKT_RDY();
            	ep0State=EP0_STATE_GD_STR_I2;
            	ep0SubState++;
             }
	    else
	    {
               
	    	WrPktEp0((uint8_t *)descStr2+(ep0SubState*EP0_PKT_SIZE),
	    		 sizeof(descStr2)-(ep0SubState*EP0_PKT_SIZE)); 
		SET_EP0_INPKTRDY_DATAEND();
		ep0State=EP0_STATE_INIT;     
		ep0SubState=0;
	    }
	    break;

	 case EP0_CONFIG_SET:
	 	WrPktEp0((uint8_t *)&ConfigSet+0,1); 
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
            break;

        case EP0_GET_STATUS0:
	     WrPktEp0((uint8_t *)&StatusGet+0,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS1:
	     WrPktEp0((uint8_t *)&StatusGet+1,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS2:
	     WrPktEp0((uint8_t *)&StatusGet+2,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS3:
	     WrPktEp0((uint8_t *)&StatusGet+3,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         case EP0_GET_STATUS4:
	     WrPktEp0((uint8_t *)&StatusGet+4,1);
            SET_EP0_INPKTRDY_DATAEND();
            ep0State=EP0_STATE_INIT;      
         break;

         
	 	
     	default:
	  

     	    break;
    }
}


   
    
void PrintEp0Pkt(uint8_t *pt)
{
    
}

void RdPktEp0(uint8_t *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(uint8_t)rEP0_FIFO;	
    }
}

void WrPktEp0(uint8_t *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        rEP0_FIFO=buf[i];	
    }
}


void InitDescriptorTable(void)
{	
    //Standard device descriptor
    descDev.bLength=0x12;	//EP0_DEV_DESC_SIZE=0x12 bytes    
    descDev.bDescriptorType=DEVICE_TYPE;         
    descDev.bcdUSBL=0x10;
    descDev.bcdUSBH=0x01; 	//Ver 1.10
    descDev.bDeviceClass=0x00; //0x0          
    descDev.bDeviceSubClass=0x0;          
    descDev.bDeviceProtocol=0x0;          
    descDev.bMaxPacketSize0=0x8;         
    descDev.idVendorL=0x45;
    descDev.idVendorH=0x53;
    descDev.idProductL=0x34;
    descDev.idProductH=0x12;
    descDev.bcdDeviceL=0x00;
    descDev.bcdDeviceH=0x01;
    descDev.iManufacturer=0x1;  //index of string descriptor
    descDev.iProduct=0x2;	//index of string descriptor 
    descDev.iSerialNumber=0x0;
    descDev.bNumConfigurations=0x1;

    //Standard configuration descriptor
    descConf.bLength=0x9;    
    descConf.bDescriptorType=CONFIGURATION_TYPE;         
    descConf.wTotalLengthL=0x20; //<cfg desc>+<if desc>+<endp0 desc>+<endp1 desc>
    descConf.wTotalLengthH=0;
    descConf.bNumInterfaces=1;
//dbg    descConf.bConfigurationValue=2;  //why 2? There's no reason.
    descConf.bConfigurationValue=1;  
    descConf.iConfiguration=0;
    descConf.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED;  //bus powered only.
    descConf.maxPower=25; //draws 50mA current from the USB bus.          

    //Standard interface descriptor
    descIf.bLength=0x9;    
    descIf.bDescriptorType=INTERFACE_TYPE;         
    descIf.bInterfaceNumber=0x0;
    descIf.bAlternateSetting=0x0; //?
    descIf.bNumEndpoints=2;	//# of endpoints except EP0
    descIf.bInterfaceClass=0x08; //0x0 ?
    descIf.bInterfaceSubClass=0x06;  
    descIf.bInterfaceProtocol=0x50;
    descIf.iInterface=0x0;

    //Standard endpoint0 descriptor
    descEndpt0.bLength=0x7;    
    descEndpt0.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt0.bEndpointAddress=1|EP_ADDR_IN;   // 2400Xendpoint 1 is IN endpoint.
    descEndpt0.bmAttributes=EP_ATTR_BULK;
    descEndpt0.wMaxPacketSizeL=EP1_PKT_SIZE; //64
    descEndpt0.wMaxPacketSizeH=0x0;
    descEndpt0.bInterval=0x0; //not used

    //Standard endpoint1 descriptor
    descEndpt1.bLength=0x7;    
    descEndpt1.bDescriptorType=ENDPOINT_TYPE;         
    descEndpt1.bEndpointAddress=3|EP_ADDR_OUT;   // 2400X endpoint 3 is OUT endpoint.
    descEndpt1.bmAttributes=EP_ATTR_BULK;
    descEndpt1.wMaxPacketSizeL=EP3_PKT_SIZE; //64
    descEndpt1.wMaxPacketSizeH=0x0;
    descEndpt1.bInterval=0x0; //not used 
}
#endif
#if 0
struct usb_dev_desc	desc_dev;
struct usb_configuration_desc	desc_config;
struct usb_interface_desc	desc_interface;
struct usb_endpoint_desc	desc_ep1;
struct usb_endpoint_desc	desc_ep3;
struct usb_setup_data		desc_data;
usb_status_t usb_status;
static const uint8_t desc_str_code[]={
	4,STRING_TYPE,LANGID_US_L,LANGID_US_H,  //codes representing languages
    };

static const uint8_t desc_str_manufacturer[]={  //Manufacturer  
        (0x14+2),STRING_TYPE, 
        'S',0x0,'y',0x0,'s',0x0,'t',0x0,'e',0x0,'m',0x0,' ',0x0,'M',0x0,
        'C',0x0,'U',0x0,
    };
    
static const uint8_t desc_str_product[]={  //Product  
        (0x2a+2),STRING_TYPE, 
        'S',0x0,'E',0x0,'C',0x0,' ',0x0,'S',0x0,'3',0x0,'C',0x0,'2',0x0,
        '4',0x0,'1',0x0,'0',0x0,'X',0x0,' ',0x0,'T',0x0,'e',0x0,'s',0x0,
        't',0x0,' ',0x0,'B',0x0,'/',0x0,'D',0x0
    };
volatile int flag = 0;

/*func ptr*/
void (*std_dev_req[13])(void);
void (*desc_type[6])(void);
void init_desc_ptr(void)
{
	flag = 0;

	std_dev_req[0] = get_status;
	std_dev_req[1] = clear_feature;
	std_dev_req[2] = 0;
	std_dev_req[3] = set_feature;
	std_dev_req[4] = 0;
	std_dev_req[5] = set_address;
	std_dev_req[6] = get_descriptor;
	std_dev_req[7] = set_descriptor;
	std_dev_req[8] = get_configuration;
	std_dev_req[9] = set_configuration;
	std_dev_req[10] = get_interface;
	std_dev_req[11] = set_interface;
	std_dev_req[12] = synch_frame;

	desc_type[0] = 0;
	desc_type[1] = get_dev_type;
	desc_type[2] = get_configuration_type;
	desc_type[3] = get_str_type;
	desc_type[4] = get_interface_type;
	desc_type[5] = get_endpoint_type;
}
void usb_init_desc_table(void)
{
	/*device desc*/

	desc_dev.bLength=0x12;
	desc_dev.bDescriptorType = DEVICE_TYPE;
	desc_dev.bcdUSBL= 0x10;
	desc_dev.bcdUSBH= 0x01;
	desc_dev.bDeviceClass = 0xFF;
	desc_dev.bDeviceSubClass = 0x0;
	desc_dev.bDeviceProtocol = 0x0;
	desc_dev.bMaxPacketSize0 = 0x8;
	desc_dev.idVendorL= 0x45;
	desc_dev.idVendorH = 0x53;
	desc_dev.idProductL = 0x34;
	desc_dev.idProductH = 0x12;
	desc_dev.bcdDeviceL = 0x00;
	desc_dev.bcdDeviceH = 0x01;
	desc_dev.iManufacturer = 0x01;
	desc_dev.iProduct	= 0x02;
	desc_dev.iSerialNumber = 0x0;
	desc_dev.bNumConfigurations = 0x01;



	desc_config.bLength = 0x09;
	desc_config.bDescriptorType = CONFIGURATION_TYPE;
	desc_config.wTotalLengthL = 0x20;
	desc_config.wTotalLengthH = 0X0;
	desc_config.bNumInterfaces = 1;
	desc_config.bConfigurationValue = 0x01;
	desc_config.iConfiguration = 0;
	desc_config.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED;  //bus powered only.
	desc_config.bMaxPower = 25;

	desc_interface.bLength = 0x9;
	desc_interface.bDescriptorType = INTERFACE_TYPE;
	desc_interface.bInterfaceNumber = 0;
	desc_interface.bAlternateSetting = 0;
	desc_interface.bNumEndpoints = 0x2;
	desc_interface.bInterfaceClass=0x08; //0x0 ?
    desc_interface.bInterfaceSubClass=0x06;  
    desc_interface.bInterfaceProtocol=0x50;
    desc_interface.iInterface=0x0;
	/*endpoint1 in*/
	desc_ep1.bLength=0x7;    
    desc_ep1.bDescriptorType=ENDPOINT_TYPE;     
	desc_ep1.bEndPointAddress =1|IN;   // 2400Xendpoint 1 is IN endpoint.
    desc_ep1.bmAttributes=EP_ATTR_BULK;
    desc_ep1.wMaxPacketSizeL =EP1_PKT_SIZE; //64
    desc_ep1.wMaxPacketSizeH = 0;
    desc_ep1.bInterval=0x0; //

	/*endpoint3 out*/
	desc_ep3.bLength=0x7;    
    desc_ep3.bDescriptorType=ENDPOINT_TYPE;         
    desc_ep3.bEndPointAddress=3|OUT;   // 2400Xendpoint 1 is IN endpoint.
    desc_ep3.bmAttributes=EP_ATTR_BULK;
    desc_ep3.wMaxPacketSizeL =EP3_PKT_SIZE; //64
   	desc_ep3.wMaxPacketSizeH = 0;
    desc_ep3.bInterval=0x0; //
}
void U_LINE clear_ep0_outpkt_dataend(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG |=((EP0_SERVICED_OUTPUT_RDY)|(EP0_DATA_END));
	
}
void U_LINE clear_ep0_out_pkt_rdy(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();	
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG	|=	(EP0_SERVICED_OUTPUT_RDY);

}
void U_LINE clear_ep0_setup_end(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG |=	(EP0_SERVICED_SETUP_END);
}
void U_LINE clear_ep0_sent_stall(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	/*sent_stall ,write "0" to clear this bit */
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG|=~(EP0_SEND_STALL);

}
void U_LINE set_ep0_in_pkt_rdy(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG |=	(EP0_IN_PKT_RDY);
}
void U_LINE set_ep0_in_pkt_dataend(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->EP0_CSR_IN_CSR1_REG &= (~(EP0_SERVICED_SETUP_END|(EP0_SERVICED_OUTPUT_RDY)));
	usb_device->EP0_CSR_IN_CSR1_REG|=((EP0_IN_PKT_RDY)|(EP0_DATA_END));

}
void get_desc_pkt(uint8_t *desc_pkt)
{
	uint8_t cnt;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	for(cnt = 0;cnt < EP0_PKT_SIZE;cnt++)
	{
		desc_pkt[cnt] = usb_device->fifo[0].EP_FIFO_REG;
	}
}

void put_desc_pkt(uint8_t *desc_pkt,uint8_t size)
{
	uint8_t cnt;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	for(cnt = 0;cnt < size;cnt++)
	{
		
		usb_device->fifo[0].EP_FIFO_REG = desc_pkt[cnt];
		//USB_TRACE("0x%x\r\n",usb_device->fifo[0].EP_FIFO_REG) ;
	}

}
void usb_setup_desc(void)
{
	uint8_t ep0_arr[EP0_PKT_SIZE];
	uint8_t cnt;
	uint8_t ep0_csr;
	epnum_t epnum = EP0;
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	usb_device->INDEX_REG = epnum;
	ep0_csr = usb_device->EP0_CSR_IN_CSR1_REG;
	/*IS request finished*/
	if(ep0_csr & EP0_SETUP_END)
	{
		USB_TRACE("request setup end\r\n");
		
		clear_ep0_setup_end();
		
		return;
	}
	 if(ep0_csr & EP0_SENT_STALL)
    {   
    	USB_TRACE("STALL \r\n");
   		clear_ep0_sent_stall();
   	
		if(ep0_csr & EP0_OUT_PKT_RDY) 
		{
		    clear_ep0_out_pkt_rdy();
		}
		return;
    }	
	/*get fifo data*/
	if(ep0_csr& EP0_OUT_PKT_RDY)
	{
		get_desc_pkt((uint8_t *)&desc_data);
		USB_TRACE("strd request code %d\r\n",desc_data.bRequest);
		if(desc_data.bRequest <= 12)
		{
			std_dev_req[desc_data.bRequest]();
		}
		else
		{
			clear_ep0_setup_end();

		}
	}
	
}
void get_descriptor(void)
{
	clear_ep0_out_pkt_rdy();
	//USB_TRACE("desc type : %d\r\n",desc_data.bValueH);
	/*check */
	if((desc_data.bValueH > 0)&&(desc_data.bValueH < 6) )
	{
		/*fun ptr*/
		desc_type[desc_data.bValueH]();
	}
	else
	{
		USB_TRACE("descriptor type :%d\r\n",desc_data.bIndexH);
		
	}
}
void set_descriptor(void)
{
	clear_ep0_outpkt_dataend();
}
void set_address(void)
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();

	
	
	usb_device->FUNC_ADDR_REG = desc_data.bValueL | 0x80;
	//USB_TRACE("set addr 0x%x\r\n",usb_device->FUNC_ADDR_REG);
	clear_ep0_outpkt_dataend();
	
	//USB_TRACE("ep0 s 0x%x\r\n",usb_device->EP0_CSR_IN_CSR1_REG);
}
volatile uint8_t temp_configuration_value ;
void set_configuration()
{
	USB_TRACE("set config\r\n");
	clear_ep0_outpkt_dataend();
	temp_configuration_value = desc_data.bIndexL;
}
void get_configuration(void)
{
	USB_TRACE("get config\r\n");
	clear_ep0_outpkt_dataend();
	put_desc_pkt((uint8_t *) &temp_configuration_value,1);
	set_ep0_in_pkt_dataend();
}
volatile uint8_t temp_interface_value;
void set_interface(void)
{
	USB_TRACE("set interface");
	
	clear_ep0_outpkt_dataend();
	temp_interface_value = desc_data.bIndexL;
}
void get_interface(void)
{
	USB_TRACE("get interface");
	clear_ep0_out_pkt_rdy();
	put_desc_pkt((uint8_t *)&temp_interface_value,1);
	set_ep0_in_pkt_dataend();
}
void WrPktEp0(uint8_t *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        rEP0_FIFO=buf[i];	
    }
}

void get_dev_type(void)
{
			

#if 1

	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	uint8_t desc_dev_size = sizeof(desc_dev);
	uint8_t pkt_num =  (desc_dev_size / EP0_PKT_SIZE);
	uint8_t cnt =0;
	USB_TRACE("dev desc \r\n");
	if(pkt_num>=1)
	{
		for(cnt = 0;cnt < pkt_num; cnt++)
		{
			
			put_desc_pkt((uint8_t *)&desc_dev+cnt*EP0_PKT_SIZE,EP0_PKT_SIZE);
			set_ep0_in_pkt_rdy();
			//USB_TRACE("s 0x%x\r\n",usb_device->EP0_CSR_IN_CSR1_REG);
		}
	}
	put_desc_pkt((uint8_t *)&desc_dev+cnt*EP0_PKT_SIZE,desc_dev_size-cnt*EP0_PKT_SIZE);
	set_ep0_in_pkt_dataend();
	//USB_TRACE("s 0x%x\r\n",usb_device->EP0_CSR_IN_CSR1_REG);
	#endif
}
void get_configuration_type(void)
{
	uint8_t desc_dev_size = sizeof(desc_config);
	uint8_t pkt_num =  (desc_dev_size / EP0_PKT_SIZE);
	uint8_t cnt =0;
	USB_TRACE("configuration desc \r\n");
	if(pkt_num >= 1)
	{
		for(cnt = 0;cnt < pkt_num; cnt++)
		{
			
			put_desc_pkt((uint8_t *)&desc_config+cnt*EP0_PKT_SIZE,EP0_PKT_SIZE);
			set_ep0_in_pkt_rdy();
		}
	}
	put_desc_pkt((uint8_t *)&desc_config+cnt*EP0_PKT_SIZE,desc_dev_size-cnt*EP0_PKT_SIZE);
	set_ep0_in_pkt_dataend();
}
/**
**descriptor type
**0 : device
**1 : configuration
**2 : string
**3 : interface
**4 : endpoint
**/
void get_str_type(void)
{
	uint8_t desc_str_manf_size = sizeof(desc_str_manufacturer);
	uint8_t pkt_num =  (desc_str_manf_size/ EP0_PKT_SIZE);
	uint8_t cnt =0;
	/*str product*/
	uint8_t desc_str_procduct_size = sizeof(desc_str_product);
	uint8_t pkt_str_proc = (desc_str_manf_size / EP0_PKT_SIZE);
	switch(desc_data.bIndexL)
	{
		case 0:
			put_desc_pkt((uint8_t *)&desc_str_code,4);
			set_ep0_in_pkt_dataend();
			break;
		case 1:
			if(pkt_num >= 1)
			{
				for(cnt = 0; cnt < pkt_num ; cnt++)
				{
					put_desc_pkt((uint8_t *)&desc_str_manufacturer, EP0_PKT_SIZE);
					set_ep0_in_pkt_rdy();
				}
			}
			put_desc_pkt((uint8_t *)&desc_str_manufacturer+cnt*EP0_PKT_SIZE,desc_str_manf_size-cnt*EP0_PKT_SIZE);
			set_ep0_in_pkt_dataend();
			break;
		case 2:
			if(pkt_str_proc >= 1)
			{
				for(cnt = 0; cnt < pkt_num ; cnt++)
				{
					put_desc_pkt((uint8_t *)&desc_str_product, EP0_PKT_SIZE);
					set_ep0_in_pkt_rdy();
				}
			}
			put_desc_pkt((uint8_t *)&desc_str_product+cnt*EP0_PKT_SIZE,desc_str_procduct_size-cnt*EP0_PKT_SIZE);
			set_ep0_in_pkt_dataend();
		default :
			break;
		
	}
}
void get_interface_type(void)
{
	uint8_t desc_interface_size = sizeof(desc_interface);
	uint8_t pkt_num =  (desc_interface_size / EP0_PKT_SIZE);
	uint8_t cnt =0;
	if(pkt_num >= 0)
	{
		for(cnt = 0;cnt < pkt_num-1; cnt++)
		{
			
			put_desc_pkt((uint8_t *)&desc_interface+cnt*EP0_PKT_SIZE,EP0_PKT_SIZE);
			set_ep0_in_pkt_rdy();
		}
	}
	put_desc_pkt((uint8_t *)&desc_interface+cnt*EP0_PKT_SIZE,desc_interface_size-cnt*EP0_PKT_SIZE);
	set_ep0_in_pkt_dataend();
	
}
void get_endpoint_type(void)
{
	uint8_t desc_endpoint_size = sizeof(desc_ep1);
	uint8_t pkt_num =  (desc_endpoint_size / EP0_PKT_SIZE);
	uint8_t cnt =0;
	
	switch(desc_data.bIndexL & 0xf)
	{
		case 0:
			if(pkt_num >= 1)
			{
				for(cnt = 0;cnt < pkt_num; cnt++)
				{
					
					put_desc_pkt((uint8_t *)&desc_ep1+cnt*EP0_PKT_SIZE,EP0_PKT_SIZE);
					set_ep0_in_pkt_rdy();
				}
			}
			put_desc_pkt((uint8_t *)&desc_ep1+cnt*EP0_PKT_SIZE,desc_endpoint_size-cnt*EP0_PKT_SIZE);
			set_ep0_in_pkt_dataend();
			break;
		case 1:
			if(pkt_num >= 1)
			{
				for(cnt = 0;cnt < pkt_num; cnt++)
				{
					
					put_desc_pkt((uint8_t *)&desc_ep3+cnt*EP0_PKT_SIZE,EP0_PKT_SIZE);
					set_ep0_in_pkt_rdy();
				}
			}
			put_desc_pkt((uint8_t *)&desc_ep3+cnt*EP0_PKT_SIZE,desc_endpoint_size-cnt*EP0_PKT_SIZE);
			set_ep0_in_pkt_dataend();
			break;
		default :
			break;
	}
}
volatile uint8_t remote_wakeup;
volatile uint8_t sel_power;
void clear_feature(void)
{
	/*
	***	0 :device
	*** 1 :interface
	*** 2 :endpoint
	*/
	switch(desc_data.bmRequestType)
	{
		case 0:
			if(desc_data.bValueL == 1)
				remote_wakeup = 0;
			break;
		case 2:
			if(desc_data.bValueL == 0)
			{
				if((desc_data.bIndexL & 0x7f) == 0 )
					usb_status.endpoint0 = 0;
				if((desc_data.bIndexL & 0x8f) == 0x81)
					usb_status.endpoint1 = 0;
				if((desc_data.bIndexL & 0x8f) == 0x03)
					usb_status.endpoint3 = 0;
			}
			break;
		default :
			break;
	}
	clear_ep0_outpkt_dataend();
}

void set_feature(void)
{
	/*
	***	0 :device
	*** 1 :interface
	*** 2 :endpoint
	*/
	switch(desc_data.bmRequestType)
	{
		case 0:
			if(desc_data.bValueL == 1)
				remote_wakeup = 1;
			break;
		case 2:
			if(desc_data.bValueL == 0)
			{
				if((desc_data.bIndexL & 0x7f) == 0 )
					usb_status.endpoint0 = 1;
				if((desc_data.bIndexL & 0x8f) == 0x81)
					usb_status.endpoint1 = 1;
				if((desc_data.bIndexL & 0x8f) == 0x03)
					usb_status.endpoint3 = 1;
			}
			break;
		default :
			break;
	}
	clear_ep0_outpkt_dataend();
}
void get_status(void)
{
	/*
	***	0x80 : device
	*** 0x81 : interface
	*** 0x82 : endpoint
	*/
	clear_ep0_out_pkt_rdy();
	switch(desc_data.bmRequestType)
	{
		case 0x80:
			usb_status.device = (remote_wakeup<<1)|sel_power;
			put_desc_pkt((uint8_t *)&usb_status, 1);
			set_ep0_in_pkt_dataend();
			break;
		case 0x81:
			usb_status.interface =0;
			put_desc_pkt((uint8_t *)&usb_status+1, 1);
			set_ep0_in_pkt_dataend();
			break;
		case 0x82:
			if((desc_data.bIndexL &0x7f)== 0)
			{
				put_desc_pkt((uint8_t *)&usb_status+2, 1);
				set_ep0_in_pkt_dataend();
			}
			else if((desc_data.bIndexL &0x8f)== 0x81)
			{
				put_desc_pkt((uint8_t *)&usb_status+3, 1);
				set_ep0_in_pkt_dataend();
			}
			else if((desc_data.bIndexL &0x8f)== 0x03)
			{
				put_desc_pkt((uint8_t *)&usb_status+4, 1);
				set_ep0_in_pkt_dataend();
			}
			break;
		default :
			break;
			
			
	}
}
void synch_frame(void)
{
	clear_ep0_outpkt_dataend();
}
#endif
