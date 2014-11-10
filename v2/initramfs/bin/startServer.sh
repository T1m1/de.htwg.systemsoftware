#!/bin/sh

ifconfig lo up
ifconfig eth0 up
udhcpc -i eth0 -s /etc/udhcpc/simple.script

httpd -h /www/
