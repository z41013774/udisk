#ifndef __S3C2440_CLK_POWER__
#define __S3C2440_CLK_POWER__

#define PDIVN	1
#define HDIVN	2
#define DIVN_UPLL	1

#define MDIV	92
#define PDIV	1
#define SDIV	1
#define FIN 	12000000/*input clk 12MHZ*/
typedef struct _CLKPARA
{
	int pclk;
	int fclk;
	int hclk;
}CLKPARA;

extern CLKPARA clkpara;
void initsystemclk(void);
void usb_clk_96M();

#endif
