#!/bin/bash

qemu-system-x86_64 -hda disk.img -enable-kvm -cpu host,+vmx -smp 3 -m 1G
