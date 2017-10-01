# The whole project is seperated in two subprojects: bootloader and the main binary
# No need to say, at some point our own custom bootloader will be replaced with grub/abstraction layer/efi
# I am not quite a fan of reinventing the bicycle
# Also I am looking forward to moving from flat binary storage to some reasonable partioning (i wanna GPT with legacy boot)
# to comment out some source file use '.' (like kernel.c vs .kernel.c)

.PHONY: all clean

SHELL=bash
CC=gcc
CFLAGS = -std=gnu11 -m64 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w -mcmodel=large -fno-pic -fpermissive -fno-stack-protector
AS=fasm

SRC_DIR ?= ./src ./data
OBJ_DIR ?= ./obj
SRCS := $(shell find $(SRC_DIR) -not -path '*/\.*' -and \( -name *.c -or -name *.asm \) ) 
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)

consts_h := include/jinet/consts.h
consts_ld := consts.ld
consts_inc := inc/consts.inc

all: $(OBJS)
	fasm src_bin/vm1.asm bin/vm1.bin
	fasm src_bin/vm2.asm bin/vm2.bin
	ld -T $(consts_ld) -T kernel.ld $(OBJS) -M -melf_x86_64 > kernel.map

$(OBJ_DIR)/%.asm.o: %.asm
	mkdir -p $(dir $@)
	$(AS) $< $@

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm obj/* -rf
	rm -f *.img
	rm -f *.map
	rm -f *.sym
	rm -f kernel_start

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