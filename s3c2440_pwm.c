#include "head.h"
/* timer0: control speaker
*  timer1: delay
*  timer2: control led
*  timer3: delta
*/
/*delay use time1*/


void delay_ms(int ms)
{
	S3C24X0_TIMERS  * const timers = S3C24X0_GetBase_TIMERS();
	timers->ch[1].TCNTB = 125000;
	timers->ch[1].TCMPB = 1;
	timers->TCON &= ~(0xf<<8);
	timers->TCON |= (1<<11)|(1<<9)|(1<<8);
	timers->TCON &= ~(2<<8);
	
	while(ms--)
	{
		/*wait TCNTO 0*/
		while(timers->ch[1].TCNTO >=1)
			;
		while(timers->ch[1].TCNTO < 1)
			;
	}
	
}
static void  __irq timer2_isr(void)
{
	S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();
	/*clear intr*/
	intr->SRCPND |= (1<<12);
	intr->INTPND |= (1<<12);
	dbg_printf("timer2_isr \r\n");
}
static void __irq timer1_isr(void)
{
	dbg_printf("timer1 isr\r\n");
}
void cfg_timer(void)
{
	S3C24X0_TIMERS	* const timers = S3C24X0_GetBase_TIMERS();
	S3C24X0_INTERRUPT *const intr = S3C24X0_GetBase_INTERRUPT();

	/*config pre0&pre1 prescaler value 3*/
	timers->TCFG0 = (0x03<<8)|0x03;
	/*all timer ,divider value 4*/
	/*so,timer0-timer3 clk are 3.125MHZ(0.32us).*/
	//timers->TCFG1 = (TIMER4DIV<<16)|(TIMER3DIV<<12)|(TIMER2DIV<<8)|(TIMER1DIV<<4)|TIMER0DIV;
	//timer2 isr fun,enable timer2 intr
#if 0
	pvect[INT_TIMER2] = timer2_interrupt;
	pvect[INT_TIMER1] = timer1_isr;
	
#endif
intr->INTMSK &=~(1<<INT_TIMER2);
intr->INTMSK &=~(1<<INT_TIMER1);
//pISR_TIMER1 = (unsigned)timer1_isr;
pISR_TIMER2 = (unsigned)timer2_isr;
}

/*control SPEAKE*/
void timer0_init()
{
	/*config timer0,timer1*/
	/*config prescaler0*/
	S3C24X0_TIMERS  * const timers = S3C24X0_GetBase_TIMERS();
	/*only config pre0*/
	timers->TCFG0 = 0x03;
	/*configture timer0,timer1.8 pre*/
	timers->TCFG1 = (2<<4)|(2<<0);

	timers->ch[0].TCMPB = clkpara.pclk >>1;
	timers->ch[0].TCNTB = clkpara.pclk;

}
void timer0_start()
{
	S3C24X0_TIMERS  * const timers = S3C24X0_GetBase_TIMERS();
	timers->TCON |= START;
}
void timer0_stop()
{
	S3C24X0_TIMERS * const timers = S3C24X0_GetBase_TIMERS();
	timers->TCON &= (START<<0);
}
void timer2_init(void)
{
	S3C24X0_TIMERS  * const timers = S3C24X0_GetBase_TIMERS();
	timers->ch[2].TCNTB = 125000;
	timers->ch[2].TCMPB = 1;
	timers->TCON &= ~(0xf<<12);
	timers->TCON |= (1<<15)|(1<<13)|(1<<12);
	//timers->TCON &= ~(2<<12);
}
void timer2_start(void)
{
	S3C24X0_TIMERS  * const timers = S3C24X0_GetBase_TIMERS();
	timers->TCON &= ~(2<<12);
}
void timer2_stop(void)
{
	S3C24X0_TIMERS	* const timers = S3C24X0_GetBase_TIMERS();
	timers->TCON &= ~(1<<12);

}
