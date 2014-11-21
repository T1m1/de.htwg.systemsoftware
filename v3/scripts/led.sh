#!/bin/sh

GPIO=18
ON=1
OFF=0
LED=/sys/class/gpio/gpio$GPIO/value

# pin bei abbrechen des scripts freigeben
trap "echo \"$GPIO\" >/sys/class/gpio/unexport" EXIT

while true
do
	echo "$ON" > $LED
	sleep 1
	echo "$OFF" > $LED
	sleep 1
done

