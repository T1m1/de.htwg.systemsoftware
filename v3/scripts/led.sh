#!/bin/sh

GPIO=18
ON=0
OFF=1
SLEEP=1
LED=/sys/class/gpio/gpio$GPIO/value
EXPORT=/sys/class/gpio/export
DIRECTION=/sys/class/gpio/gpio$GPIO/direction
# pin reservieren
echo "$GPIO" > $EXPORT

# festlegen ob Ein- oder Ausgabe
echo "out" > $DIRECTION

# pin bei abbrechen des scripts freigeben
trap "echo \"$GPIO\" >/sys/class/gpio/unexport" EXIT

while true
do
	echo "$ON" > $LED
	sleep $SLEEP
	echo "$OFF" > $LED
	sleep $SLEEP
done
