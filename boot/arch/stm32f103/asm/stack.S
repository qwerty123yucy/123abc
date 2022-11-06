	.section .stack
	.align 4

	.global stack_bottom
stack_limit:
	.space STACK_SIZE

	.global stack_top
stack_top:
