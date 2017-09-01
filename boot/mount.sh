#!/bin/bash

DISK=disk.img
echo "root required"
LOOPRAW=`losetup -f`
losetup $LOOPRAW $DISK
TMP=`kpartx -av $DISK`
TMP2=${TMP/add map /}
LOOP=${TMP2%%p1 *}
LOOPDEV=/dev/${LOOP}
LOOPMAP=/dev/mapper/${LOOP}p1

if [ ! -e $LOOPDEV ] ; then
    echo "Bailing! $LOOPDEV is not valid"
    exit
fi

if [ ! -e $LOOPMAP ] ; then
    echo "Bailing! $LOOPMAP is not valid"
    exit
fi

mount ${LOOPMAP} /mnt