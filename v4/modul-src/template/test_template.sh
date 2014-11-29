#!/bin/sh

# clear dmesg output
dmesg -c > /dev/null

# show modul informations
echo "**********modul info **********"
modinfo template.ko

# load modul
insmod template.ko

# show kernel logs for loading (e.g. last 10 lines)
echo "********** kernel logs for loading **********"
dmesg -c

# show information from /proc/devices
echo "********** /proc/devices **********"
cat /proc/devices

# unload modul (without .ko)
rmmod template

# show kernel logs for unload
echo "********** kernel logs for unloading **********"
dmesg -c

