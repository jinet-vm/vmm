# TODO
## Things we do NOW
- [x] makefiles! make it recursive way (solved with `find`)
- [x] grub! booting the multiboot way
	+ [ ] proper module management (we'll need it for BIOS, DOS ramdisks)
		* maybe use `tar` for it?
- [x] memory management (we still need it)
	+ [x] memory map (E820h? nah, grub) 
	+ [x] physical memory management (binary buddy on ~steroids~ bitmaps)
	+ [x] virtual memory management
	+ [x] heap ~(need a proper algorithm, huh?)~ (let's say it's already done, huh?)
- [x] Intel-lpss UART support ~~(PCI: vendor 8086h, device a166h)~~; it's a 16550 uart (MMIO) with a fifo buffer
	+ [ ] proper console management for SMP
	+ [ ] fifo is not rocket science
- [ ] virtualization
	+ [x] make it work on revision > 1 (?!)
	+ [ ] EPT 
	+ [ ] seabios (not so distant future, I hope)


## ...and things we do *after*
- [ ] ATA PIO
- [ ] FAT32 (should I use someone's realization?) (as of sep2017, not now: using grub module file ramdisk)
- [ ] GPT (not now; see FAT32)
- [ ] ioremap-style MMIO interactions
- [ ] USB
	+ [ ] xHCI (?)
		* it's messy
		* it requires PCI support
		* it requires SCSI
		* it requires time
	+ [ ] mass storage support
- [ ] virtualization
	+ [ ] UEFI? (Tianocore y'know)
	+ [ ] this is where magic happens
- [ ] seriously? by this time I'll graduate from university
