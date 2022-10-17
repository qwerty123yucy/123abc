#ifndef	__CPU_H
#define __CPU_H

#include <bits.h>

#define CONTROL_RESET	(0x0)
#define XPSR_RESET	( BIT(24) )
#define PRIMASK_RESET	(0x0)
#define BASEPRI_RESET	(0x0)
#define FAULTMASK_RESET	(0x0)


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
