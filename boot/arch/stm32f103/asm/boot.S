#include <cpu.h>
#include <linkage.h>
	.text
	.global _start 
_start:
	bl	init_bss
	bl	init_data
	
	bl	rcc_init
	bl	init_rtc
	bl	init_gpio
	bl	init_afio
	bl	init_usart
	bl	init_exti
	bl	init_nvic
	bl	init_adc
	bl 	init_dma
	b	first_schedule

	
	// call schedule to choose a runnable tcb and set g_current_tcb
first_schedule:
	bl 	init_task
	bl	schedule
	cmp	r0, #0
	// no tasks or none of the tasks is runnable
	beq	_loop
	ldr	r1, =g_current_tcb
	str	r0, [r1, #0]
	b	tcb_resume
	


	// switch context to g_current_tcb
ASM_FUNC(tcb_resume)
	// put g_current_tcb's register list into r0
	ldr	r0, =g_current_tcb
	ldr	r0, [r0, #0]
	bl	tcb_registers
	
	// first recover r8 - r12
	ldr	r1, [r0, #32]
	mov	r8, r1
	ldr	r1, [r0, #36]
	mov	r9, r1
	ldr	r1, [r0, #40]
	mov	r10, r1
	ldr	r1, [r0, #44]
	mov	r11, r1
	ldr	r1, [r0, #48]
	mov	r12, r1
	ldr	r1, [r0, #52]
	// using psp as tasks' sp 
	msr	psp, r1
	ldr	r1, [r0, #56]
	mov 	lr, r1
	ldr	r1, [r0, #64]
	msr	apsr, r1
	// set psp as current sp
	mov	r1, #2
	msr	control, r1
	isb

	// put pc in stack in order to jump to thread with a pop
	ldr	r1, [r0, #60]
	push	{r1}
	
	ldr	r7, [r0, #28]
	ldr	r6, [r0, #24]
	ldr	r5, [r0, #20]
	ldr	r4, [r0, #16]
	ldr	r3, [r0, #12]
	ldr	r2, [r0, #8]
	ldr	r1, [r0, #4]
	ldr	r0, [r0, #0]
	// jump to g_current_tcb
	pop	{pc}





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
	
	
