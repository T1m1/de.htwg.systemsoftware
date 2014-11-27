#!/bin/sh

LINUX_VERSION=linux-3.17.2

ARCH=arm
CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-
KDIR=~/syso_ws1415_14/buildroot/output/build/$LINUX_VERSION



make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE KDIR=$KDIR 
