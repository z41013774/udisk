#ifndef __S3C2440_ERROR_H__
#define __S3C2440_ERROR_H__
//#define USB_DEBUG		
//#ifdef USB_DEBUG
//#else
//#define USB_LOG
//#endif
#define USB_DEBUG
#ifdef USB_DEBUG
	#define USB_TRACE	dbg_printf
#else
	#define USB_TRACE
#endif

void error_handler();

#endif
