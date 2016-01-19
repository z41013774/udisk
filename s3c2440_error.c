#include "s3c2440_error.h"
#include "s3c2440.h"
#include "s3c2440_usb.h"

void dump_usb_reg()
{
	S3C24X0_USB_DEVICE * const usb_device = S3C24X0_GetBase_USB_DEVICE();
	USB_TRACE("func_addr 0x%x\r\n",&usb_device->FUNC_ADDR_REG);
	USB_TRACE("pwr_addr 0x%x\r\n",&usb_device->PWR_REG);
	USB_TRACE("ep_int_addr 0x%x\r\n",&usb_device->EP_INT_REG);
	USB_TRACE("usb_int_addr 0x%x\r\n",&usb_device->USB_INT_REG);
	
	USB_TRACE("ep_int_en_addr 0x%x\r\n",&usb_device->EP_INT_EN_REG);
	USB_TRACE("frame_num1 0x%x\r\n",&usb_device->FRAME_NUM1_REG);
	USB_TRACE("frame_num1 0x%x\r\n",&usb_device->FRAME_NUM2_REG);
	USB_TRACE("index 0x%x\r\n",&usb_device->INDEX_REG);
	USB_TRACE("ep0_fifo 0x%x\r\n",&usb_device->fifo[0].EP_FIFO_REG);
	USB_TRACE("ep1_fifo 0x%x\r\n",&usb_device->fifo[1].EP_FIFO_REG);
	USB_TRACE("ep2_fifo 0x%x\r\n",&usb_device->fifo[2].EP_FIFO_REG);
	USB_TRACE("ep3_fifo 0x%x\r\n",&usb_device->fifo[3].EP_FIFO_REG);
	USB_TRACE("ep4_fifo 0x%x\r\n",&usb_device->fifo[4].EP_FIFO_REG);
	USB_TRACE("maxp_addr 0x%x\r\n",&usb_device->MAXP_REG);
	USB_TRACE("in_csr1 0x%x\r\n",&usb_device->EP0_CSR_IN_CSR1_REG);
	USB_TRACE("in_csr2_addr 0x%x\r\n",&usb_device->IN_CSR2_REG);
	USB_TRACE("out_csr1_addr 0x%x\r\n",&usb_device->OUT_CSR1_REG);
	USB_TRACE("out_csr2_addr 0x%x\r\n",&usb_device->OUT_CSR2_REG);
	USB_TRACE("out_fifo_cnt1_addr 0x%x\r\n",&usb_device->OUT_FIFO_CNT1_REG);
	USB_TRACE("out_fifo_cnt2_addr 0x%x\r\n",&usb_device->OUT_FIFO_CNT2_REG);
	

}


