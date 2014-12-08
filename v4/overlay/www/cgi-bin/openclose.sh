#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_openclose.sh > openclose`"
echo "`cat openclose`"
