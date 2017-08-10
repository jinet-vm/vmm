void enterlm()
{
	short *d = 0xb8000;
	*d = 0x0442;
	for(;;);
}