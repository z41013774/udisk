#ifndef _USB_UTIL_H__
#define _USB_UTIL_H__
#include "s3c2440_uart.h"
#define USB_TRACE_CMD				0x00000002
#define USB_TRACE_ALLOCATE			0x00000004
#define USB_TRACE_SCAN				0x00000008
#define USB_TRACE_BAD_BLOCKS		0x00000010
#define USB_TRACE_ERASE				0x00000020
#define USB_TRACE_GC				0x00000040
#define USB_TRACE_WRITE				0x00000080
#define USB_TRACE_TRACING			0x00000100
#define USB_TRACE_DELETION			0x00000200
#define USB_TRACE_BUFFERS			0x00000400
#define USB_TRACE_NANDACCESS		0x00000800
#define USB_TRACE_GC_DETAIL			0x00001000
#define USB_TRACE_SCAN_DEBUG		0x00002000


#define USB_TRACE_ERROR				0x40000000
#define USB_TRACE_BUG				0x80000000
#define USB_TRACE_ALWAYS			0xF0000000
extern uint8_t usb_trace_mask ;
#define TOUT(p) dbg_printf p
void usb_mem_copy(const uint8_t *src, uint8_t *dest,uint32_t size);
#define T(mask, p) do { if ((mask) & (usb_trace_mask | USB_TRACE_ALWAYS)) TOUT(p); } while (0)

#endif
