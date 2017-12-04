// structure placed in the very beginning of Long Mode kernel binary
// telling 32-bit trampoline where to load the kernel
// 'Hey, ma', I think I have invented an executable format!'
// there are fields defined by the trampoline kernel
// and fields defined by the LM-kernel

#ifndef BOOTSTRUCT_H
#define BOOTSTRUCT_H

#define BTSTR_LM_MAGIC 0x0DEAD5AD // dead sad
#define BTSTR_TR_MAGIC 0xBEEFCAFE // beef cafe

// copies MULTIBOOT_FRAMEBUFFER_TYPE_* from <jinet/multiboot2.h>
#define BTSTP_VDTP_INDEXED 0
#define BTSTR_VDTP_RGB 1
#define BTSTR_VDTP_TEXT 2

// lm_* - Long Mode binary filled fields
// tr_* - Trampoline binary filled fields



/**
 * @brief      Structure with boot information that is neccessary for the hypervisor to start.
 * 
 * Boot process of the project is a bit complex. The hypervisor is split into two files: `enterlm.bin` and `kernel.img`. `kernel.img` is the hypervisor itself, it is loaded into bootloader (GRUB at the moment) as a ramdisk (grub2 module in terms of GRUB2). `enterlm.bin` is run after GRUB init. It initialises paging, interprets GRUB boot info (from multiboot) and jumps into the Jinet VMM after that.
 * 
 * In order to transmit boot data (e.g. about video mode or debug console) a bootstruct structure is placed at the beginning of `kernel.img`. Magic values #BTSTR_LM_MAGIC and #BTSTR_TR_MAGIC are used to maintain consistency.
 */
struct bootstruct
{
	uint32_t	lm_magic;
	uint32_t	tr_magic;
	uint64_t	lm_load_addr; // vma
	uint64_t	lm_entry_addr; // vma
	uint64_t	tr_pgtb_start; // paddr, a 1MB region -- we'll use it until buddy allocator is actually working
	uint64_t	tr_pgtb_cur; // paddr, current value
	uint64_t 	tr_video_type; // BTSTR_VDTP_*
	uint64_t	tr_vd_framebuffer;
	uint64_t	tr_vd_width;
	uint64_t	tr_vd_height;
	uint64_t	tr_vd_depth; // 0 if text-mode
	uint64_t	lm_mmap_addr; // it must be copied there by tr_kernel
	uint64_t	tr_mmap_len; // number of mmap entries
} __attribute__((packed));


#endif