#include <exti.h>
#include <timer.h>

	.syntax unified
	.section    .vectors
    .word       stack_top		// initial SP
    .word       _start + 1		// reset handler
    .word       _loop + 1		// NMI
    .word       _loop + 1		// hard falt
    .word       _loop + 1		// memory fault
    .word       _loop + 1		// bus fault
    .word       _loop + 1		// usage fault
    .space	0x10			// reserved
    .word	_loop + 1		// SVCall
    .word	_loop + 1		// debug monitor
    .space	0x04			// reserved
    .word	_loop + 1		// pend SV
    .word	_loop + 1		// systick
    .word	_loop + 1		// WWDG
    .word	_loop + 1		// PVD
    .word	_loop + 1		// tamper
    .word	_loop + 1		// RTC
    .word	_loop + 1		// flash
    .word	_loop + 1		// RCC
    .word	_exti0_handler + 1	// exti0
    .word	_loop + 1		// exti1
    .word	_loop + 1		// exti2
    .word	_loop + 1		// exti3
    .word	_loop + 1		// exti4
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_tim2_handler + 1		// tim2 interrupt 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_exti15_10_handler + 1	// exti 15_10 
    .word	_loop + 1		// 
    .word	_loop + 1		// 
    .word	_loop + 1		// 


	.text
_exti0_handler:
	// save context
	push	{r4-r11, lr}
	
	// clear the interrupt
	ldr	r0, =EXTI_BASE + EXTI_PR
	mov	r1, 1U
	str	r1, [r0]

	// handle the interrupt
	bl	tim2_trigger
	cmp 	r0, #0
	bne	_exti0_handler_exit
	bl	led_off	
	
_exti0_handler_exit:
	// restore context
	pop	{r4-r11, lr}
	
	// exit interrupt
	bx	lr
	
    
_exti15_10_handler:
	push    {r4-r11, lr}

        // clear the interrupt
        ldr     r0, =EXTI_BASE + EXTI_PR
        mov     r1, (1U << 13) 
        str     r1, [r0]

        // handle the interrupt
        bl      led_trigger

        // restore context
        pop     {r4-r11, lr}

        // exit interrupt
        bx      lr

_tim2_handler:
	push    {r4-r11, lr}

        // clear the interrupt
	ldr	r0, =TIM_BASE(2) + TIM_SR
	ldr	r1, [r0]
	and	r1, r1, ~(1U << 0)
	str	r1, [r0]

        // handle the interrupt
        bl      led_trigger

        // restore context
        pop     {r4-r11, lr}

        // exit interrupt
        bx      lr


	.data
exti0_str:
	.asciz "external interrupt caught!\n"
