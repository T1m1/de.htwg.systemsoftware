#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_lock.sh > lock`"
echo "`cat lock`"
