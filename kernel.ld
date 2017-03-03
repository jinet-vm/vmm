MEMORY
{
	bootloader : org = 0x7c00, len = 1k
	basic_kernel : org = 0x8000, len = 8K-1
	sysenter : org = 0x9FFF, len = 1
}

OUTPUT_FORMAT("binary")
OUTPUT("final.img")
ENTRY(start)

SECTIONS
{
 	. = 0;
	.boot : {
		FILL(0);
		*(.text16);
		. = 510;
		SHORT(0xaa55);
		*(.text32);
	} > bootloader

	.text :
	{
		:*(.text)
	} > basic_kernel

	.data :
	{
		:*(.data);
	} > basic_kernel

	.bss :
	{
		:*(.bss);
	} > basic_kernel

	.fill :
	{
		BYTE(0x90);
	} > sysenter
}