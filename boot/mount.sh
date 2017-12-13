#!/bin/bash

# Check the script is being run by root 
if [ "$(id -u)" != "0" ]; then 
    echo "This script must be run as root\n" 
    exit 1
fi

# Get directory of the script
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DISKFILE=disk.img
DISKPATH="$SCRIPTDIR/$DISKFILE"
LOOPDRIVE=`losetup --associated $DISKPATH --output NAME --noheadings`
echo $LOOPDRIVE

if [ -z "$LOOPDRIVE" ]
then
    LOOPDRIVE=`losetup -f`
    losetup $LOOPDRIVE $DISKPATH
else
    echo "Already mounted!"
fi

echo "Waiting for losetup..."
sleep 1s
echo "Resuming!"

echo "Connected $DISKFILE to $LOOPDRIVE"


KPARTXMSG=`kpartx -av $DISKPATH`
# cut output string to new devmapping
MAPNAME=${KPARTXMSG/add map /}
# add suffix and cut output string from another side
LOOP=${MAPNAME%%p1 *}
# puiing it in the standard directory
LOOPMAP=/dev/mapper/${LOOP}p1

echo "Waiting for kpartx..."
sleep 1s
echo "Mapped loopdrive $LOOPDRIVE to $LOOPMAP partition"

mount ${LOOPMAP} /mnt


(cd ..; make clean all)

make clean all

cp boot.elf /mnt/
cp grub.cfg /mnt/boot/grub/
cp ../kernel.img /mnt
umount /mnt
