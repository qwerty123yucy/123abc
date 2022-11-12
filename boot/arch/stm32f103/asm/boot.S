#include <cpu.h>
#include <linkage.h>
#include <stm32f103.h>
	.thumb
	.text
	.syntax unified
ASM_FUNC(_loop)
        wfe
        b       _loop

	.global _start 
_start:
	bl	init_bss
	bl	init_data
	bl	init_systick_priority
	bl	init_pendSV_priority
	
	bl	rcc_init
	bl	init_rtc
	bl	init_gpio
	bl	init_afio
	bl	init_usart
	bl	init_exti
	bl	init_nvic
	bl	init_adc
	bl 	init_dma
	bl	init_systick
	bl	init_task
	b	first_schedule

	
	// call schedule to choose a runnable tcb and set g_current_tcb
first_schedule:
	// change current sp to psp
	mrs	r0, msp
	msr	psp, r0
	mov	r0, #2
	msr	control, r0
	isb
	// just raise a pendSV then things done
	bl	pendSV_raise
	b	_loop




ASM_FUNC(init_pendSV_priority)
	push	{r0, r1}
	ldr	r0, =(SCB_BASE + SCB_SHPR_PENDSV)
	mov	r1, #PENDSV_PRIORITY
	strb	r1, [r0, #0]
	pop	{r0, r1}
	bx	lr

ASM_FUNC(init_systick_priority)
        push    {r0, r1}
        ldr     r0, =(SCB_BASE + SCB_SHPR_SYSTICK)
        mov     r1, #SYSTICK_PRIORITY
        strb    r1, [r0, #0]
        pop     {r0, r1}
        bx      lr



ASM_FUNC(pendSV_raise)
	push	{r0, r1}
	ldr	r0, =(SCB_BASE + SCB_ICSR)
	ldr	r1, =SCB_ICSR_PENDSVSET
	str	r1, [r0, #0]
	pop	{r0, r1}
	bx	lr

ASM_FUNC(pendSV_clear_flag)
        push    {r0, r1}
        ldr     r0, =(SCB_BASE + SCB_ICSR)
        ldr     r1, =SCB_ICSR_PENDSVCLR
        str     r1, [r0, #0]
        pop     {r0, r1}
        bx      lr

ASM_FUNC(systick_raise)
        push    {r0, r1}
        ldr     r0, =(SCB_BASE + SCB_ICSR)
        ldr     r1, =SCB_ICSR_PENDSTSET
        str     r1, [r0, #0]
        pop     {r0, r1}
        bx      lr

ASM_FUNC(systick_clear_flag)
        push    {r0, r1}
        ldr     r0, =(SCB_BASE + SCB_ICSR)
        ldr     r1, =SCB_ICSR_PENDSTCLR
        str     r1, [r0, #0]
        pop     {r0, r1}
        bx      lr





init_data:
	ldr	r0, =data__start
	ldr	r1, =data__end
	ldr	r2, =_sidata
init_data_loop:
	cmp	r0, r1
	bge	init_data_exit
	ldr	r3, [r2, #0]
	str	r3, [r0, #0]
	add 	r0, r0, #4
	add	r2, r2, #4
	b	init_data_loop
init_data_exit:
	bx	lr



init_bss:
	ldr	r0, =bss__start
	ldr	r1, =bss__end
	eor	r2, r2, r2
init_bss_loop:
	cmp	r0, r1
	bge	init_bss_exit
	str	r2, [r0]
	add	r0, r0, #4
	b	init_bss_loop
init_bss_exit:
	bx	lr
	
	
