#include "head.h"
#include "s3c2440_usb_util.h"
uint8_t usb_trace_mask = USB_TRACE_ALWAYS;

void usb_mem_copy(const uint8_t *src, uint8_t *dest,uint32_t size)
{
	uint32_t cnt;
	for(cnt = 0;cnt < size;cnt++)
	{
		*dest++ = *src++;
	}
	//*dest = '\0';
}
