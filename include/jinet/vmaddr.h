#ifndef VMADDR_H
#define VMADDR_H

// todo: this is what pure WRONG looks like


// VMAs for kernel
#define VMA_PHYS_LOW 0x0
#define VMA_TERM_FB 0x100000000llu
#define VMA_TERM_FB_SIZE 0x3f48000llu
#define VMA_TTY 0x104000000llu
#define VMA_TTY_SIZE 0x10000 // almost 0xfd20 = 1920/8*1080/16*4 bytes
#define VMA_VMCS 0x104020000llu
#define VMA_VMCS_SIZE 0x10000
#define VMA_BBD_BITMAP 0x4000000000llu
#define VMA_BBD_BITMAP_SIZE 0x200000llu
#define VMA_HEAP 0x4000200000llu
#define VMA_HEAP_SIZE 0x10000


#define PADDR_AP_TRUMP 0x4000
#define PADDR_AP_TRUMP_SIZE 0x4000

#endif