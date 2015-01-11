#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_led250.sh > led250`"
echo "`cat led250`"
