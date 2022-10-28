#ifndef __ADC_H
#define __ADC_H

#include <stm32f103.h>
#include <stdint.h>
#include <stdbool.h>

#define ADC_SR				(0x00)
#define ADC_SR_STRT			(1U << 4)	// indicates starting of regular conversion
#define ADC_SR_JSTRT			(1U << 3)	// indicates ....        inject ....
#define ADC_SR_JEOC			(1U << 2)	// end of convertion of inject
#define ADC_SR_EOC			(1U << 1)	// ...
#define ADC_SR_AWD			(1U << 0)	// analog watchdog event occured

#define ADC_CR1				(0x04)		
#define ADC_CR1_AWDEN			(1U << 23)	// analog watchdog enable
#define ADC_CR1_JAWDEN			(1U << 22)	// ...
#define ADC_CR1_DUALMOD_SHIFT(x)	((x) << 16)	// dual mode selection (4 bit, 0000 indicates independent mode)
#define ADC_CR1_DISCNUM_SHIFT(x)	((x) << 13)	// Discontinuous mode channel count (3 bit)
#define ADC_CR1_DISCNUM_MASK		(ADC_CR1_DISCNUM_SHIFT(0b111))
#define ADC_CR1_DISCNUM_MAX		(7U)
#define ADC_CR1_JDISCEN			(1U << 12)	// enable Discontinuous mode on injected channels
#define ADC_CR1_DISCEN			(1U << 11)	// ...
#define ADC_CR1_JAUTO			(1U << 10)	// enable Automatic injected group conversion
#define ADC_CR1_AWDSGL			(1U << 9)	// 1: analog watchdog enable on single channel. 0: all channel
#define ADC_CR1_SCAN			(1U << 8)	// scan mode
#define ADC_CR1_JEOCIE			(1U << 7)	// enable interrupt for inject at end of conversion
#define ADC_CR1_AWDIE			(1U << 6)	// analog watchdog interrupt enable
#define ADC_CR1_EOCIE			(1U << 5)	// enable interrupt for regular at end of conversion
#define ADC_CR1_AWDCH_SHIFT(x)		((x) << 0)	// select analog watchdog channel  (5 bits)
#define ADC_CR1_AWDCH_MASK		(ADC_CR1_AWDCH_SHIFT(0b11111))

#define ADC_CR2				(0x08)
#define ADC_CR2_TSVREFE			(1U << 23)	// ADC1 only. enable Temperature sensor and V(refint) connected to ADC1
#define ADC_CR2_SWSTART			(1U << 22)	// start regular conversion
#define ADC_CR2_JSWSTART		(1U << 21)	// ...   inject
#define ADC_CR2_EXTTRIG			(1U << 20)	// enable regular conversion on external event 
#define ADC_CR2_EXTSEL_SHIFT(x)		((x) << 17)	// 3 bit. select external event to trigger regular conversion
#define ADC_CR2_JEXTTRIG                (1U << 15)      // enable inject conversion on external event 
#define ADC_CR2_JEXTSEL_SHIFT(x)        ((x) << 12)     // 3 bit. select external event to trigger inject conversion
#define ADC_CR2_ALIGN			(1U << 11)	// 1: left align	0: right
#define ADC_CR2_DMA			(1U << 8)	// enable DMA mode	(only ADC1 and ADC3)
#define ADC_CR2_RSTCAL			(1U << 3)	// set 1 to initialize caliration. 0 indicates initialized
#define	ADC_CR2_CAL			(1U << 2)	// enable calibration
#define ADC_CR2_CONT			(1U << 1)	// 1: continuous mode 0: single
#define ADC_CR2_ADON			(1U << 0)	// enable ADC (shouldnt set this bit together with other bits)


#define ADC_SMPR(ch)			(0x0c + ((ch)>9 ? 0x00 : 0x04))  // determine the sample cycle for each channel
#define ADC_SMPR_SHIFT(ch, val)		((ch)>9 ? (val)<<(((ch)-10)*3) : (val)<<((ch)*3))
#define ADC_SMPR_MASK(ch)		(ADC_SMPR_SHIFT(ch, 0b111))


#define ADC_JOFR(x)			(0x14 + (((x)) << 2))	// x = 0..3 (reference manual 1..4)


#define ADC_HTR				(0x24)		// watchdog high threshold  (bit [11:0])
#define ADC_LTR                         (0x28)          // watchdog low threshold  (bit [11:0])
#define ADC_HLTR_MAX			((1U << 12) - 1)



#define ADC_SQR(num)			(0x34 - (((num) / 6) << 2))    // num = 0..15  (reference manual 1..16)
#define ADC_SQR_MAX			(15U)
#define ADC3_SQR_MAX			(7U)
#define ADC_SQR_SHIFT(num, val)		((val) << (((num) % 6) * 5))
#define ADC_SQR_MASK(num)		(ADC_SQR_SHIFT(num, 0b11111))
// L(together with ADC_SQR(12..15)), determins num of regular conversion channels enabled, 0000 means 1 channel ..etc
#define ADC_SQR_L_SHIFT(num)		(((num) - 1) << 20)
#define ADC_SQR_L_MASK			(ADC_SQR_L_SHIFT(0b10000))   // means 0b1111	


#define ADC_JSQR			(0x38)

#define ADC_JDR(x)			(0x3c + ((x) << 2))  // x=0..3 (reference manual 1..4) result of (x+1)th inject conversion


#define ADC_DR				(0x4c)		// result of regular conversion
#define ADC_DR_DATA_MASK		(0xffff)	
#define ADC_DR_ADC2DATA_MASK		(ADC_DR_DATA_MASK << 16)  // result of ADC2 in dual mode

#define V_REFINT			(3.3l)
#define V_25				(1.43l)
#define V_PERC				(0.0043l)
								  



enum adc_periph {adc1 = ADC1_BASE, adc2 = ADC2_BASE, adc3 = ADC3_BASE};
enum adc_ch {adc_ch0 = 0U, adc_ch1, adc_ch2, adc_ch3, adc_ch4, adc_ch5, adc_ch6, adc_ch7, adc_ch8, adc_ch9, adc_ch10, adc_ch11, adc_ch12, adc_ch13, adc_ch14, adc_ch15, adc_ch16, adc_ch17};
enum adc_sample {c1p5 = 0U, c7p5, c13p5, c28p5, c41p5, c55p5, c71p5, c239p5};
enum adc_mode {single, continous, scan, continous_scan, discontinous};

//enum adc_mode {};


struct awdg_cfg {
	bool enable_regular;
	bool enable_inject;
	bool is_single;
	bool enable_interrupt;
	uint32_t low;
	uint32_t high;
	enum adc_ch ch;
};

struct adc_mode_cfg {
	enum adc_mode mode;
	uint32_t disc_num;
};

void init_adc();

uint32_t adc_get_rdata(enum adc_periph periph);

// can only call when adc1 is configured as continously converting V_SENSOR
double adc_get_temp();













#endif

