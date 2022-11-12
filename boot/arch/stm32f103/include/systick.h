#ifndef	__SYSTICK_H
#define __SYSTICK_H

#include <stdint.h>

#define STK_CTRL			(0x00)
#define STK_CTRL_COUNTFLAG		(1U << 16)
#define STK_CTRL_CLKSOURCE		(1U << 2)	// 0: AHB/8, 1: AHB
#define STK_CTRL_TICKINT		(1U << 1)	// 1: cause a exception when counting down to 0
#define STK_CTRL_ENABLE			(1U << 0)	// enable systick

#define STK_LOAD			(0x04)		// if we need an interrupt each n clock, set RELOAD to n
#define STK_LOAD_RELOAD_SHIFT(x)	((x) << 0)
#define STK_LOAD_RELOAD_MASK		(STK_LOAD_RELOAD_SHIFT(0xffffff))  // 24 bits
#define STK_LOAD_RELOAD_MAX		(STK_LOAD_RELOAD_MASK)

#define STK_VAL				(0x08)		// records current counter value. write 0 to reset counter(COUNTERFLAG will be cleared)
#define STK_VAL_CURRENT_SHIFT(x)        ((x) << 0)
#define STK_VAL_CURRENT_MASK            (STK_VAL_CURRENT_SHIFT(0xffffff))  // 24 bits


#define STK_CALIB			(0x0c)
#define STK_CALIB_NOREF			(1U << 31)	// ???
#define STK_CALIB_SKEW			(1U << 30)	// ???
#define STK_CALIB_TENMS_SHIFT(x)	((x) << 0)
#define STK_CALIB_TENMS_MASK		(STK_CALIB_TENMS_SHIFT(0xffffff))  // 24 bits	

#define SCHED_RELOAD			(900000U)		// 1ms

enum systick_clk_src {ahbdiv8 = 0, ahb};

void systick_enable();
void systick_disable();
void systick_set_clk_src(enum systick_clk_src src);
void systick_set_reload(uint32_t reload);
void systick_enable_interrupt();
void systick_disable_interrupt();
void systick_clear();
void init_systick();

#endif
