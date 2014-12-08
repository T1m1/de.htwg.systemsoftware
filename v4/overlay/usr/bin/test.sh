#!/bin/sh

echo "insmod"
insmod /lib/modules/3.17.2/extra/openclose.ko

echo "mknod -> minor 1"
mknod /dev/timi c 254 1

