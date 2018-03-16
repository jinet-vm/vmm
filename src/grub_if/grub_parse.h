#ifndef GRUB_PARSE_H
#define GRUB_PARSE_H

#include <jinet/multiboot2.h>

struct multiboot_tag* gp_next_tag(struct multiboot_tag* mt);
void gp_init(void* mbi);

#endif