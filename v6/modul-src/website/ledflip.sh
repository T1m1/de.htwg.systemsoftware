#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_ledrpiflip.sh > /www/cgi-bin/ledrpiflip`"
echo "`cat /www/cgi-bin/ledrpiflip`"
