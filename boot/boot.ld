OUTPUT_FORMAT("elf32-i386")
OUTPUT("boot.elf")
ENTRY("trump");

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