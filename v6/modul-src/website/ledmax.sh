#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_ledrpimax.sh > /www/cgi-bin/ledrpimax`"
echo "`cat /www/cgi-bin/ledrpimax`"
