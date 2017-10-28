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

	initGDTR();
	gdt_set_code(1); // 0x08
	gdt_set_data(2); // 0x10
	gdt_set_tss(3,104,0xffff800000000000); // host tss - 0x18
	gdt_set_tss(5,104,0xffff800000000100); // vm0 tss - 0x20
	gdt_set_tss(7,104,0xffff800000000200); // vm1 tss - 0x28
	gdt_flush();
	asm("xchg %bx, %bx");
	cs_set(0x08);
	es_set(0x10);
	ds_set(0x10);
	fs_set(0x10);
	gs_set(0x10);
	ss_set(0x10);
	tr_set(SEG(3));
	physmm_init((struct multiboot_mmap_entry*)bs.lm_mmap_addr, bs.tr_mmap_len);
	mprint("phys memmng initialized");
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
	pg_map_reg(VMA_PHYS_LOW, 0, 0x100000000);
	acpi_add_driver("APIC", madt_probe);
	acpi_add_driver("MCFG", mcfg_probe);
	acpi_add_driver("DBGP", dbgp_probe);
	acpi_probe();
	mprint("acpi");
	if(!dbgp_ok())
	{
		dell_serial.addr = dbgp_base_addr();
		//term_add(dell_serial);
	}
	mprint("tada");
	//for(;;);
	// cpci_init();
	// pcie_init();
	// pci_probe();
	// uint64_t num = mcfg_seg_group_count();
	// if(num != MCFG_INVALID)
	// 	mprint("mcfg number: %x", num);
	// term_add(dell_serial);
	//mprint("GDT flushed");
	//volatile int s = 1/0;
	//tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	//mprint("IDT initialized.");
	//tty_setcolor(VC_DEFAULT);
	// heap not needed yet
	// // MADT
	//detect_rsdt();
	//print_sdts();
	//uint32_t madtb = detect_madt();
	pic_enable();
	pic_disable();
	lapic_setup();
	mprint("ha!");
	ioapic_setup();
	for(uint8_t i = 0; i<=23; i++)
		ioapic_set_gate(i,32+i,0,0,0,0,0,0); // just to be on a safe side
	//irq_install_handler(1, keyboard_handler);
	asm("xchg %bx, %bx");
	memcpy(0x7000, &test_bin, 0x1000);
	asm("sti");
	pit_init();
	mprint("test");
	pit_sleep(500);
	mprint("test");
	//for(;;);
	ipi_send(0x7,5,0,0,0,0,1);
	pit_sleep(1);
	ipi_send(0x7,6,0,0,0,0,1);
	int i = 0;
	int *I = 0x7800, *m = 0x7880;
	while(1)
	{
		if(*m = 1)
		{
			mprint("%d", *I);
			*m = 0;
		}
	}
	//ipi_send(0x7,6,0,0,0,0,1);
	//virt_init();
	for(;;);
}
