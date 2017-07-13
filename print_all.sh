#!/bin/bash
find . | grep '\.c\|\.h\|.asm\|.py\|makefile\|\.ld' | xargs -I{} ./print.sh {}
