#include <stm32f103.h>
#include <afio.h>
#include <gpio.h>
#include <cpu.h>

void set_exti_pin(enum gpio_num num, enum gpio_alphabet alphabet){
	// read old value of AFIO_EXTICR
	uint32_t cr = raw_readl((void *)(AFIO_BASE + AFIO_EXTICR_OF_LINE(num)));

	// clear bits of related area
	cr &= ~(AFIO_EXTICR_MASK_OF_LINE(num));

	// fill bits according to the gpio_alphabet
	cr |= AFIO_EXTICR_VAL(num, alphabet);

	// write new value to the register
	raw_writel(cr, (void *)(AFIO_BASE + AFIO_EXTICR_OF_LINE(num)));

	return;
}

void init_afio(){
	// select PC13 as exti13 trigger (button 2)
	set_exti_pin(L13, C);
	
	return;
}
