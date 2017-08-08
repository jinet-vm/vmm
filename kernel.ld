/*
	this is a 64-bit kernel (link script)
	the kernel is loaded as a module-initrd
	section .boot contains structure about
	where 64-bit kernel entrypoint is
*/

OUTPUT_FORMAT("binary")
OUTPUT("kernel.img")

SECTIONS
{
	. = KERNEL_VMA_ADDR;

	.boot :
	{
		*(.boot);
	}

	.text :
	{
		:*(.text)
	}

	.data :
	{
		:*(.data);
	}

	.bss :
	{
		:*(.bss);
	}

	.rodata :
	{
		:*(.rodata);
	}
}