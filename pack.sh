#!/bin/bash

# Check the script is being run by root 
if [ "$(id -u)" != "0" ]; then 
    echo "This script must be run as root\n" 
    exit 1
fi

# Get directory of the script
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Mounting $DISKFILE to $LOOPDRIVE..."

DISKFILE=disk.img
DISKPATH="$SCRIPTDIR/boot/$DISKFILE"
LOOPDRIVE=`losetup --associated $DISKPATH --output NAME --noheadings`

if [ -z "$LOOPDRIVE" ]
then
    LOOPDRIVE=`losetup -f`
    losetup $LOOPDRIVE $DISKPATH
    echo "OK!"
else
    echo "Already mounted!"
fi

sleep 1s

echo "Mapping loopdrive $LOOPDRIVE to $LOOPMAP partition"

KPARTXMSG=`kpartx -av $DISKPATH`
# cut output string to new devmapping
MAPNAME=${KPARTXMSG/add map /}
# add suffix and cut output string from another side
LOOP=${MAPNAME%%p1 *}
# putting it in the standard directory
LOOPMAP=/dev/mapper/${LOOP}p1

sleep 1s

echo "Mounting $LOOPMAP to /mnt..."
mount ${LOOPMAP} /mnt

sleep 1s

echo "Copying files..."
cp boot/boot.elf /mnt/
cp boot/grub.cfg /mnt/boot/grub/
cp kernel.img /mnt
umount /mnt

kpartx -d $DISKPATH