# TODO
## Things we do NOW
- [x] makefiles! make it recursive way (solved with `find`)
- [x] grub! booting the multiboot way
	+ [ ] proper module management (we'll need it for BIOS, DOS ramdisks)
		* maybe use `tar` for it?
	+ [ ] get debug console from bootstruct
- [x] memory management (we still need it)
	+ [x] memory map (E820h? nah, grub) 
	+ [x] physical memory management (binary buddy on ~steroids~ bitmaps)
	+ [x] virtual memory management
		* [ ] virtual memory allocation
	+ [x] heap ~(need a proper algorithm, huh?)~ (let's say it's already done, huh?)	
- [ ] multiprocessor stuff **finally**:
	+ [x] working SIPI
	+ [x] boot into PM
	+ [ ] boot into LM
	+ [ ] vmx init (see `virtualization`)
- [x] kernel threads on BSP (scheduling works!)
	+ the way things *should* work:
		* VMs and stuff =(via buffers `termbuf`)=> `term_task` (also listens to keyboard and (serial?) commands) =(writes to a text buffer in ansi)=> `log_task` (basically an interface to...) => `VBE_DRIVER`, `SERIALMMIO_DRIVER`, `COMPORT_DRIVER`, `VGA_DRIVER`
	+ expected processes:
	+ [ ] kernel_idle
	+ [ ] logger (execs lower)
		* [ ] vbe_term/vga_term
		* [ ] uart_term
	+ [ ] shell (gui or cli) for VM interactions & settings
- [x] Intel-lpss UART support ~~(PCI: vendor 8086h, device a166h)~~; it's a 16550 uart (MMIO) with a fifo buffer
	+ ~~proper console management for SMP~~ (see previous point)
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
