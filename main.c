#include "head.h"
#include "s3c2440_error.h"
#include "s3c2440_usb.h"
#include "s3c2440_ftl.h"


int main()
{ 
	 
	 
	//initsystemclk();
	
	MMU_Init();
	usb_clk_96M();
	usb_init();
	nand_init();
	ftl_init();
	//create_bbt();
	//s3c2440_init_vect();
	scan_storage();
	#if 0
	cfg_timer();
	init_timer2();
	timer2_start();
	
	while(1)
	{
		tem++;
		if(tem == 1300000)
			break;
		//delay_ms(1);
	}
	timer2_stop();
	//led_on_off(0,1);
	//iic_test();
	#endif
	#if 0
	init_baud(115200);
	put_onechar('a');
	put_onechar('b');
	put_onechar('c');
	tem = get_onechar();
	put_onechar(tem);
	
	dbg_printf("Entry main\r\n");
	dbg_printf("the tem value 0x%2x\r\n",tem);
	#endif
	//flash_test();
	
	//init_512byte_data((char *)&w_buf[0],1,2);
	//usb_write(0,(uint8_t *)data_cache[0]);	
	
	isUsbdSetConfiguration = 0;
	is_usb_set_interface = 0;

	while(!isUsbdSetConfiguration)
	;
	
	USB_TRACE("scan done\r\n");
	/*CMD DATA STATUS*/
	while(1)
	{
		usb_buckonly_wait_cmd();
		switch(buckonly)
		{
			case BUCK_ONLY_CMD_STAGE:
				usb_process_cmd();
				break;
			case BUCK_ONLY_OUT_STAGE:
				usb_buckonly_out();
				break;
			case BUCK_ONLY_IN_STAGE:
				usb_buckonly_in();
				break;
			case BUCK_ONLY_STS_STAGE:
				usb_buckonly_sts();
				break;
			default:
				break;
		}
	
	}
}
