MEMORY
{
	basic_kernel : org = KERNEL_VMA_ADDR, len = KERNEL_SIZE /* before that, since 1MB - paging + gdt */
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

	.rodata :
	{
		:*(.rodata);
	} > basic_kernel

	.fill :
	{
		FILL(0x2a); /* = 42 */
		. = ORIGIN(basic_kernel)+LENGTH(basic_kernel)-1;
		BYTE(0x90);
	} > basic_kernel
}