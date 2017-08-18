#include <jinet/multiboot2.h>
#include <jinet/bootstruct.h>

void enterlm(void* mb2_info_tags)
{
	// mb2_info_tags: uint32_t total_size, reserved; tags!
	uint32_t frst = mb2_info_tags;
	uint32_t size = *(uint32_t*)mb2_info_tags;
	mb2_info_tags += 8;
	uint16_t *t = 0xb8000;
	// for(int i = 'a'; i<='z'; i++)
	// 	*t++ = 0x0400+i;
	// for(;;);
	while(mb2_info_tags < frst+size)
	{
		struct multiboot_tag* tag = mb2_info_tags;
		switch(tag->type)
		{
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* fb = tag;
				uint64_t addr = fb->framebuffer_addr;
				uint32_t* a = (uint32_t*)&addr;
				//*a++;
				uint32_t _a = *a;
				uint32_t *x = _a;
				for(int i = 0; i<100000; i++)
				{
					*x++ = 0xffffffff;
				}
				//int* r = 1/0;
				//*r = 1/0;
				break;
			}
			default :
				break;
		}
		mb2_info_tags += (tag->size + 7) & ~7;
	}
	for(;;);
}