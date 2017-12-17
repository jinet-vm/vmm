#!/bin/bash

qemu-system-x86_64 -hda boot/disk.img -enable-kvm -cpu host,+vmx