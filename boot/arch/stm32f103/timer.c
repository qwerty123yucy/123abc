#include <stm32f103.h>
#include <timer.h>
#include <stdint.h>
#include <stdbool.h>
#include <cpu.h>

// tim 2-5 is accepted
void timer_general_init(enum general_purpose_timer tim, uint16_t prescale, uint16_t arr){
	// set arr value 
	uint32_t old_arr = raw_readl((void *)(TIM_BASE(tim) + TIM_ARR));
	old_arr &= 0xffff0000;
	old_arr |= (uint32_t)arr;
	raw_writel(old_arr, (void *)(TIM_BASE(tim) + TIM_ARR));
	
	// set prescale value
	uint32_t old_prescale = raw_readl((void *)(TIM_BASE(tim) + TIM_PSC)); 
	old_prescale &= 0xffff0000;
	old_prescale |= (uint32_t)prescale;
	raw_writel((uint32_t)prescale, (void *)(TIM_BASE(tim) + TIM_PSC));

	//set cr1 urs
	uint32_t old_cr1 = raw_readl((void *)(TIM_BASE(tim) + TIM_CR1));
	old_cr1 |= (TIM_CR1_URS);
	raw_writel(old_cr1, (void *)(TIM_BASE(tim) + TIM_CR1));

	return;
}

void timer_general_on(enum general_purpose_timer tim, bool enable_interrupt){
	uint32_t old_cr1 = raw_readl((void *)(TIM_BASE(tim) + TIM_CR1));
	old_cr1 |= (TIM_CR1_CEN);
	raw_writel(old_cr1, (void *)(TIM_BASE(tim) + TIM_CR1));
	
	if(enable_interrupt){
		uint32_t old_dier = raw_readl((void *)(TIM_BASE(tim) + TIM_DIER));
		old_dier |= TIM_DIER_UIE;
		raw_writel(old_dier, (void *)(TIM_BASE(tim) + TIM_DIER));
	}

	return;
}

void timer_general_off(enum general_purpose_timer tim){
	// disable interrupt
	uint32_t old_dier = raw_readl((void *)(TIM_BASE(tim) + TIM_DIER));
	old_dier |= TIM_DIER_UIE;
	raw_writel(old_dier, (void *)(TIM_BASE(tim) + TIM_DIER));
	
	// disable counter
	uint32_t old_cr1 = raw_readl((void *)(TIM_BASE(tim) + TIM_CR1));
	old_cr1 &= ~(TIM_CR1_URS | TIM_CR1_CEN);
	raw_writel(old_cr1, (void *)(TIM_BASE(tim) + TIM_CR1));

	return;
}


uint32_t tim2_trigger(){
	static volatile bool tim2_is_inited = false;
	static volatile bool tim2_is_on = false;

	if(!tim2_is_inited){
		timer_general_init(tim2, (uint16_t)PRESCALE_VALUE, (uint16_t)ARR_VALUE);
	}

	if(!tim2_is_on){
		timer_general_on(tim2, true);
		tim2_is_on = true;
	}
	else{
		timer_general_off(tim2);
		tim2_is_on = false;
	}

	return tim2_is_on;
}
