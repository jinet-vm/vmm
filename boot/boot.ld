OUTPUT_FORMAT("binary")
OUTPUT("boot.img")

SECTIONS
{
	. = 0x100000;

	.multiboot :
	{
		*(.multiboot);
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