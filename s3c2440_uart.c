#include "head.h"
#include <stdarg.h> 

#define UARTBUFSIZE	256
#define UARTLOGSIZE	(UARTBUFSIZE * 4)
uint8_t sprint_buf[UARTBUFSIZE];
uint8_t log_buf[UARTLOGSIZE];

void init_uart()
{	/*get uart0 reg addr*/
	S3C24X0_UART  *const uart0 = S3C24X0_GetBase_UART(0);
	/*stop bit:1bit,word len 8bit,no parity bit*/
	uart0->ULCON = ULCON_PARITY_MODE | ULCON_STOP_BIT | ULCON_WORD_LEN;
	/*pclk,tx and rx int type:level,trans and recv mode :poll or interrupt*/
	uart0->UCON = UCON_CLOCK_SEL|UCON_TX_INT_TYPE|UCON_RX_INT_TYPE|UCON_RX_TIME_OUT|UCON_RX_ERROR_STS|UCON_TX_MODE|UCON_RX_MODE;
	
}
void init_baud(uint32_t baud)
{
	uint32_t ubrdivn;
	S3C24X0_UART  * const uart0 = S3C24X0_GetBase_UART(0);
	/*stop bit:1bit,word len 8bit,no parity bit*/
	uart0->ULCON = ULCON_PARITY_MODE | ULCON_STOP_BIT | ULCON_WORD_LEN;
	/*pclk,tx and rx int type:level,trans and recv mode :poll or interrupt*/
	uart0->UCON = UCON_CLOCK_SEL|UCON_TX_INT_TYPE|UCON_RX_INT_TYPE|UCON_RX_TIME_OUT|UCON_RX_ERROR_STS|UCON_TX_MODE|UCON_RX_MODE;

	ubrdivn =  (clkpara.pclk/(baud*16));
	ubrdivn -= 1;
	uart0->UBRDIV0 = ubrdivn;
}
void put_onechar(uint8_t ch)
{
	S3C24X0_UART *const uart0 = S3C24X0_GetBase_UART(0);
	while(!(uart0->UTRSTAT & UTRSTAT_TRANS_EMPTY))
		;
	uart0->UTXH = ch;
	
}
uint8_t get_onechar()
{
	uint8_t value;
	S3C24X0_UART *const uart0 = S3C24X0_GetBase_UART(0);
	while(!(uart0->UTRSTAT & UTRSTAT_RECV_BUF_READY))
		;
	value = uart0->URXH;
	return value;
}
#if 0
volatile uint16_t pos = 0;
volatile uint16_t cur_time =0;
void record_log(uint8_t ch)
{
	log_buf[pos] = ch;
	if(pos == (UARTLOGSIZE-4))
	{
		pos = 0;
		dbg_printf("%d",cur_time);

	}
	else
	{
		pos++;
	}
		
}
void dbg_printf(char *fmt,...)
{
	va_list args;
	int i,n;
	va_start(args, fmt);
	n = vsprintf(log_buf, fmt, args);
	va_end(args);
	if(n>UARTLOGSIZE)
		n=UARTLOGSIZE;
	for(i=0;i<n;i++)		
		put_onechar(log_buf[i]);

}

#else
void dbg_printf(char *fmt,...)
{
	va_list args;
	int i,n;
	va_start(args, fmt);
	n = vsprintf(sprint_buf, fmt, args);
	va_end(args);
	if(n>UARTBUFSIZE)
		n=UARTBUFSIZE;
	for(i=0;i<n;i++)		
		put_onechar(sprint_buf[i]);

}
#endif
