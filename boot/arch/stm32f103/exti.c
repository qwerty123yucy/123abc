#include <stm32f103.h>
#include <exti.h>
#include <cpu.h>

void enable_exti_interrupt(enum exti_num idx, enum exti_trigger_method method){
	uint32_t imr, rtsr, ftsr;
	// mask exti(idx) before configuring
	imr = raw_readl((void *)(EXTI_BASE + EXTI_IMR));
	imr &= ~(0x1 << idx);
	raw_writel(imr, (void *)(EXTI_BASE + EXTI_IMR));
	
	
	// set trigger method for exti(idx)
	rtsr = raw_readl((void *)(EXTI_BASE + EXTI_RTSR));
	ftsr = raw_readl((void *)(EXTI_BASE + EXTI_FTSR));
	if(method == rising_trigger){
		rtsr |= (0x1 << idx);
		ftsr &= ~(0x1 << idx);
	}
	else if(method == falling_trigger){
		rtsr &= ~(0x1 << idx);
		ftsr |= (0x1 << idx);
	}
	else{
		rtsr |= (0x1 << idx);
		ftsr |= (0x1 << idx);
	}
	raw_writel(rtsr, (void *)(EXTI_BASE + EXTI_RTSR));
	raw_writel(ftsr, (void *)(EXTI_BASE + EXTI_FTSR));

	// enable exti(idx)
	imr |= (0x1 << idx);
	raw_writel(imr, (void *)(EXTI_BASE + EXTI_IMR));
	
	return;

}



void init_exti(){
	// button 1 interrupt
	enable_exti_interrupt(exti0, falling_trigger);
	// button 2 interrupt
	enable_exti_interrupt(exti13, falling_trigger);

	return;
}
