#!/bin/sh

echo "Content-type: text/html";
echo ""
echo "`cat /sys/class/gpio/gpio17/value`"
