/**
 * @file kernel.c
 * @brief Main kernel file.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include <kernel/tty.h>
#include <kernel/debug.h>
#include <kernel/stack.h>
#include <kernel/paging.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>
#include <kernel/vga.h>
#include <kernel/memtab.h>
#include <kernel/ints.h>
#include <kernel/keyboard.h>
#include <kernel/consts.h>
#include <kernel/printf.h>
//#include <msr.h>

#define p_entry(addr, f) (addr << 12) | f

/*
extern void sys_enter();
extern void sys_write(char* src);

void* PD_a;

extern void msr_get(uint32_t num, uint32_t* low, uint32_t *high);
extern void msr_set(uint32_t num, uint32_t low, uint32_t high);
*/

// Happy the man, and happy he alone
// He who can call today his own,
// He who, secure within, can say:
// Tomorrow do thy worst, for I have lived today

// \xC9 - ╔ \xCB - ╦ \xCD - ═ \xBB - ╗
//
// \xBA - ║ \xCC - ╠ \xCE - ╬ \xB9 - ╣
//
// \xC8 = ╚ \xBC - ╝ \xCA - ╩

size_t p_init();

void kernel_start()
{
	init_PD();
	map_page(0xB8000,0xB8000,pg_P | pg_U);
	double D = 0.042;
	vga_init();
	tty_init();
	printf("demos kernel loaded at 0x%x\n(phys = 0x%x)\n",KERNEL_VMA_ADDR,get_paddr(KERNEL_VMA_ADDR));
	idt_init();
	ints_install();
	idt_flush();
	mem_setup();
	irq_install_handler(1, keyboard_handler);
	ints_sti();
}