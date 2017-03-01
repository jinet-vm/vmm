MEMORY
{
	bkernel : org = 0x8000, len = 4k
}

OUTPUT_FORMAT(binary)
OUTPUT("boot.img")
ENTRY(start)

SECTIONS
{
	.boot : {
		*_kernel.o(.data);
	} > bootloader
}
