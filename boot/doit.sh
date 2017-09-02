#!/bin/bash
# do mount.sh twice before (so buggy!)
make clean all
sudo mount /dev/mapper/loop* /mnt
sudo cp boot.elf /mnt/
sudo cp grub.cfg /mnt/boot/grub/
sudo cp ../kernel.img /mnt
sudo umount /mnt