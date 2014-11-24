#!/bin/sh

echo "Content-type: text/html";
echo ""
echo "`echo "1" > /sys/class/gpio/gpio17/value`"
