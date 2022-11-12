#ifndef	__STM32F103_H
#define	__STM32F103_H


#define PERIPH_BASE		0x40000000U
#define AHBPERIPH_BASE		(PERIPH_BASE + 0x20000)
#define APB1PERIPH_BASE		PERIPH_BASE
#define APB2PERIPH_BASE		(PERIPH_BASE + 0x10000)

// usart base addrs
#define	USART1_BASE		0x40013800U	//on APB2
#define USART2_BASE		0x40004400U	//on APB1
#define USART3_BASE		0x40004800U	//on APB1

// system clock frequency
#define PCLK1			36000000U	//APB1 bus default frequency
#define PCLK2			72000000U	//APB2 bus default frequency

#define FLASH_ACR_BASE  	0x40022000U

#define RCC_BASE        	0x40021000U	

#define GPIOA_BASE      	0x40010800U
#define GPIOB_BASE      	0x40010C00U
#define GPIOC_BASE      	0x40011000U

#define NVIC_BASE       	0xE000E100U
#define STK_BASE		0xE000E010U
#define SCB_BASE		0xE000ED00U
			

#define AFIO_BASE		(APB2PERIPH_BASE + 0x0000)

#define EXTI_BASE		(APB2PERIPH_BASE + 0x0400)

//rtc
#define RTC_BASE		(APB1PERIPH_BASE + 0x2800)

#define PWR_BASE		(APB1PERIPH_BASE + 0x7000)

//timer
#define TIM_BASE(x)		(APB1PERIPH_BASE + ((0x400) * ((x) - 2)))  // x from 2 to 5

//bkp
#define BKP_BASE		(APB1PERIPH_BASE + 0x6C00)

//adc
#define ADC1_BASE		(APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE		(APB2PERIPH_BASE + 0x2800)
#define ADC3_BASE		(APB2PERIPH_BASE + 0x3C00)

//dma
#define DMA1_BASE		(AHBPERIPH_BASE + 0x0000)
#define DMA2_BASE		(AHBPERIPH_BASE + 0x0400)

//spi
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)


#endif
