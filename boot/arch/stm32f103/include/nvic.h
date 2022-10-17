#ifndef	__NVIC_H
#define	__NVIC_H

#include <stm32f103.h>

#define	NVIC_ISER(x)	(0x00 + ((x) << 2))

#define NVIC_ICER(x)	(0x80 + ((x) << 2))

#define NVIC_ISPR(x)	(0x100 + ((x) << 2))

#define NVIC_ICPR(x)	(0x180 + ((x) << 2))

#define NVIC_IABR(x)	(0x200 + ((x) << 2))

#define NVIC_IPR(x)	(0x300 + ((x) << 2)) //????

#define NVIC_STIR	(0xE00)





#endif
