#ifndef __S3C2440_MMU_H__
#define __S3C2440_MMU_H__
#include "head.h"
#ifdef __cplusplus
extern "C" {
#endif

#define _MMUTT_STARTADDRESS	0x33ff8000


#define DESC_SEC		(0x2|(1<<4))
#define CB				(3<<2)  //cache_on, write_back
#define CNB				(2<<2)  //cache_on, write_through 
#define NCB             (1<<2)  //cache_off,WR_BUF on
#define NCNB			(0<<2)  //cache_off,WR_BUF off
#define AP_RW			(3<<10) //supervisor=RW, user=RW
#define AP_RO			(2<<10) //supervisor=RW, user=RO

#define DOMAIN_FAULT	(0x0)
#define DOMAIN_CHK		(0x1) 
#define DOMAIN_NOTCHK	(0x3) 
#define DOMAIN0			(0x0<<5)
#define DOMAIN1			(0x1<<5)

#define DOMAIN0_ATTR	(DOMAIN_CHK<<0) 
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2) 

#define RW_CB			(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB			(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_NCNB			(AP_RW|DOMAIN0|NCNB|DESC_SEC)
#define RW_FAULT		(AP_RW|DOMAIN1|NCNB|DESC_SEC)

int SET_IF(void);
void WR_IF(int cpsrValue);
void CLR_IF(void);
void EnterCritical(uint32_t*pSave);
void ExitCritical(uint32_t *pSave);
void MMU_EnableICache(void);
void MMU_DisableICache(void);
void MMU_EnableDCache(void);
void MMU_DisableDCache(void);
void MMU_EnableAlignFault(void);
void MMU_DisableAlignFault(void);
void MMU_EnableMMU(void);
void MMU_DisableMMU(void);
void MMU_SetTTBase(uint32_t base);
void MMU_SetDomain(uint32_t domain);

void MMU_SetFastBusMode(void);  //GCLK=HCLK
void MMU_SetAsyncBusMode(void); //GCLK=FCLK @(FCLK>=HCLK)

void MMU_InvalidateIDCache(void);
void MMU_InvalidateICache(void);
void MMU_InvalidateICacheMVA(uint32_t mva);
void MMU_PrefetchICacheMVA(uint32_t mva);
void MMU_InvalidateDCache(void);
void MMU_InvalidateDCacheMVA(uint32_t mva);
void MMU_CleanDCacheMVA(uint32_t mva);
void MMU_CleanInvalidateDCacheMVA(uint32_t mva);
void MMU_CleanDCacheIndex(uint32_t index);
void MMU_CleanInvalidateDCacheIndex(uint32_t index);	
void MMU_WaitForInterrupt(void);
	
void MMU_InvalidateTLB(void);
void MMU_InvalidateITLB(void);
void MMU_InvalidateITLBMVA(uint32_t mva);
void MMU_InvalidateDTLB(void);
void MMU_InvalidateDTLBMVA(uint32_t mva);

void MMU_SetDCacheLockdownBase(uint32_t base);
void MMU_SetICacheLockdownBase(uint32_t base);

void MMU_SetDTLBLockdown(uint32_t baseVictim);
void MMU_SetITLBLockdown(uint32_t baseVictim);

void MMU_SetProcessId(uint32_t pid);


void MMU_Init(void);
void MMU_SetMTT(int vaddrStart,int vaddrEnd,int paddrStart,int attr);


#ifdef __cplusplus
}
#endif

#endif
