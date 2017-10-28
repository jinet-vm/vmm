# trampoline for application processors

.PHONY: all clean

SHELL=bash
CC=gcc
CFLAGS = -std=gnu11 -m64 -o0 -Iinclude -ffreestanding -nostdlib -lgcc -w -mcmodel=large -fno-pic -fpermissive -fno-stack-protector
AS=fasm

SRC_DIR ?= ./src/ap_trump
BIN_DIR ?= ./bin

all: $(SRC_DIR)/boot.asm
	fasm $(SRC_DIR)/boot.asm $(BIN_DIR)/ap_trump.bin

clean:
	rm $(BIN_DIR)/boot.bin