#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_timer.sh > timer`"
echo "`cat timer`"
