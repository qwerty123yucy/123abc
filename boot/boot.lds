ENTRY(_start)
MEMORY
{
	rom : ORIGIN = 0x08000000, LENGTH = 0x80000
	ram : ORIGIN = 0x20000000, LENGTH = 0x10000

}

SECTIONS
{
	. = 0x08000000;
	.boot :
	{
		*(.vectors*)
		*(.text*)
		*(.rodata*)
	} > rom

	. = 0x20000000;

	_sidata = LOADADDR(.data);
	.data :
	{
		data__start = .;
		*(.data*)
		. = ALIGN(4);
		data__end = .;

	} > ram AT > rom
	
	.bss ALIGN(4):
	{
		bss__start = .;
		*(.bss* COMMON)
		. = ALIGN(4);
		bss__end = .;

	} > ram

	.stack ALIGN(4):
	{
		*(.stack*)
		stack__end = .;
	} > ram
}
