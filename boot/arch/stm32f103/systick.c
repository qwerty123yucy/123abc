#include <systick.h>
#include <stm32f103.h>
#include <cpu.h>
#include <stdint.h>





void systick_enable(){
	uint32_t ctrl = raw_readl((void *)(STK_BASE + STK_CTRL));
	ctrl |= STK_CTRL_ENABLE;
	raw_writel(ctrl, (void *)(STK_BASE + STK_CTRL));
	return;
}

void systick_disable(){
	uint32_t ctrl = raw_readl((void *)(STK_BASE + STK_CTRL));
        ctrl &= ~STK_CTRL_ENABLE;
        raw_writel(ctrl, (void *)(STK_BASE + STK_CTRL));
	return;
}

void systick_set_clk_src(enum systick_clk_src src){
	uint32_t ctrl = raw_readl((void *)(STK_BASE + STK_CTRL));
	if(src == ahbdiv8){
		ctrl &= ~STK_CTRL_CLKSOURCE;	
	}
	else{
		ctrl |= STK_CTRL_CLKSOURCE;
	}
	raw_writel(ctrl, (void *)(STK_BASE + STK_CTRL));
	return;
}

void systick_set_reload(uint32_t reload){
	if(reload & ~STK_LOAD_RELOAD_MASK){
		reload = STK_LOAD_RELOAD_MAX;
	}	
	raw_writel(reload, (void *)(STK_BASE + STK_LOAD));
	return;
}

void systick_enable_interrupt(){
	uint32_t ctrl = raw_readl((void *)(STK_BASE + STK_CTRL));
	ctrl |= STK_CTRL_TICKINT;
	raw_writel(ctrl, (void *)(STK_BASE + STK_CTRL));
	return;
}

void systick_disable_interrupt(){
	uint32_t ctrl = raw_readl((void *)(STK_BASE + STK_CTRL));
	ctrl &= ~STK_CTRL_TICKINT;
	raw_writel(ctrl, (void *)(STK_BASE + STK_CTRL));
	return;
}

// reset counting
void systick_clear(){
	uint32_t val = raw_readl((void *)(STK_BASE + STK_VAL));
	val &= ~STK_VAL_CURRENT_MASK;
	raw_writel(val, (void *)(STK_BASE + STK_VAL));
	return;
}

void init_systick(){
	systick_disable();
	systick_disable_interrupt();
	systick_clear();
	systick_set_clk_src(ahbdiv8);
	systick_set_reload(SCHED_RELOAD);
	systick_enable();
	
	return;
}
