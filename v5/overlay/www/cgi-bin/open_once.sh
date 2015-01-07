#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_open_once.sh > mutex`"
echo "`cat mutex`"
