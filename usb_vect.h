#ifndef __USB_VECT_H__
#define __USB_VECT_H__
#define 	TOTALINTRSOURCE		32
typedef void  (*PVECT)(void);
//void  HandlerIRQ(void);
void s3c2440_init_vect(void);

extern PVECT pvect[TOTALINTRSOURCE];
#endif

