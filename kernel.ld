MEMORY
{
	basic_kernel : org = 0x100000, len = 16K-1
	fill : org = 0x103FFF, len = 1
}

OUTPUT_FORMAT("binary")
OUTPUT("kernel.img")
ENTRY(start)

SECTIONS
{
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
	} > fill
}