MEMORY
{
	bootloader : org = ENTERLM_ADDR, len = ENTERLM_SIZE
}

OUTPUT_FORMAT("binary")
OUTPUT("enterlm.img")
ENTRY(start)

SECTIONS
{
 	. = 0;
	.boot : {
		FILL(0);
		*(.text);
	} > bootloader
}