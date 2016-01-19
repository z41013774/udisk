//#include "head.h"
#include "s3c2440.h"
#include "usb_vect.h"
#define		STORAGEVECTADDR		0x33FFFF20
PVECT pvect[TOTALINTRSOURCE];

void s3c2440_init_vect()
{
	int cnt=0;
	//pvect[0] = HandlerIRQ;
	for(cnt=0;cnt<TOTALINTRSOURCE;cnt++)
		pvect[cnt] = 0;
	

}
#if 0
void  HandlerIRQ()
{
	 S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();
	 pvect[intr->INTOFFSET]();
}
#endif

