#include <stm32f103.h>
#include <pwr.h>
#include <cpu.h>
#include <stdint.h>
#include <stdbool.h>

/* 
 * before modifying backup domain registers, call this to unlock writting. 
 * call this to lock writting after modifying.
 */
void bkp_write_protect(bool prot){
	uint32_t cr = raw_readl((void *)(PWR_BASE + PWR_CR));
	if(prot){
		// enable writting access to backup domain
		cr &= ~PWR_CR_DBP;
	}
	else{
		// disable writting access to backup domain
		cr |= PWR_CR_DBP;
	}
	raw_writel(cr, (void *)(PWR_BASE + PWR_CR));

	return;
}

