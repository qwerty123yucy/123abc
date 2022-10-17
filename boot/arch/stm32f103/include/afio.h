#ifndef	__AFIO_H
#define	__AFIO_H

#include <stm32f103.h>

#define	AFIO_EVCR	(0x00)	// used to set certain gpio pin among PA, PB etc. to act as event output pin

#define	AFIO_MAPR	(0x04)  // memory remmap (havent understood)


// the following EXTICRx are used to select certain gpio pin to act as exti trigger pin, each exti takes place of 4 bits (0000 means PA is selected ...)


#define	AFIO_EXTICR(x)	(0x08 + ((x) << 2))  //x in 0-3 (reference manual names 1-4)

#define AFIO_EXTICR_OF_LINE(num) 	(AFIO_EXTICR(((num) / 4)))
#define AFIO_EXTICR_MASK		(0xfU)
#define AFIO_EXTICR_SHIFT_OF_LINE(num)	(((num) % 4) << 2)
#define AFIO_EXTICR_MASK_OF_LINE(num)	(AFIO_EXTICR_MASK << AFIO_EXTICR_SHIFT_OF_LINE(num))
#define AFIO_EXTICR_VAL(num, val)	((val) << (AFIO_EXTICR_SHIFT_OF_LINE(num)))

#define AFIO_MAPR2			(0x1c)	//exti



void init_afio();

#endif
