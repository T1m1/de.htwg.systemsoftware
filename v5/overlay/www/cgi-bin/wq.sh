#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_wq.sh > wq`"
echo "`cat wq`"
