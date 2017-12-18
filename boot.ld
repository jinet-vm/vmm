OUTPUT_FORMAT("elf32-i386")
OUTPUT("boot/boot.elf")
ENTRY("trump");

SECTIONS
{
	. = 0x100000;

	.multiboot ALIGN (0x1000) :
	{
		*(.multiboot);
	}

	.text ALIGN (0x1000) :
	{
		:*(.text)
	}

	.data ALIGN (0x1000) :
	{
		:*(.data);
	}

	.bss ALIGN (0x1000) :
	{
		:*(.bss);
	}

	.rodata ALIGN (0x1000) :
	{
		:*(.rodata);
	}
}