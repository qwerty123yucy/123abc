#ifndef __EXTI_H
#define __EXTI_H

#include <stm32f103.h>

// EXTI registers
#define EXTI_IMR	(0x00)	//set bit(x) to unmask exti(x) interrupt


#define EXTI_EMR	(0x04)	//set bit(x) to unmask exti(x) event

#define EXTI_RTSR	(0x08)  //set bit(x) to trigger exti(x) when rising
	
#define EXTI_FTSR	(0x0c)  //set bit(x) to trigger exti(x) when falling	

#define EXTI_SWIER	(0x10)  //software interrupt event register

#define EXTI_PR		(0x14)	//when exti(x) is triggered,  bit(x) is set; writing1 to bit(x) can clear bit(x) 



#ifndef	__ASSEMBLY__
#include <stdint.h>
enum exti_num {exti0 = 0, exti1, exti2, exti3, exti4, exti5, exti6, exti7, exti8, exti9, exti10, exti11, exti12, exti13};
enum exti_trigger_method {rising_trigger, falling_trigger, rising_falling};

void enable_exti_interrupt(enum exti_num idx, enum exti_trigger_method method);
void init_exti();
#endif



#endif
