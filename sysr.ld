MEMORY
{
	sysr : org = 0xC0000000, len = 4k-1
}

OUTPUT_FORMAT("binary")
OUTPUT("sysr.bin")

SECTIONS
{
	.text :
	{
		sysr:*.o(.text);
	} > sysr
}