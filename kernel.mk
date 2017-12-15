# kernel running on BSP

.PHONY: all clean

SHELL=bash
CC=gcc
CFLAGS = -std=gnu11 -m64 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w -mcmodel=large -fno-pic -fpermissive -fno-stack-protector
AS=fasm

SRC_DIR ?= ./src/kernel
OBJ_DIR ?= ./obj
SRCS := $(shell find $(SRC_DIR) -not -path '*/\.*' -and \( -name *.c -or -name *.asm \) ) 
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)

all: $(OBJS)
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
