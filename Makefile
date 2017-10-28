# The whole project is seperated in two subprojects: bootloader and the main binary
# No need to say, at some point our own custom bootloader will be replaced with grub/abstraction layer/efi
# I am not quite a fan of reinventing the bicycle
# Also I am looking forward to moving from flat binary storage to some reasonable partioning (i want GPT with legacy boot)
# to comment out some source file use '.' (like kernel.c vs .kernel.c)

# Compiled from root of the project

.PHONY: all clean

SHELL=bash
CC=gcc
CFLAGS = -std=gnu11 -m64 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w -mcmodel=large -fno-pic -fpermissive -fno-stack-protector
AS=fasm

all: ap_trump kernel

kernel: ap_trump
	make -f kernel.mk all

ap_trump:
	make -f ap_trump.mk all

clean:
	make -f kernel.mk clean

configure: config.json
	# consts.h generation
	touch $(consts_h)
	cat /dev/null > $(consts_h)
	printf "#ifndef CONSTS_H\n#define CONSTS_H\n\n" >> $(consts_h)
	./configure.py --lang c >> $(consts_h)
	printf "\n#endif" >> $(consts_h)
	# consts.ld generation
	touch $(consts_ld)
	cat /dev/null > $(consts_ld)
	./configure.py --lang ld >> $(consts_ld)
	# consts.inc generation
	touch $(consts_inc)
	cat /dev/null > $(consts_inc)
	./configure.py --lang fasm >> $(consts_inc)