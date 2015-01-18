#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_buf.sh > buf`"
echo "`cat buf`"
