#ifndef	__CPU_H
#define __CPU_H

#include <bits.h>

#define CONTROL_RESET		(0x0)
#define XPSR_RESET		(BIT(24))
#define APSR_MASK		(0x1f << 27)
#define PRIMASK_RESET		(0x0)
#define BASEPRI_RESET		(0x0)
#define FAULTMASK_RESET		(0x0)

// if we need to write certain bit in SCB_ICSR, we should keep other bits unset in the 32 bit value being written to this register
#define SCB_ICSR		(0x04)
#define SCB_ICSR_NMIPENDSET	(1U << 31)
#define SCB_ICSR_PENDSVSET	(1U << 28)
#define SCB_ICSR_PENDSVCLR	(1U << 27)
#define SCB_ICSR_PENDSTSET	(1U << 26)
#define SCB_ICSR_PENDSTCLR	(1U << 25)
#define SCB_ICSR_ISRPENDING	(1U << 22)
#define SCB_ICSR_VECTPENDING_SHIFT(x)		((x) << 12)
#define SCB_ICSR_VECTPENDING_MASK		SCB_ICSR_VECTPENDING_SHIFT(0x3ff)

#define SCB_ICSR_RETOBASE			(1U << 11)
#define SCB_ICSR_VECTACTIVE_SHIFT(x)		((x) << 0)
#define SCB_ICSR_VECTACTIVE_MASK		SCB_ICSR_VECTACTIVE_SHIFT(0x1ff)

// all system handler priority have 8 bit space, SCB_SHPR can be accessed by 'byte'
#define SCB_SHPR				(0x18)
#define SCB_SHPR_PENDSV				(SCB_SHPR + 10)
#define SCB_SHPR_SYSTICK			(SCB_SHPR + 11)

#define PENDSV_PRIORITY				(0xff)
#define SYSTICK_PRIORITY			(0x00)


#ifndef	__ASSEMBLY__

#include <stdint.h>
static inline void raw_writel(uint32_t val, void *addr)
{
        __asm__ volatile ("str %r0, [%1]\n" : : "r" (val), "r" (addr));
}

static inline uint32_t raw_readl(void *addr)
{
        uint32_t val;

        __asm__ volatile ("ldr %r0, [%1]\n" : "=r" (val) : "r" (addr));
        return val;
}


#endif

#endif
