#ifndef __TIMER_H
#define	__TIMER_H

#include <bits.h>

#define PRESCALE_VALUE	(8999U)		// 36000000hz APB1 bus freq becomes 4000Mhz
#define	ARR_VALUE	(3999U)		// on tim2, with the prescare above, 2hz timer event is generated 


// tim 2-7
#define TIM_CR1		(0x00)		// 16 bit register  (other bits reserved)
#define TIM_CR1_CKD	(3U << 8)	// affects ETR ???
#define TIM_CR1_ARPE	(1U << 7)	// whether TIM_ARR is buffered
#define TIM_CR1_CMS	(3U << 5)	// Center-aligned mode selection (set to 0 enable edge-aligned mode)
#define	TIM_CR1_DIR	(1U << 4)	// when edge-aligned mode is enabled, this bit determines whether counter goes up or down
#define TIM_CR1_OPM	(1U << 3)	// one pulse mode (when enabled, the counter will stop after generating an event)
#define TIM_CR1_URS	(1U << 2)	// set to 1 to only enable interrupt on overflow/underflow (0 seems to adapt all UEVs)
#define TIM_CR1_UDIS	(1U << 1)	// set to 0 to enable UEV generation
#define TIM_CR1_CEN	(1U << 0)	// counter enable bit
					

#define TIM_CR2		(0x04)		// 16 bit register (other bits reserved)
#define TIM_CR2_TI1S	(1U << 7)	// ??? (just keep 0)
#define TIM_CR2_MMS	(7U << 4)	// master mode selection (just keep 0)
#define TIM_CR2_CCDS	(1U << 3)	// DMA function
					// bit 2-0 reserved

#define TIM_SMCR	(0x08)		// controls the behavior in slave mode

#define TIM_DIER	(0x0c)		// about interrupt and DMA (other bits reserved)
#define TIM_DIER_TDE	(1U << 14)	// enable DMA
					// bit 13 need to be reserved
					// bit 12-8 are about DMA
					// bit 7 is reserved
#define TIM_DIER_TIE	(1U << 6)	// enable interrupt trigger
					// bit 5 is reserved
// there are 4 channels of interrupts
#define TIM_DIER_CC4IE	(1U << 4)	// enable CC4 interrupt
#define TIM_DIER_CC3IE	(1U << 3)	// enable CC3 interrupt
#define TIM_DIER_CC2IE	(1U << 2)	// enable CC2 interrupt
#define TIM_DIER_CC1IE	(1U << 1)	// enable CC1 interrupt
#define TIM_DIER_UIE	(1U << 0)	// enable update interrupt


#define TIM_SR		(0x10)		// status register
#define TIM_SR_TIF	(1U << 6)	// indicates whether there is an interrupt pending
#define TIM_SR_UIF	(1U << 0)	// update interrupt flag (when UDIS is 0, overflow/underflow turns this bit to 1)


#define TIM_EGR		(0x14)		// event generate register (generating event by software)
#define TIM_EGR_TG	(1U << 6)	// software set this to 1 to trigger an interrupt 


#define TIM_CCMR1	(0x18)		// capture/compare mode register 1 
#define TIM_CCMR2	(0x1c)

#define TIM_CCER	(0x20)

#define TIM_CNT		(0x24)		// the counter
#define TIM_PSC		(0x28)		// prescare register
#define TIM_ARR		(0x2c)		// auto reload register

#define TIM_CCR(x)	(0x34 + ((x) << 2))	//x from 1-4 (CC1..CC4)
#define TIM_DCR		(0x48)		// DMA control register
#define TIM_DMAR	(0x4c)

#ifndef __ASSEMBLY__

#include <stdint.h>
#include <stdbool.h>

enum general_purpose_timer {tim2 = 2, tim3, tim4, tim5};
void timer_general_init(enum general_purpose_timer tim, uint16_t prescale, uint16_t arr);
void timer_general_on(enum general_purpose_timer tim, bool enable_interrupt);
void timer_general_off(enum general_purpose_timer tim);
uint32_t tim2_trigger();
#endif




#endif
