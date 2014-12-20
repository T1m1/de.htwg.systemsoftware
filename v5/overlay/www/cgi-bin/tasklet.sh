#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "`/usr/bin/test_tasklet.sh > tasklet`"
echo "`cat tasklet`"
