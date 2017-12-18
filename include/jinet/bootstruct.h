/**
 * @file
 * @brief      Bootstruct definition
 */

#ifndef BOOTSTRUCT_H
#define BOOTSTRUCT_H

#define BTSTR_LM_MAGIC 0x0DEAD5AD // dead sad
#define BTSTR_TR_MAGIC 0xBEEFCAFE // beef cafe

// copies MULTIBOOT_FRAMEBUFFER_TYPE_* from <jinet/multiboot2.h>
#define BTSTR_VDTP_INDEXED 0
#define BTSTR_VDTP_RGB 1
#define BTSTR_VDTP_TEXT 2

// lm_* - Long Mode binary filled fields
// tr_* - Trampoline binary filled fields



/**
 * @brief      Structure with boot information that is neccessary for the
 *             hypervisor to start.
 *
 *             Boot process of the project is a bit complex. The hypervisor is
 *             split into two files: `enterlm.bin` and `kernel.img`.
 *             `kernel.img` is the hypervisor itself, it is loaded into
 *             bootloader (GRUB at the moment) as a ramdisk (grub2 module in
 *             terms of GRUB2). `enterlm.bin` is run after GRUB init. It
 *             initialises paging, interprets GRUB boot info (from multiboot)
 *             and jumps into the Jinet VMM after that.
 *
 *             In order to transmit boot data (e.g. about video mode or debug
 *             console) a bootstruct structure is placed at the beginning of
 *             `kernel.img`. Magic values #BTSTR_LM_MAGIC and #BTSTR_TR_MAGIC
 *             are used to maintain consistency.
 *
 *             'Hey, ma', I think I have invented an executable format!'
 *
 * @todo Add support for DBGP recognition in enterlm.bin - *that's* what debug
 * port is supposed to be.
 */
struct bootstruct
{
	/// @brief      Magic value written to at compile-time.
	uint32_t	lm_magic;
	/// @brief      Magic value written to by trampoline binary during run-time.
	uint32_t	tr_magic;
	/// @brief      Compile-time value with VMA of kernel binary start.
	uint64_t	lm_load_addr;
	/// @brief      Compile-time value with VMA of kernel_start.
	uint64_t	lm_entry_addr;
	/// @brief      Physical pointer to physical 1 MB region for first time allocation for tables.
	uint64_t	tr_pgtb_start;
	/// @brief      Phisycal pointer to current value in first-alloc buffer (see #tr_pgtb_start).
	uint64_t	tr_pgtb_cur; // paddr, current value
	/// @brief      Type of video output (either #BTSTR_VDTP_INDEXED, #BTSTR_VDTP_RGB or #BTSTR_VDTP_TEXT)
	uint64_t 	tr_video_type;
	/// @brief      Physical pointer to text-buffer or framebuffer of video output.
	uint64_t	tr_vd_framebuffer;
	/// @brief     	Number of columns in video.
	uint64_t	tr_vd_width;
	/// @brief     	Number of lines in video.
	uint64_t	tr_vd_height;
	/// @brief     	If we're given framebuffer, it is equal to BPP, otherwise 0.
	uint64_t	tr_vd_depth; // 0 if text-mode
	// @brief      VMA pointer to where memory map has to be stored.
	uint64_t	lm_mmap_addr;
	/// @brief Size (in bytes) of memory map.
	uint64_t	tr_mmap_len;
} __attribute__((packed));


#endif