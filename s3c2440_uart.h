#include "s3c2440.h"
#ifndef __S3C2440_UART__
#define __S3C2440_UART__
//#define USBLOG				
#define ULCON_PARITY_MODE		(0<<3)
#define ULCON_STOP_BIT			(0<<2)
#define ULCON_WORD_LEN			(3<<0)

#define UCON_CLOCK_SEL			(2<<10)
#define UCON_TX_INT_TYPE		(1<<9)
#define UCON_RX_INT_TYPE		(1<<8)
#define UCON_RX_TIME_OUT		(1<<7)
#define UCON_LOOPBACK_MODE		(0<<5)
#define UCON_RX_ERROR_STS		(1<<6)
#define UCON_TX_MODE			(01<<2)
#define UCON_RX_MODE			(01<<0)

#define UTRSTAT_TRANS_EMPTY		(1<<2)
#define UTRSTAT_TRANS_BUF_EMPYT	(1<<1)
#define UTRSTAT_RECV_BUF_READY	(1<<0)

#define UERSTAT_BREAK_DETECT	(1<<3)
#define UERSTAT_FRAME_ERROR		(1<<2)
#define UERSTAT_PARITY_ERROR	(1<<1)
#define UERSTAT_OVERRUN_ERROR	(1<<0)

#define UFSTAT_TX_FIFO_FULL		(1<<14)
#define UFSTAT_TX_FIFO_COUNT	
#define UFSTAT_RX_FIFO_FULL		(1<<6)
#define UFSTAT_RX_FIFO_COUNT



#define UBRDIV					

void init_uart(void);
void put_onechar(uint8_t ch);
uint8_t get_onechar(void);
void init_baud(uint32_t baud);
void dbg_printf(char *fmt,...);

#endif
