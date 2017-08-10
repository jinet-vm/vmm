// structure placed in the very beginning of Long Mode kernel binary
// telling 32-bit trampoline where to load the kernel
// 'Hey, mom, I think I have invented an executable format!'
// there are fields defined by the trampoline kernel
// and fields defined by the LM-kernel

#ifndef BOOTSTRUCT_H
#define BOOTSTRUCT_H

#define BTSTR_LM_MAGIC 0x0DEAD5AD // dead sad
#define BTSTR_TR_MAGIC 0xBEEFCAFE // beef cafe

#define BTSTR_VDTP_VGA_TEXT 0
#define BTSTR_VDTP_VGA_GRPH 1
#define BTSTR_VDTP_VBE_GRPH 2

// lm_* - Long Mode binary filled fields
// tr_* - Trampoline binary filled fields

struct bootstruct
{
	uint32_t	lm_magic;
	uint32_t	tr_magic;
	uint64_t	lm_load_addr; // vma
	uint64_t	lm_entry_addr; // vma
	uint64_t	tr_phys_addr; // of kernel loaded
	uint64_t 	tr_video_type; // BTSTR_VDTP_*
	uint64_t	tr_vd_framebuffer;
	uint64_t	tr_vd_width;
	uint64_t	tr_vd_height;
	uint64_t	tr_vd_depth;
	uint64_t	lm_mmap_addr; // it must be copied there by tr_kernel
	uint64_t	tr_mmap_len;
} __attribute__((packed));

#endif