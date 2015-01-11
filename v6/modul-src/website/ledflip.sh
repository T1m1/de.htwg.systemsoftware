#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_ledflip.sh > ledflip`"
echo "`cat ledflip`"
