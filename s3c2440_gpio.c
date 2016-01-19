#include "head.h"
/***************
 |flag	|led		|
 |0    	| off	|
 |1    	| on  	|
 **************/
void led_on_off(int led, int flag)
{
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
	gpio->GPBCON &=~(0xff<<10);
	gpio->GPBCON |=(0x55<<10);
	gpio->GPBDAT |=(0xf<<5);
	gpio->GPBDAT &=~(0xf<<5);
#if 0
	gpio->GPBCON &=~(2<<12);
	gpio->GPBDAT |=(1<<6);
	gpio->GPBCON &=~(2<<14);
	gpio->GPBDAT |=(1<<7);
	gpio->GPBCON &=~(2<<16);
	gpio->GPBDAT |=(1<<8);
#endif	
}