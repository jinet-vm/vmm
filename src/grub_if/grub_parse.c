#include <jinet/multiboot2.h>
#include "grub_parse.h"


static void* mb2_init;
static uint32_t mb2_size;

void gp_init(void* mbi)
{
	mb2_init = mbi;
	mb2_size = *(uint32_t*)mbi;
}

/**
 * @brief      get next tag
 *
 * @param      mt    { parameter_description }
 *
 * @return     returns next tag or zero if out of range
 */
struct multiboot_tag* gp_next_tag(struct multiboot_tag* mt)
{
	void * t = mt;
	t += (mt->size + 7) & ~7;
	if(mt < mb2_init + mb2_size)
		return t;
	else
		return 0;
}
