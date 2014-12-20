#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_mutex.sh > mutex`"
echo "`cat mutex`"
