#!/bin/sh

echo "Content-type: text/html";
echo ""
echo "`cat /sys/class/gpio/gpio18/value`"

