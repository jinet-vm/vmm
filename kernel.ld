/* >>> 16K <<< */

MEMORY
{
	basic_kernel : org = 0x502000, len = 16K-1 /* before that, since 1MB - paging + gdt */
	fill : org = 0x504FFF, len = 1
}

OUTPUT_FORMAT("binary")
OUTPUT("kernel.img")
ENTRY(start)

SECTIONS
{
	KERNEL_VMA_START = .;
	KERNEL_SIZE = 16K;
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