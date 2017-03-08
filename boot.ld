MEMORY
{
	bootloader : org = 0x7c00, len = 1k
}

OUTPUT_FORMAT("binary")
OUTPUT("boot.img")
ENTRY(start)

SECTIONS
{
	/* kernel_start = 0x00000000001009ec; */
 	. = 0;
	.boot : {
		FILL(0);
		*(.text16);
		*(.data16);
		. = 510;
		SHORT(0xaa55);
		*(.text32);
		. = LENGTH(bootloader)+ORIGIN(bootloader)-1;
		BYTE(0x2a);
	} > bootloader
}