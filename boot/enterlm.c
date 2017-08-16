#include <jinet/multiboot.h>
#include <jinet/bootstruct.h>
#include <jinet/vga.h>

void enterlm(struct multiboot_info* mt)
{
	if(!(mt->flags & MULTIBOOT_INFO_MODS))
	if(!(mt->flags & MULTIBOOT_INFO_VIDEO_INFO))
	if(!(mt->flags & MULTIBOOT_INFO_MEM_MAP)) // there's a lot of stuff we need
	{
		int *i = 1/0; // OH NO
		*i = 0;	// OH NO
		// causing reboot
		for(;;); // just to be DEAD sure
	}
	struct vbe_info* vbei = mt->framebuffer_addr;
	uint32_t* p = (uint32_t)vbei->framebuffer;
	for(int i = 0; i<vbei->width*vbei->height; i++)
		*p++ = 0xffffffff;
	for(;;);
}