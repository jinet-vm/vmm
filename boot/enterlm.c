#include <jinet/multiboot.h>

void enterlm(struct multiboot_info* mt)
{
	char *name = mt->boot_loader_name;
	short *d = 0xb8000;
	while(*name != 0)
	{
		*d++ = *name | 0x0400;
	}
	for(;;);
}