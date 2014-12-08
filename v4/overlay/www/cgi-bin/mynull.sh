#!/bin/sh
echo "Content-type: text/html"
echo " "
echo "`/usr/bin/test_mynull.sh > mynull`"
echo "`cat mynull`"

