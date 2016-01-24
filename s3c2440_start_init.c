#include "s3c2440.h"

/*disable watchdog*/
void disable_watch_dog()
{
	S3C24X0_WATCHDOG *const watchdog = S3C24X0_GetBase_WATCHDOG();
	watchdog->WTCON = 0;
}

void maskintr()
{
	S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();
	intr->INTMSK = 0xffffffff;
	intr->INTSUBMSK = 0xffff;
}
void memoryconfig()
{
	int i;
	S3C24X0_MEMCTL *const memctl = S3C24X0_GetBase_MEMCTL();
	memctl->BWSCON = (2<<28)|(2<<24)|(1<<20)|(2<<16)|(1<<12)|(1<<8)|(1<<4)|(1<<1);
	for(i=0;i<6;i++)
		memctl->BANKCON[i] = 0x0700;
	memctl->BANKCON[6] = (3<<15)|(1<<2)|(1<<0);
	memctl->BANKCON[7] = (3<<15)|(1<<2)|(1<<0);
	memctl->REFRESH = (1<<23)|(0<<22)|(1<<20)|(1<<18)|(1268);
	memctl->BANKSIZE = (1<<5)|(1<<4)|2;
	memctl->MRSRB6 = 0x30;
	memctl->MRSRB7 = 0x30;
}
#if 0
void initstack()
{

}

/*code position*/
/*OM[1:0] != 0 NOR FLash boot*/
/*OM[1:0] == 0, NAND FLash boot*/
void judgecodeposition()
{
	S3C24X0_MEMCTL *const memctl = S3C24X0_GetBase_MEMCTL();
	if(memctl->BWSCON & 0x6)
	{
		/*boot form nandflash*/
		
	}
	else if(!(memctl->BWSCON & 0x6))
	{
		/*boot form norflash*/
		/*init zi bss*/
	}
}
#endif
void copycodetoddr()
{

}

