#include <stm32f103.h>
#include <cpu.h>
#include <nvic.h>
#include <stdint.h>
#include <stdbool.h>

// implemented nothing expect for blind enabling most interrupts



void init_nvic(){
	raw_writel(0xffffffff, (void *)(NVIC_BASE + NVIC_ISER(0)));
	raw_writel(0xffffffff, (void *)(NVIC_BASE + NVIC_ISER(1)));

	return;
}
