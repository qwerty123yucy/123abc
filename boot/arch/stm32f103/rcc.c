#include <rcc.h>
#include <flash_acr.h>

// adopted from repository: EdwardLu2018/STM32-From-Scratch 


void rcc_init(void) {
    rcc_t *rcc = (rcc_t *)(RCC_BASE);
    flash_t *flash = (flash_t *)(FLASH_ACR_BASE);

    // switch to 72Mhz clock //
    flash->acr |= LATENCY_2;
    rcc->cfgr |= APB1_DIV2;

    rcc->cr |= HSEON;
    while(!(rcc->cr & HSERDY));

    rcc->cfgr |= (PLLHSE|PLL_9);
    rcc->cr |= PLLON;
    while(!(rcc->cr & PLLRDY));

    rcc->cfgr |= SWPLL;
    while(!(rcc->cfgr & SWSPLL));

    rcc->apbe1 |= (TIM2_EN); // enable timers
    rcc->apbe2 |= (GPIOA_EN|GPIOB_EN|GPIOC_EN); // enable all GPIO pins
    rcc->apbe2 |= USART1_EN;
    rcc->apbe2 |= AFIO_EN;
    rcc->apbe1 |= PWR_EN | BKP_EN;
}


// must set bkp registers as writable before calling enable_rtc
void enable_rtc(){
	rcc_t *rcc = (rcc_t *)(RCC_BASE);
	rcc->bdcr &= ~RTCSEL_SHIFT(3);
	rcc->bdcr |= RTCSEL_SHIFT(1);
	rcc->bdcr |= LSEON;
	rcc->bdcr |= RTC_EN;
}
