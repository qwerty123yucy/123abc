#ifndef	__RTC_H
#define	__RTC_H

#include <stdint.h>
#include <bkp.h>
// PRL ALR CNT and DIV are backup regs

// default prescale and count value
#define RTC_PRL_VAL		(32767U)
#define RTC_PRLH_VAL		(RTC_PRL_VAL >> 16)
#define RTC_PRLL_VAL		(RTC_PRL_VAL & 0xffff)

#define RTC_CNT_VAL		(1666024187U)
#define RTC_CNTH_VAL		(RTC_CNT_VAL >> 16)
#define RTC_CNTL_VAL		(RTC_CNT_VAL & 0xffff)


#define RTC_CRH			(0x00)		//enable interrupts
#define RTC_CRH_OWIE		(1U << 2)	//overflow
#define RTC_CRH_ALRIE		(1U << 1)	//alarm
#define RTC_CRH_SECIE		(1U << 0)	//sec

#define RTC_CRL			(0x04)		
#define RTC_CRL_RTOFF		(1U << 5)	//1: last write terminated
#define RTC_CRL_CNF		(1U << 4)	//set 1 to enter conf mode (wait RTOFF turnning to 1 first)
#define RTC_CRL_RSF		(1U << 3)	//1: register already sync
#define RTC_CRL_OWF		(1U << 2)	//1: overflow detected
#define RTC_CRL_ALRF		(1U << 1)	//1: alarm detected
#define RTC_CRL_SECF		(1U << 0)	//1: sec detected


// TR_CLK = RTC_CLK / (PRL[19:0] + 1)
#define RTC_PRLH		(0x08)		// bit 3-0 as PRL19-16
#define RTC_PRLL		(0x0c)		// bit 15-0 as PRL 15-0


#define RTC_DIVH		(0x10)		//bit 3-0 as DIV19-16
#define RTC_DIVL		(0x14)		//bit 15-0 as DIV15-0


#define RTC_CNTH		(0x18)		//bit 3-0 as CNT19-16
#define RTC_CNTL		(0x1c)		//bit 3-0 as CNT19-16


#define RTC_ALRH		(0x20)		//bit15-0 as ALR31-16
#define RTC_ALRL		(0x24)		//bit15-0 as ALR15-0
						

// RTC CHECK (start from BKP_BASE)
#define RTC_CHECKR		BKP_DR(1)	// using this bkp register to detect reset on RTC
#define RTC_CHECK_VAL		0xf0ff		

void init_rtc();
uint32_t rtc_read_cnt();


#endif
