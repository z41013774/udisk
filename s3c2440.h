#ifndef __S3C24X0_H__
#define __S3C24X0_H__
typedef unsigned char 		uint8_t;
typedef unsigned short 		uint16_t;
typedef unsigned int 		uint32_t;


typedef volatile uint8_t		S3C24X0_REG8;
typedef volatile uint16_t		S3C24X0_REG16;
typedef volatile uint32_t		S3C24X0_REG32;
#define S3C24X0_UART_CHANNELS	3
#define S3C24X0_SPI_CHANNELS	2

/* S3C2440  supports 512 Byte HW ECC */
#define S3C2440_ECCSIZE		512
#define S3C2440_ECCBYTES	3

/* S3C2410 device base addresses */
#define S3C24X0_MEMCTL_BASE			0x48000000
#define S3C24X0_USB_HOST_BASE		0x49000000
#define S3C24X0_INTERRUPT_BASE		0x4A000000
#define S3C24X0_DMA_BASE			0x4B000000
#define S3C24X0_CLOCK_POWER_BASE	0x4C000000
#define S3C24X0_LCD_BASE			0x4D000000
#define S3C2410_NAND_BASE			0x4E000000
#define S3C24X0_UART_BASE			0x50000000
#define S3C24X0_TIMER_BASE			0x51000000
#define S3C24X0_USB_DEVICE_BASE		0x52000140
#define S3C24X0_WATCHDOG_BASE		0x53000000
#define S3C24X0_I2C_BASE			0x54000000
#define S3C24X0_I2S_BASE			0x55000000
#define S3C24X0_GPIO_BASE			0x56000000
#define S3C24X0_RTC_BASE			0x57000000
#define S3C2410_ADC_BASE			0x58000000
#define S3C24X0_SPI_BASE			0x59000000
#define S3C2410_SDI_BASE			0x5A000000


typedef enum {
	S3C24X0_UART0,
	S3C24X0_UART1,
	S3C24X0_UART2
} S3C24X0_UARTS_NR;


/* Memory controller (see manual chapter 5) */
typedef struct {
	S3C24X0_REG32	BWSCON;
	S3C24X0_REG32	BANKCON[8];
	S3C24X0_REG32	REFRESH;
	S3C24X0_REG32	BANKSIZE;
	S3C24X0_REG32	MRSRB6;
	S3C24X0_REG32	MRSRB7;
} /*__attribute__((__packed__))*/ S3C24X0_MEMCTL;


/* USB HOST (see manual chapter 12) */
typedef struct {
	S3C24X0_REG32	HcRevision;
	S3C24X0_REG32	HcControl;
	S3C24X0_REG32	HcCommonStatus;
	S3C24X0_REG32	HcInterruptStatus;
	S3C24X0_REG32	HcInterruptEnable;
	S3C24X0_REG32	HcInterruptDisable;
	S3C24X0_REG32	HcHCCA;
	S3C24X0_REG32	HcPeriodCuttendED;
	S3C24X0_REG32	HcControlHeadED;
	S3C24X0_REG32	HcControlCurrentED;
	S3C24X0_REG32	HcBulkHeadED;
	S3C24X0_REG32	HcBuldCurrentED;
	S3C24X0_REG32	HcDoneHead;
	S3C24X0_REG32	HcRmInterval;
	S3C24X0_REG32	HcFmRemaining;
	S3C24X0_REG32	HcFmNumber;
	S3C24X0_REG32	HcPeriodicStart;
	S3C24X0_REG32	HcLSThreshold;
	S3C24X0_REG32	HcRhDescriptorA;
	S3C24X0_REG32	HcRhDescriptorB;
	S3C24X0_REG32	HcRhStatus;
	S3C24X0_REG32	HcRhPortStatus1;
	S3C24X0_REG32	HcRhPortStatus2;
} /*__attribute__((__packed__))*/ S3C24X0_USB_HOST;


/* INTERRUPT (see manual chapter 14) */
typedef struct {
	S3C24X0_REG32	SRCPND;
	S3C24X0_REG32	INTMOD;
	S3C24X0_REG32	INTMSK;
	S3C24X0_REG32	PRIORITY;
	S3C24X0_REG32	INTPND;
	S3C24X0_REG32	INTOFFSET;
	S3C24X0_REG32	SUBSRCPND;
	S3C24X0_REG32	INTSUBMSK;
} /*__attribute__((__packed__))*/ S3C24X0_INTERRUPT;


/* DMAS (see manual chapter 8) */
typedef struct {
	S3C24X0_REG32	DISRC;
	S3C24X0_REG32	DISRCC;
	S3C24X0_REG32	DIDST;
	S3C24X0_REG32	DIDSTC;
	S3C24X0_REG32	DCON;
	S3C24X0_REG32	DSTAT;
	S3C24X0_REG32	DCSRC;
	S3C24X0_REG32	DCDST;
	S3C24X0_REG32	DMASKTRIG;
	S3C24X0_REG32	res[7];
} /*__attribute__((__packed__))*/ S3C24X0_DMA;

typedef struct {
	S3C24X0_DMA	dma[4];
} /*__attribute__((__packed__))*/ S3C24X0_DMAS;


/* CLOCK & POWER MANAGEMENT (see S3C2400 manual chapter 6) */
/*                          (see S3C2410 manual chapter 7) */
typedef struct {
	S3C24X0_REG32	LOCKTIME;
	S3C24X0_REG32	MPLLCON;
	S3C24X0_REG32	UPLLCON;
	S3C24X0_REG32	CLKCON;
	S3C24X0_REG32	CLKSLOW;
	S3C24X0_REG32	CLKDIVN;
} /*__attribute__((__packed__))*/ S3C24X0_CLOCK_POWER;


/* LCD CONTROLLER (see manual chapter 15) */
typedef struct {
	S3C24X0_REG32	LCDCON1;
	S3C24X0_REG32	LCDCON2;
	S3C24X0_REG32	LCDCON3;
	S3C24X0_REG32	LCDCON4;
	S3C24X0_REG32	LCDCON5;
	S3C24X0_REG32	LCDSADDR1;
	S3C24X0_REG32	LCDSADDR2;
	S3C24X0_REG32	LCDSADDR3;
	S3C24X0_REG32	REDLUT;
	S3C24X0_REG32	GREENLUT;
	S3C24X0_REG32	BLUELUT;
	S3C24X0_REG32	res[8];
	S3C24X0_REG32	DITHMODE;
	S3C24X0_REG32	TPAL;
	S3C24X0_REG32	LCDINTPND;
	S3C24X0_REG32	LCDSRCPND;
	S3C24X0_REG32	LCDINTMSK;
	S3C24X0_REG32	LPCSEL;

} /*__attribute__((__packed__))*/ S3C24X0_LCD;


/* NAND FLASH (see S3C2410 manual chapter 6) */
typedef struct {
	S3C24X0_REG32	NFCONF;
	S3C24X0_REG32	NFCONT;
	S3C24X0_REG32	NFCMD;
	S3C24X0_REG32	NFADDR;
	S3C24X0_REG32	NFDATA;
	S3C24X0_REG32	NFMECCD0;
	S3C24X0_REG32	NFMECCD1;
	S3C24X0_REG32	NFSECCD;
	S3C24X0_REG32	NFSTAT;
	S3C24X0_REG32	NFESTAT0;
	S3C24X0_REG32	NFESTAT1;
	S3C24X0_REG32	NFMECC0;
	S3C24X0_REG32	NFMECC1;
	S3C24X0_REG32	NFSECC;
	S3C24X0_REG32	NFSBLK;
	S3C24X0_REG32	NFEBLK;
} /*__attribute__((__packed__))*/ S3C2440_NAND;


/* UART (see manual chapter 11) */
typedef struct {
	S3C24X0_REG32	ULCON;
	S3C24X0_REG32	UCON;
	S3C24X0_REG32	UFCON;
	S3C24X0_REG32	UMCON;
	S3C24X0_REG32	UTRSTAT;
	S3C24X0_REG32	UERSTAT;
	S3C24X0_REG32	UFSTAT;
	S3C24X0_REG32	UMSTAT;

	S3C24X0_REG8	UTXH;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	URXH;
	S3C24X0_REG8	res2[3];
	S3C24X0_REG32	UBRDIV0;
} /*__attribute__((__packed__))*/ S3C24X0_UART;


/* PWM TIMER (see manual chapter 10) */
typedef struct {
	S3C24X0_REG32	TCNTB;
	S3C24X0_REG32	TCMPB;
	S3C24X0_REG32	TCNTO;
} /*__attribute__((__packed__))*/ S3C24X0_TIMER;

typedef struct {
	S3C24X0_REG32	TCFG0;
	S3C24X0_REG32	TCFG1;
	S3C24X0_REG32	TCON;
	S3C24X0_TIMER	ch[4];
	S3C24X0_REG32	TCNTB4;
	S3C24X0_REG32	TCNTO4;
} /*__attribute__((__packed__))*/ S3C24X0_TIMERS;


/* USB DEVICE (see manual chapter 13) */
typedef struct {
	S3C24X0_REG8	EP_FIFO_REG;
	S3C24X0_REG8	res[3];
} /*__attribute__((__packed__))*/ S3C24X0_USB_DEV_FIFOS;

typedef struct {

	S3C24X0_REG8	EP_DMA_CON;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	EP_DMA_UNIT;
	S3C24X0_REG8	res2[3];
	S3C24X0_REG8	EP_DMA_FIFO;
	S3C24X0_REG8	res3[3];
	S3C24X0_REG8	EP_DMA_TTC_L;
	S3C24X0_REG8	res4[3];
	S3C24X0_REG8	EP_DMA_TTC_M;
	S3C24X0_REG8	res5[3];
	S3C24X0_REG8	EP_DMA_TTC_H;
	S3C24X0_REG8	res6[3];
} /*__attribute__((__packed__))*/ S3C24X0_USB_DEV_DMAS;

typedef struct {

	S3C24X0_REG8	FUNC_ADDR_REG;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	PWR_REG;
	S3C24X0_REG8	res2[3];
	S3C24X0_REG8	EP_INT_REG;
	S3C24X0_REG8	res3[15];
	S3C24X0_REG8	USB_INT_REG;
	S3C24X0_REG8	res4[3];
	S3C24X0_REG8	EP_INT_EN_REG;
	S3C24X0_REG8	res5[15];
	S3C24X0_REG8	USB_INT_EN_REG;
	S3C24X0_REG8	res6[3];
	S3C24X0_REG8	FRAME_NUM1_REG;
	S3C24X0_REG8	res7[3];
	S3C24X0_REG8	FRAME_NUM2_REG;
	S3C24X0_REG8	res8[3];
	S3C24X0_REG8	INDEX_REG;
	S3C24X0_REG8	res9[7];
	S3C24X0_REG8	MAXP_REG;
	S3C24X0_REG8	res10[3];
	S3C24X0_REG8	EP0_CSR_IN_CSR1_REG;
	S3C24X0_REG8	res11[3];
	S3C24X0_REG8	IN_CSR2_REG;
	S3C24X0_REG8	res12[7];
	S3C24X0_REG8	OUT_CSR1_REG;
	S3C24X0_REG8	res13[3];
	S3C24X0_REG8	OUT_CSR2_REG;
	S3C24X0_REG8	res14[3];
	S3C24X0_REG8	OUT_FIFO_CNT1_REG;
	S3C24X0_REG8	res15[3];
	S3C24X0_REG8	OUT_FIFO_CNT2_REG;
	S3C24X0_REG8	res16[3];
	S3C24X0_REG8	res17[32];
	S3C24X0_USB_DEV_FIFOS	fifo[5];
	S3C24X0_USB_DEV_DMAS	dma[5];
} /*__attribute__((__packed__))*/ S3C24X0_USB_DEVICE;


/* WATCH DOG TIMER (see manual chapter 18) */
typedef struct {
	S3C24X0_REG32	WTCON;
	S3C24X0_REG32	WTDAT;
	S3C24X0_REG32	WTCNT;
} /*__attribute__((__packed__))*/ S3C24X0_WATCHDOG;


/* IIC (see manual chapter 20) */
typedef struct {
	S3C24X0_REG32	IICCON;
	S3C24X0_REG32	IICSTAT;
	S3C24X0_REG32	IICADD;
	S3C24X0_REG32	IICDS;
	S3C24X0_REG32 	IICLC;
} /*__attribute__((__packed__))*/ S3C24X0_I2C;


/* IIS (see manual chapter 21) */
typedef struct {

	S3C24X0_REG16	IISCON;
	S3C24X0_REG16	res1;
	S3C24X0_REG16	IISMOD;
	S3C24X0_REG16	res2;
	S3C24X0_REG16	IISPSR;
	S3C24X0_REG16	res3;
	S3C24X0_REG16	IISFCON;
	S3C24X0_REG16	res4;
	S3C24X0_REG16	IISFIFO;
	S3C24X0_REG16	res5;
} /*__attribute__((__packed__))*/ S3C24X0_I2S;


/* I/O PORT (see manual chapter 9) */
typedef struct {

	S3C24X0_REG32	GPACON;
	S3C24X0_REG32	GPADAT;
	S3C24X0_REG32	res1[2];
	S3C24X0_REG32	GPBCON;
	S3C24X0_REG32	GPBDAT;
	S3C24X0_REG32	GPBUP;
	S3C24X0_REG32	res2;
	S3C24X0_REG32	GPCCON;
	S3C24X0_REG32	GPCDAT;
	S3C24X0_REG32	GPCUP;
	S3C24X0_REG32	res3;
	S3C24X0_REG32	GPDCON;
	S3C24X0_REG32	GPDDAT;
	S3C24X0_REG32	GPDUP;
	S3C24X0_REG32	res4;
	S3C24X0_REG32	GPECON;
	S3C24X0_REG32	GPEDAT;
	S3C24X0_REG32	GPEUP;
	S3C24X0_REG32	res5;
	S3C24X0_REG32	GPFCON;
	S3C24X0_REG32	GPFDAT;
	S3C24X0_REG32	GPFUP;
	S3C24X0_REG32	res6;
	S3C24X0_REG32	GPGCON;
	S3C24X0_REG32	GPGDAT;
	S3C24X0_REG32	GPGUP;
	S3C24X0_REG32	res7;
	S3C24X0_REG32	GPHCON;
	S3C24X0_REG32	GPHDAT;
	S3C24X0_REG32	GPHUP;
	S3C24X0_REG32	res8;

	S3C24X0_REG32	MISCCR;
	S3C24X0_REG32	DCLKCON;
	S3C24X0_REG32	EXTINT0;
	S3C24X0_REG32	EXTINT1;
	S3C24X0_REG32	EXTINT2;
	S3C24X0_REG32	EINTFLT0;
	S3C24X0_REG32	EINTFLT1;
	S3C24X0_REG32	EINTFLT2;
	S3C24X0_REG32	EINTFLT3;
	S3C24X0_REG32	EINTMASK;
	S3C24X0_REG32	EINTPEND;
	S3C24X0_REG32	GSTATUS0;
	S3C24X0_REG32	GSTATUS1;
	S3C24X0_REG32	GSTATUS2;
	S3C24X0_REG32	GSTATUS3;
	S3C24X0_REG32	GSTATUS4;
	S3C24X0_REG32 	DSC0;
	S3C24X0_REG32	DSC1;

} /*__attribute__((__packed__))*/ S3C24X0_GPIO;


/* RTC (see manual chapter 17) */
typedef struct {
	S3C24X0_REG8	res0[64];
	S3C24X0_REG8	RTCCON;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	TICNT;
	S3C24X0_REG8	res2[11];
	S3C24X0_REG8	RTCALM;
	S3C24X0_REG8	res3[3];
	S3C24X0_REG8	ALMSEC;
	S3C24X0_REG8	res4[3];
	S3C24X0_REG8	ALMMIN;
	S3C24X0_REG8	res5[3];
	S3C24X0_REG8	ALMHOUR;
	S3C24X0_REG8	res6[3];
	S3C24X0_REG8	ALMDATE;
	S3C24X0_REG8	res7[3];
	S3C24X0_REG8	ALMMON;
	S3C24X0_REG8	res8[3];
	S3C24X0_REG8	ALMYEAR;
	S3C24X0_REG8	res9[3];
	S3C24X0_REG8	RTCRST;
	S3C24X0_REG8	res10[3];
	S3C24X0_REG8	BCDSEC;
	S3C24X0_REG8	res11[3];
	S3C24X0_REG8	BCDMIN;
	S3C24X0_REG8	res12[3];
	S3C24X0_REG8	BCDHOUR;
	S3C24X0_REG8	res13[3];
	S3C24X0_REG8	BCDDATE;
	S3C24X0_REG8	res14[3];
	S3C24X0_REG8	BCDDAY;
	S3C24X0_REG8	res15[3];
	S3C24X0_REG8	BCDMON;
	S3C24X0_REG8	res16[3];
	S3C24X0_REG8	BCDYEAR;
	S3C24X0_REG8	res17[3];
} /*__attribute__((__packed__))*/ S3C24X0_RTC;


/* ADC (see manual chapter 16) */
typedef struct {
	S3C24X0_REG32	ADCCON;
	S3C24X0_REG32	ADCDAT;
} /*__attribute__((__packed__))*/ S3C2400_ADC;


/* ADC (see manual chapter 16) */
typedef struct {
	S3C24X0_REG32	ADCCON;
	S3C24X0_REG32	ADCTSC;
	S3C24X0_REG32	ADCDLY;
	S3C24X0_REG32	ADCDAT0;
	S3C24X0_REG32	ADCDAT1;
} /*__attribute__((__packed__))*/ S3C2410_ADC;


/* SPI (see manual chapter 22) */
typedef struct {
	S3C24X0_REG8	SPCON;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	SPSTA;
	S3C24X0_REG8	res2[3];
	S3C24X0_REG8	SPPIN;
	S3C24X0_REG8	res3[3];
	S3C24X0_REG8	SPPRE;
	S3C24X0_REG8	res4[3];
	S3C24X0_REG8	SPTDAT;
	S3C24X0_REG8	res5[3];
	S3C24X0_REG8	SPRDAT;
	S3C24X0_REG8	res6[3];
	S3C24X0_REG8	res7[16];
} /*__attribute__((__packed__))*/ S3C24X0_SPI_CHANNEL;

typedef struct {
	S3C24X0_SPI_CHANNEL	ch[S3C24X0_SPI_CHANNELS];
} /*__attribute__((__packed__))*/ S3C24X0_SPI;


/* MMC INTERFACE (see S3C2400 manual chapter 19) */
typedef struct {

	S3C24X0_REG8	MMCON;
	S3C24X0_REG8	res1[3];
	S3C24X0_REG8	MMCRR;
	S3C24X0_REG8	res2[3];
	S3C24X0_REG8	MMFCON;
	S3C24X0_REG8	res3[3];
	S3C24X0_REG8	MMSTA;
	S3C24X0_REG8	res4[3];
	S3C24X0_REG16	MMFSTA;
	S3C24X0_REG16	res5;
	S3C24X0_REG8	MMPRE;
	S3C24X0_REG8	res6[3];
	S3C24X0_REG16	MMLEN;
	S3C24X0_REG16	res7;
	S3C24X0_REG8	MMCR7;
	S3C24X0_REG8	res8[3];
	S3C24X0_REG32	MMRSP[4];
	S3C24X0_REG8	MMCMD0;
	S3C24X0_REG8	res9[3];
	S3C24X0_REG32	MMCMD1;
	S3C24X0_REG16	MMCR16;
	S3C24X0_REG16	res10;
	S3C24X0_REG8	MMDAT;
	S3C24X0_REG8	res11[3];
} /*__attribute__((__packed__))*/ S3C2400_MMC;


/* SD INTERFACE (see S3C2410 manual chapter 19) */
typedef struct {
	S3C24X0_REG32	SDICON;
	S3C24X0_REG32	SDIPRE;
	S3C24X0_REG32	SDICARG;
	S3C24X0_REG32	SDICCON;
	S3C24X0_REG32	SDICSTA;
	S3C24X0_REG32	SDIRSP0;
	S3C24X0_REG32	SDIRSP1;
	S3C24X0_REG32	SDIRSP2;
	S3C24X0_REG32	SDIRSP3;
	S3C24X0_REG32	SDIDTIMER;
	S3C24X0_REG32	SDIBSIZE;
	S3C24X0_REG32	SDIDCON;
	S3C24X0_REG32	SDIDCNT;
	S3C24X0_REG32	SDIDSTA;
	S3C24X0_REG32	SDIFSTA;

	S3C24X0_REG8	SDIDAT;
	S3C24X0_REG8	res[3];

	S3C24X0_REG32	SDIIMSK;
} /*__attribute__((__packed__))*/ S3C2410_SDI;

static __inline S3C24X0_MEMCTL * S3C24X0_GetBase_MEMCTL(void)
{
	return (S3C24X0_MEMCTL * const)S3C24X0_MEMCTL_BASE;
}
static __inline S3C24X0_USB_HOST * S3C24X0_GetBase_USB_HOST(void)
{
	return (S3C24X0_USB_HOST * const)S3C24X0_USB_HOST_BASE;
}
static __inline S3C24X0_INTERRUPT * S3C24X0_GetBase_INTERRUPT(void)
{
	return (S3C24X0_INTERRUPT * const)S3C24X0_INTERRUPT_BASE;
}
static __inline S3C24X0_DMAS * S3C24X0_GetBase_DMAS(void)
{
	return (S3C24X0_DMAS * const)S3C24X0_DMA_BASE;
}
static __inline S3C24X0_CLOCK_POWER * S3C24X0_GetBase_CLOCK_POWER(void)
{
	return (S3C24X0_CLOCK_POWER * const)S3C24X0_CLOCK_POWER_BASE;
}
static __inline S3C24X0_LCD * S3C24X0_GetBase_LCD(void)
{
	return (S3C24X0_LCD * const)S3C24X0_LCD_BASE;
}
static __inline S3C2440_NAND * S3C2410_GetBase_NAND(void)
{
	return (S3C2440_NAND * const)S3C2410_NAND_BASE;
}
static __inline S3C24X0_UART * S3C24X0_GetBase_UART(S3C24X0_UARTS_NR nr)
{
	return (S3C24X0_UART * const)(S3C24X0_UART_BASE + (nr * 0x4000));
}
static __inline S3C24X0_TIMERS * S3C24X0_GetBase_TIMERS(void)
{
	return (S3C24X0_TIMERS * const)S3C24X0_TIMER_BASE;
}
static __inline S3C24X0_USB_DEVICE * S3C24X0_GetBase_USB_DEVICE(void)
{
	return (S3C24X0_USB_DEVICE * const)S3C24X0_USB_DEVICE_BASE;
}
static __inline S3C24X0_WATCHDOG * S3C24X0_GetBase_WATCHDOG(void)
{
	return (S3C24X0_WATCHDOG * const)S3C24X0_WATCHDOG_BASE;
}
static __inline S3C24X0_I2C * S3C24X0_GetBase_I2C(void)
{
	return (S3C24X0_I2C * const)S3C24X0_I2C_BASE;
}
static __inline S3C24X0_I2S * S3C24X0_GetBase_I2S(void)
{
	return (S3C24X0_I2S * const)S3C24X0_I2S_BASE;
}
static __inline S3C24X0_GPIO * S3C24X0_GetBase_GPIO(void)
{
	return (S3C24X0_GPIO * const)S3C24X0_GPIO_BASE;
}
static __inline S3C24X0_RTC * S3C24X0_GetBase_RTC(void)
{
	return (S3C24X0_RTC * const)S3C24X0_RTC_BASE;
}
static __inline S3C2410_ADC * S3C2410_GetBase_ADC(void)
{
	return (S3C2410_ADC * const)S3C2410_ADC_BASE;
}
static __inline S3C24X0_SPI * S3C24X0_GetBase_SPI(void)
{
	return (S3C24X0_SPI * const)S3C24X0_SPI_BASE;
}
static __inline S3C2410_SDI * S3C2410_GetBase_SDI(void)
{
	return (S3C2410_SDI * const)S3C2410_SDI_BASE;
}
#define _ISR_STARTADDRESS 	0x33ffff00 

// Exception vector
#define pISR_RESET		(*(unsigned *)(_ISR_STARTADDRESS+0x0))
#define pISR_UNDEF		(*(unsigned *)(_ISR_STARTADDRESS+0x4))
#define pISR_SWI		(*(unsigned *)(_ISR_STARTADDRESS+0x8))
#define pISR_PABORT		(*(unsigned *)(_ISR_STARTADDRESS+0xc))
#define pISR_DABORT		(*(unsigned *)(_ISR_STARTADDRESS+0x10))
#define pISR_RESERVED	(*(unsigned *)(_ISR_STARTADDRESS+0x14))
#define pISR_IRQ		(*(unsigned *)(_ISR_STARTADDRESS+0x18))
#define pISR_FIQ		(*(unsigned *)(_ISR_STARTADDRESS+0x1c))
// Interrupt vector
#define pISR_EINT0		(*(unsigned *)(_ISR_STARTADDRESS+0x20))
#define pISR_EINT1		(*(unsigned *)(_ISR_STARTADDRESS+0x24))
#define pISR_EINT2		(*(unsigned *)(_ISR_STARTADDRESS+0x28))
#define pISR_EINT3		(*(unsigned *)(_ISR_STARTADDRESS+0x2c))
#define pISR_EINT4_7	(*(unsigned *)(_ISR_STARTADDRESS+0x30))
#define pISR_EINT8_23	(*(unsigned *)(_ISR_STARTADDRESS+0x34))
#define pISR_CAM		(*(unsigned *)(_ISR_STARTADDRESS+0x38))		// Added for 2440.
#define pISR_BAT_FLT	(*(unsigned *)(_ISR_STARTADDRESS+0x3c))
#define pISR_TICK		(*(unsigned *)(_ISR_STARTADDRESS+0x40))
#define pISR_WDT_AC97	(*(unsigned *)(_ISR_STARTADDRESS+0x44))   //Changed to pISR_WDT_AC97 for 2440A 
#define pISR_TIMER0	 	(*(unsigned *)(_ISR_STARTADDRESS+0x48))
#define pISR_TIMER1	 	(*(unsigned *)(_ISR_STARTADDRESS+0x4c))
#define pISR_TIMER2		(*(unsigned *)(_ISR_STARTADDRESS+0x50))
#define pISR_TIMER3		(*(unsigned *)(_ISR_STARTADDRESS+0x54))
#define pISR_TIMER4		(*(unsigned *)(_ISR_STARTADDRESS+0x58))
#define pISR_UART2		(*(unsigned *)(_ISR_STARTADDRESS+0x5c))
#define pISR_LCD		(*(unsigned *)(_ISR_STARTADDRESS+0x60))
#define pISR_DMA0		(*(unsigned *)(_ISR_STARTADDRESS+0x64))
#define pISR_DMA1		(*(unsigned *)(_ISR_STARTADDRESS+0x68))
#define pISR_DMA2		(*(unsigned *)(_ISR_STARTADDRESS+0x6c))
#define pISR_DMA3		(*(unsigned *)(_ISR_STARTADDRESS+0x70))
#define pISR_SDI		(*(unsigned *)(_ISR_STARTADDRESS+0x74))
#define pISR_SPI0		(*(unsigned *)(_ISR_STARTADDRESS+0x78))
#define pISR_UART1		(*(unsigned *)(_ISR_STARTADDRESS+0x7c))
#define pISR_NFCON		(*(unsigned *)(_ISR_STARTADDRESS+0x80))		// Added for 2440.
#define pISR_USBD		(*(unsigned *)(_ISR_STARTADDRESS+0x84))
#define pISR_USBH		(*(unsigned *)(_ISR_STARTADDRESS+0x88))
#define pISR_IIC		(*(unsigned *)(_ISR_STARTADDRESS+0x8c))
#define pISR_UART0		(*(unsigned *)(_ISR_STARTADDRESS+0x90))
#define pISR_SPI1		(*(unsigned *)(_ISR_STARTADDRESS+0x94))
#define pISR_RTC		(*(unsigned *)(_ISR_STARTADDRESS+0x98))
#define pISR_ADC		(*(unsigned *)(_ISR_STARTADDRESS+0x9c))


// PENDING BIT
#define BIT_EINT0		(0x1)
#define BIT_EINT1		(0x1<<1)
#define BIT_EINT2		(0x1<<2)
#define BIT_EINT3		(0x1<<3)
#define BIT_EINT4_7		(0x1<<4)
#define BIT_EINT8_23	(0x1<<5)
#define BIT_CAM			(0x1<<6)		// Added for 2440.
#define BIT_BAT_FLT		(0x1<<7)
#define BIT_TICK		(0x1<<8)
#define BIT_WDT_AC97	(0x1<<9)	// Changed from BIT_WDT to BIT_WDT_AC97 for 2440A  
#define BIT_TIMER0		(0x1<<10)
#define BIT_TIMER1		(0x1<<11)
#define BIT_TIMER2		(0x1<<12)
#define BIT_TIMER3		(0x1<<13)
#define BIT_TIMER4		(0x1<<14)
#define BIT_UART2		(0x1<<15)
#define BIT_LCD			(0x1<<16)
#define BIT_DMA0		(0x1<<17)
#define BIT_DMA1		(0x1<<18)
#define BIT_DMA2		(0x1<<19)
#define BIT_DMA3		(0x1<<20)
#define BIT_SDI			(0x1<<21)
#define BIT_SPI0		(0x1<<22)
#define BIT_UART1		(0x1<<23)
#define BIT_NFCON		(0x1<<24)		// Added for 2440.
#define BIT_USBD		(0x1<<25)
#define BIT_USBH		(0x1<<26)
#define BIT_IIC			(0x1<<27)
#define BIT_UART0		(0x1<<28)
#define BIT_SPI1		(0x1<<29)
#define BIT_RTC			(0x1<<30)
#define BIT_ADC			((uint32_t)0x1<<31)
#define BIT_ALLMSK		(0xffffffff)

#define BIT_SUB_ALLMSK	(0x7fff)			//Changed from 0x7ff to 0x7fff for 2440A 
#define BIT_SUB_AC97	(0x1<<14)		//Added for 2440A 
#define BIT_SUB_WDT		(0x1<<13)		//Added for 2440A 
#define BIT_SUB_CAM_P	(0x1<<12)		// edited for 2440A.
#define BIT_SUB_CAM_C   (0x1<<11)       // edited for 2440A
#define BIT_SUB_ADC		(0x1<<10)
#define BIT_SUB_TC		(0x1<<9)
#define BIT_SUB_ERR2	(0x1<<8)
#define BIT_SUB_TXD2	(0x1<<7)
#define BIT_SUB_RXD2	(0x1<<6)
#define BIT_SUB_ERR1	(0x1<<5)
#define BIT_SUB_TXD1	(0x1<<4)
#define BIT_SUB_RXD1	(0x1<<3)
#define BIT_SUB_ERR0	(0x1<<2)
#define BIT_SUB_TXD0	(0x1<<1)
#define BIT_SUB_RXD0	(0x1<<0)

#endif

