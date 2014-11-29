#!/bin/sh

# clear dmesg output
dmesg -c > /dev/null

# show modul informations
modinfo template.ko

# load modul
insmod template.ko

# show kernel logs for loading (e.g. last 10 lines)
dmesg -c

# show information from /proc/devices
cat /proc/devices

# unload modul (without .ko)
rmmod template

# show kernel logs for unload
dmesg -c

