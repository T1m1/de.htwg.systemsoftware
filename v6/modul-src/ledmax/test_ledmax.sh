#!/bin/sh

PROG_NAME=mygpio
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

# show kernel logs for loading (e.g. last 10 lines)
echo "********** kernel logs for loading **********"
dmesg -c

# show information from /proc/devices
echo "********** /proc/devices **********"
cat /proc/devices
cd /usr/bin
./ledmax &
echo `ps -a`

echo "wait 5 seconds ..."
# wait 5 seconds 
sleep 10

# kill process
processID=`ps -a | grep ledmax -m 1 | grep -o "[0-9]*" | grep -o "^[0-9]*" -m 1`
echo "kill $processID"
kill -SIGINT $processID

echo "********** kernel logs for testing **********"
dmesg -c

# unload modul (without .ko)
rmmod $PROG_NAME

# show kernel logs for unload
echo "********** kernel logs for unloading **********"
dmesg -c

