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
void usb_fill_buf(uint8_t *buf,uint32_t val,uint32_t size)
{
	uint32_t cnt;
	for(cnt = 0;cnt < size;)
	{
		buf[cnt++] = (val & 0xff);
		buf[cnt++] = ((val >>8)&0xff);
		buf[cnt++] = ((val>>16) & 0xff);
		buf[cnt++] = ((val >>24)&0xff);
	}
}
uint32_t Shifts(uint32_t x)
{
	int nShifts;

	nShifts =  0;

	if (!x)
		return 0;

	while (!(x&1)) {
		x >>= 1;
		nShifts++;
	}

	return nShifts;
}

