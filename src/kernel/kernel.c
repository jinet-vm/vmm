#include <stdint.h>
#include <stddef.h>
#include <jinet/debug.h>
#include <jinet/ints.h>
#include <jinet/idt.h>
#include <jinet/printf.h>
#include <jinet/keyboard.h>
#include <jinet/acpi.h>
#include <jinet/madt.h>
#include <jinet/heap.h>
#include <jinet/ipi.h>
#include <jinet/consts.h>
#include <jinet/ioapic.h>
#include <jinet/vmx.h>
#include <jinet/gdt.h>
#include <jinet/font.h>
#include <jinet/term.h>
#include <jinet/module.h>
#include <jinet/pci.h>
#include <jinet/mcfg.h>
#include <jinet/bootstruct.h>
#include <jinet/multiboot2.h>
#include <jinet/paging.h>
#include <jinet/vmaddr.h>
#include <jinet/tss.h>
#include <jinet/dbgp.h>
#define p_entry(addr, f) (addr << 12) | f

#define title_lines 6

extern ints_sti(void);

void putc(void* none, char c)
{
	tty_putc(c);
}

void kernel_start();

extern void rt0();

struct bootstruct  __attribute__((section(".boot"))) bs =
{
	.lm_magic = BTSTR_LM_MAGIC,
	.lm_load_addr = KERNEL_VMA_ADDR,
	.lm_entry_addr = &rt0,
	.lm_mmap_addr = 0x100000000
};

struct term_dev vga =
{
	.name = "VGA_TERM",
	.type = TERM_VGA,
	.addr = 0xb8000,
	.init = term_vga_init,
	.putc = term_vga_putc
};

struct term_dev vbe =
{
	.name = "VBE_TERM",
	.type = TERM_VBE,
	.addr = 0,
	.init = term_vbe_init,
	.putc = term_vbe_putc
};

struct term_dev com_port =
{
	.name = "COM_PORT",
	.type = TERM_SERIAL_IO,
	.addr = 0x3F8,
	.init = term_serial_io_init,
	.putc = term_serial_io_putc
};

struct term_dev dell_serial =
{
	.name = "E5470_SERIAL",
	.type = TERM_SERIAL_MMIO,
	.addr = 0,
	.init = term_serial_mmio_init,
	.putc = term_serial_mmio_putc
};

MODULE("KERNEL");

asm("test_bin: .incbin \"bin/test.bin\"");
extern void* test_bin;

void kernel_start()
{
	// for(;;);
	// VGA
	term_init();
	term_add(com_port);

	idt_init();
	isr_install();
	irq_install();
	idt_flush();

	// setting up TSS; TODO: hardcoded
	physmm_init((struct multiboot_mmap_entry*)bs.lm_mmap_addr, bs.tr_mmap_len);
	pg_map_reg(VMA_KERNEL_TSS, physmm_alloc(1), 0x1000);
	pg_map_reg(VMA_IST1, physmm_alloc(4), 0x10000);
	struct tss_entry_64* kernelTSS = VMA_KERNEL_TSS;
	memset(kernelTSS, 0, 0x68);
	kernelTSS->ist[0] = VMA_IST1+0x8000; // 2 Kib above

	// GDT
	initGDTR();
	gdt_set_code(1); // 0x08
	gdt_set_data(2); // 0x10
	gdt_set_tss(3,104,VMA_KERNEL_TSS); // host tss - 0x18
	gdt_flush();
	asm("xchg %bx, %bx");
	cs_set(0x08);
	es_set(0x10);
	ds_set(0x10);
	fs_set(0x10);
	gs_set(0x10);
	ss_set(0x10);
	tr_set(SEG(3));

	asm ("xchg %bx, %bx");

	//int i = 1/0;

	mprint("phys memmng initialized");
	heap_init();
	if(bs.tr_video_type == BTSTR_VDTP_TEXT)
	{
		term_add(vga);
	}
	else
	{
		vbe.addr = bs.tr_vd_framebuffer;
		term_add(vbe);
	}
	mprint("demo");
	pg_map_reg(0, 0, 0x100000000);

	acpi_add_driver("APIC", madt_probe);
	acpi_add_driver("MCFG", mcfg_probe);
	acpi_add_driver("DBGP", dbgp_probe);
	acpi_probe();
	mprint("acpi");
	asm("xchg %bx, %bx");
	// for(;;)
	// {
	// 	mprint("a");
	// }
	if(!dbgp_ok()) // TODO: fix
	{
		//dell_serial.addr = dbgp_base_addr();
		//term_add(dell_serial);
	}
	pic_enable();
	pic_disable();
	lapic_setup();
	mprint("ha!");
	ioapic_setup();
	for(uint8_t i = 0; i<=23; i++)
		ioapic_set_gate(i,32+i,0,0,0,0,1,0); // just to be on a safe side
	ioapic_set_gate(1,33,0,0,0,0,0,0);
	irq_install_handler(1, keyboard_handler);
	asm("sti");
	for(;;);
	// asm("xchg %bx, %bx");

	// while(1)
	// {
	// 	printf("a");
	// }
	//ipi_send(0x7,6,0,0,0,0,1);
	//virt_init();
	// mprint("%llx",malloc(0x100));
	// mprint("%llx",malloc(0x100));
	// mprint("%llx",malloc(0x100));
	mprint("done");
	sched_init();
	for(int i = 100; i<150; i++)
		vbe_put_pixel(i,i,3);
	while(1);
	//asm("sti");
	int x = 0, y = 0, color = 0, t = 0;
	while(1)
	{	//printf("abc");
		t++;
		x++; 
		if(x > 100) y++, x=0;
		if(y > 100) x = 0, y = 0;
		if(x*y*t % 3 == 0) color = (color + 1) % 16;
		vbe_put_pixel(x,y,color);
		// if(p1)
		// {
		// 	mprint("task1: %d", s1);
		// 	s1 = 0;
		// 	p1 = 0;
		// }
		// if(p2)
		// {
		// 	mprint("task2: %d", s2);
		// 	s2 = 0;
		// 	p2 = 0;
		// }
	}

	for(;;);
}
