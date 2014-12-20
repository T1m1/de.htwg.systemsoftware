#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_kthread.sh > kthread`"
echo "`cat kthread`"
