#include "head.h"

CLKPARA clkpara ;

void initsystemclk()
{

	int delay=0;
	
	S3C24X0_CLOCK_POWER *const clk = S3C24X0_GetBase_CLOCK_POWER();
	clk->LOCKTIME = 0xffffff;
	clk->UPLLCON = ((56<<12)|(2<<4)|(1));
	for(delay = 0;delay<100;delay++)
		;
	/*mpll 400M*/
	clk->MPLLCON = (MDIV<<12)|(PDIV<<4)|(SDIV);
	
	/*upclk 48M*/
	clk->CLKDIVN = (HDIVN<<1)|PDIVN;
	/*upll 48M*/
	//clk->UPLLCON = (56<<12)|(2<<4)|(2);
	/*dealy for pll */
	for(delay = 0;delay<100;delay++)
		;
	/*FCLK=400M,HCLK=100M,PCLK=50M*/
	clkpara.fclk = ((2*(MDIV+8)*12)/((PDIV+2)*2))*1000000;
	//clkpara.fclk = ((2*(MDIV+8)*FIN)/((PDIV+2)*2));
	clkpara.hclk = clkpara.fclk/4;
	clkpara.pclk = clkpara.hclk/2;
	
}
void usb_clk_96M()
{
	S3C24X0_CLOCK_POWER *const clk = S3C24X0_GetBase_CLOCK_POWER();
	/*upclk 48M*/
	clk->CLKDIVN = (DIVN_UPLL<<3) |(HDIVN<<1)|PDIVN;
}

