MEMORY
{
	bootloader : org = 0x7c00, len = BOOTLOADER_SIZE
}

OUTPUT_FORMAT("binary")
OUTPUT("boot.img")
ENTRY(start)

SECTIONS
{
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