#!/bin/bash

qemu-system-x86_64 -hda final.img -enable-kvm -cpu host,+vmx
