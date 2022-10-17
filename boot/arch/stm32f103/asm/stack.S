	.section .stack
	.align 4

	.global stack_bottom
stack_bottom:
	.space STACK_SIZE

	.global stack_top
stack_top:
