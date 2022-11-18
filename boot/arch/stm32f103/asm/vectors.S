#include <exti.h>
#include <timer.h>
	.thumb
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
    .word	_pendSV_handler + 1	// pendSV
    .word	_systick_handler + 1	// systick
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
    .word	_tim2_handler + 1	// tim2 interrupt 
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
_systick_handler:
	push	{r4-r11, lr}
	bl	systick_disable_interrupt
	bl	systick_clear_flag
	bl	pendSV_raise

	pop	{r4-r11, lr}
	bx lr


	// context switch
_pendSV_handler:
	push	{lr}
	// r0-r3, r12, pc, psr, lr are under psp
	push	{r4-r11}
	bl	pendSV_clear_flag
	// first put g_current_tcb's addr into r0
	ldr	r0, =g_current_tcb
	ldr	r0, [r0, #0]
	// but if it is the first schedule, we dont need to save context
	cmp	r0, #0
	bne	save_context
	pop	{r4-r11}
	b	next_schedule

	// save all the context for g_current_tcb
	// if g_current_tcb is 'NULL' (it is the first time to schedule for a task),
	// then this part will be skipped
save_context:
	// save r4-r11 for g_current_tcb
	// get g_current_tcb's reglist addr
	bl	tcb_registers
	pop	{r4-r11}
	str	r4, [r0, #16]
	str	r5, [r0, #20]
	str	r6, [r0, #24]
	str	r7, [r0, #28]
	str	r8, [r0, #32]
	str	r9, [r0, #36]
	str	r10, [r0, #40]
	str	r11, [r0, #44]
	// save r0-r3 for g_current_tcb
	// first put psp value into r1 (r0-r3 are in psp)
	mrs	r1, psp
	ldr	r4, [r1, #0]	// r0
	ldr	r5, [r1, #4]	// r1
	ldr	r6, [r1, #8]	// r2
	ldr	r7, [r1, #12]	// r3
	str	r4, [r0, #0]	// save r0 to tcb
	str	r5, [r0, #4]	// save r1
	str	r6, [r0, #8]	// save r2
	str	r7, [r0, #12]	// save r3
	// save r12, lr, pc, xpsr for g_current_tcb
	// r12, lr, pc, xpsr are also in psp
	ldr	r4, [r1, #16]	// r12
	ldr	r5, [r1, #20]	// lr
	ldr	r6, [r1, #24]	// pc
	ldr	r7, [r1, #28]	// xpsr(apsr)
	str	r4, [r0, #48]	// save r12 to tcb
	str	r5, [r0, #56]	// save lr
	str	r6, [r0, #60]	// save pc
	str	r7, [r0, #64]	// save xpsr
	// finally we need to save psp for g_current tcb
	// but we should exclude the context saved in psp
	add	r1, r1, #32
	str	r1, [r0, #52]
	isb

	// finished context saving for g_current tcb
	// it's needed to tick once for g_current_tcb
	ldr	r0, =g_current_tcb
	ldr	r0, [r0, #0]
	bl	tcb_tick_once

	// now we should prepare for the next tcb
next_schedule:
	// increase the g_tick
	ldr	r0, =g_tick
	ldr	r1, [r0, #0]
	add	r1, r1, #1
	str	r1, [r0, #0]
	dmb
	bl	schedule
	cmp	r0, #0
	// schedule returns no tcb
	beq	next_schedule
	// else we got the next tcb to be resumed
	// just save it into g_current_tcb
	ldr	r1, =g_current_tcb
	str	r0, [r1, #0]
	dmb

	// now we resume to the new g_current_tcb
	// the method first recovers r4-r11,
	// and then puts r0-r3, lr, pc, xpsr into psp
	// after which we can resume to g_current_tcb with a 'bx lr'
tcb_resume:
	// first we should clear systick and enable its interrupt
	bl	systick_clear
	bl	systick_enable_interrupt
	// put the addr of g_current_tcb's register context into r0
	ldr	r0, =g_current_tcb
	ldr	r0, [r0, #0]
	bl	tcb_registers

	// recover r4-r11
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
	ldr     r6, [r0, #24]
	ldr     r7, [r0, #28]
	ldr     r8, [r0, #32]
	ldr     r9, [r0, #36]
	ldr     r10, [r0, #40]
	ldr     r11, [r0, #44]

	// we should change the psp to g_current_tcb's psp - 32
	// put psp into r1
	ldr	r1, [r0, #52]
	sub	r1, r1, #32
	msr	psp, r1
	// then put r0-r3 to psp
	ldr	r2, [r0, #0]
	str	r2, [r1, #0]
	ldr     r2, [r0, #4]
        str     r2, [r1, #4]
	ldr     r2, [r0, #8]
        str     r2, [r1, #8]
	ldr     r2, [r0, #12]
        str     r2, [r1, #12]
	// then put r12, lr, pc, xpsr into psp
	ldr	r2, [r0, #48]	// r12
	str	r2, [r1, #16]
	ldr	r2, [r0, #56]	// lr
	str	r2, [r1, #20]
	ldr	r2, [r0, #60]	// pc (need to add 1 to pc to maintain thumb state)
	ldr	r3, =0xfffffffe
	and	r2, r2, r3
	str	r2, [r1, #24]
	ldr	r2, [r0, #64]	// xpsr
	str	r2, [r1, #28]
	isb
	// now r0-r3, r12, lr, pc, xpsr are under psp, r4-r11 are recovered,
	// and psp is the tcb's psp - 32
	// recover magic number
	pop	{lr}
	// exit handler mode and turn to g_current_tcb
	bx	lr
	
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
