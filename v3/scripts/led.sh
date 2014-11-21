#!/bin/sh

GPIO=18
ON=1
OFF=0
LED=/sys/class/gpio/gpio$GPIO/values

while true
do
	echo ON > $LED
	sleep 1

	echo OFF > $LED
	sleep 1
done

