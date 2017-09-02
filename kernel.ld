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
	KERNEL_START = .;

	.boot ALIGN(0x1000) :
	{
		*(.boot);
	}

	.text ALIGN(0x1000) :
	{
		:*(.text)
	}

	.data ALIGN(0x1000) :
	{
		:*(.data);
	}

	.bss ALIGN(0x1000) :
	{
		:*(.bss);
	}

	.rodata ALIGN(0x1000) :
	{
		:*(.rodata);
	}

	mmap_bootstruct = ALIGN(0x1000);
	pgst_bootstruct = . + 0x2000;
	KERNEL_END = .;
}