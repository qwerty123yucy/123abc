#ifndef _RCC_H_
#define _RCC_H_

// adopted from repository: EdwardLu2018/STM32-From-Scratch 

#include <stm32f103.h>
#include <stdint.h>
#include <bits.h>
#include <adc.h>
#include <spi.h>


#define APB1_DIV2	(4<<8)  // AHB1 = HCLK/2
#define HSION       (1<<0)  // enable HSI
#define HSERDY      (1<<17) // external high-speed clock ready flag
#define HSEON       (1<<16) // enable HSE
#define PLLHSE      (1<<16) // HSE oscillator clock selected as PLL input clock
#define PLLON       (1<<24) // enable PLL
#define PLLRDY      (1<<25) // PLL ready
#define PLL_9	    (7<<18) // PLL x 9
#define SWSPLL      (2<<2)  // PLL used as system clock
#define SWPLL       (2<<0)  // set PLL as system clock

#define GPIOA_EN    (1<<2) // apbe2
#define GPIOB_EN    (1<<3) // apbe2
#define GPIOC_EN    (1<<4) // apbe2

#define TIM2_EN     (1<<0) // apbe1
#define TIM3_EN     (1<<1) // apbe1
#define TIM4_EN     (1<<2) // apbe1
#define TIM5_EN     (1<<3) // apbe1
#define TIM6_EN     (1<<4) // apbe1
#define TIM7_EN     (1<<5) // apbe1

// spi
#define SPI1_EN	    (1<<12) // apbe2

#define USART1_EN   (1<<14) // apbe2
#define USART2_EN   (1<<17) // apbe1
#define USART3_EN   (1<<18) // apbe1

#define AFIO_EN	    (1<<0)  // apbe2
#define ADC1_EN	    (1<<9)  // apbe2
#define ADC_PRESCALE_SHIFT(x)	((x) << 14) // in RCC_CFGR

// dma
#define DMA1_EN	    (1<<0)  // ahbe

#define PWR_EN	    (1<<28) //apbe1	
#define BKP_EN	    (1<<27) //apbe1

// about RTC
#define RTC_EN	    	(1<<15) //bdcr
#define RTCSEL_SHIFT(x)	(U(x) << 8)   //bdcr 01 to choose LSE 
#define LSEON		(1<<0)	//bdcr

// Reset and clock control (page 99 Reference Manual) //
typedef struct {
    uint32_t volatile cr;       // 0x0 - control register
    uint32_t volatile cfgr;     // 0x4 - configuration register
    uint32_t volatile cir;      // 0x8 - interrupt register
    uint32_t volatile apbr2;    // 0xC - APB2 peripheral reset register
    uint32_t volatile apbr1;    // 0x10 - APB1 peripheral reset register
    uint32_t volatile ahbe;     // 0x14 - AHB peripheral clock enable register
    uint32_t volatile apbe2;    // 0x18 - APB2 peripheral enable register
    uint32_t volatile apbe1;    // 0x1C - APB1 peripheral enable register
    uint32_t volatile bdcr;     // 0x20 - backup domain control register
    uint32_t volatile csr;      // 0x24 - control/status register
} rcc_t;

enum adc_prescale {adc_div2 = 0U, adc_div4, adc_div6, adc_div8};

void rcc_init(void);
void rcc_enable_rtc();
void rcc_enable_adc(enum adc_periph periph, enum adc_prescale pre);
void rcc_enable_spi(enum spi_periph periph);
#endif /* _RCC_H_ */
