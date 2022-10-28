#include <cpu.h>
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
	
	bl	main_loop
	b	_loop



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
	
	
