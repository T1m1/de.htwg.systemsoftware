#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_ledrpi250.sh > /www/cgi-bin/ledrpi250`"
echo "`cat /www/cgi-bin/ledrpi250`"
