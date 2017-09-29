# TODO
- [x] makefiles! make it recursive way (solved with `find`)
- [x] grub! booting the multiboot way
- [ ] memory management (we still need it)
	+ [x] memory map (E820h? nah, grub) 
	+ [x] physical memory management (binary buddy on ~steroids~ bitmaps)
	+ [ ] virtual memory management
	+ [x] heap ~(need a proper algorithm, huh?)~ (let's say it's already done, huh?)
- [ ] Intel-lpss UART support (PCI: vendor 8086h, device a166h) (BK promised to work on that)
- [ ] ATA PIO
- [ ] FAT32 (should I use someone's realization?) (as of sep2017, not now: using grub module file ramdisk)
- [ ] GPT (not now; see FAT32)
- [ ] ioremap-style MMIO interactions
- [ ] USB
	+ [ ] xHCI (?)
	+ [ ] mass storage support
- [ ] virtualization
	+ [ ] make it work on revision > 1 (?!)
	+ [ ] EPT
	+ [ ] seabios
	+ [ ] UEFI? (Tianocore y'know)
	+ [ ] this is where magic happens
- [ ] seriously? by this time I'll graduate from university
