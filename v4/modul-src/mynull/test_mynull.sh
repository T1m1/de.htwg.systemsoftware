#!/bin/sh

PROG_NAME=mynull
PATH_TO_KO=/lib/modules/3.17.2/extra

## init
# unload modul (if loaded)
rmmod $PROG_NAME > /dev/null
# clear dmesg output 
dmesg -c > /dev/null

# show modul informations
echo "********** modul info **********"
modinfo $PATH_TO_KO/$PROG_NAME.ko

# load modul
insmod $PATH_TO_KO/$PROG_NAME.ko

# save major number from dmesg to make own device
major=`dmesg | grep -o "[^ ]*$"`

# show kernel logs for loading (e.g. last 10 lines)
echo "********** kernel logs for loading **********"
dmesg -c

# show information from /proc/devices
echo "********** /proc/devices **********"
cat /proc/devices

echo "********** test with access.c **********"
# make own devices
mknod /dev/major_0_test_$PROG_NAME c $major 0 
mknod /dev/major_1_test_$PROG_NAME c $major 1 

echo "parameter of access.c:"
echo "-d /dev/major_0_test_$PROG_NAME -r -v -t 5000 -n 5 -e 5 -m /dev/major_1_test_$PROG_NAME"

./usr/bin/access -d /dev/major_0_test_$PROG_NAME -r -v -t 5000 -n 5 -e 5 -m /dev/major_1_test_$PROG_NAME

echo "********** dmesg logs from access script **********"
dmesg -c

# unload modul (without .ko)
rmmod $PROG_NAME

# show kernel logs for unload
echo "********** kernel logs for unloading **********"
dmesg -c
