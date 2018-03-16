# boot routine for grub

.PHONY: all clean

SHELL=bash
CC=gcc
CFLAGS = -Iinclude -std=gnu11 -m32 -o0 -ffreestanding -nostdlib -lgcc -w -fno-pic -fpermissive -fno-stack-protector
# fix for __FILENAME__ mprint:
CFLAGS += -D__FILENAME__=\"$(notdir $<)\"
AS=fasm

SRC_DIR ?= ./src/grub_if
OBJ_DIR ?= ./obj/boot
SRCS := $(shell find $(SRC_DIR) -not -path '*/\.*' -and \( -name *.c -or -name *.asm \) ) 
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)

all: $(OBJS)
	ld -T boot.ld $(OBJS) -melf_i386

$(OBJ_DIR)/%.asm.o: %.asm
	mkdir -p $(dir $@)
	$(AS) $< $@

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) -rf