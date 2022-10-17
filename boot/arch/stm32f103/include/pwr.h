#ifndef	__PWR_H
#define	__PWR_H

#include <stdbool.h>

#define PWR_CR		(0x00)
#define PWR_CR_DBP	(1U << 8)

#define PWR_CSR		(0x04)

void bkp_write_protect(bool prot);

#endif
