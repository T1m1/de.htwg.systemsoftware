#!/bin/sh

BUILDROOT_PACKAGE=../../buildroot/package
MODUL_PACKAGE=../../v4/modul-package
PACKAGE=syso

cd $BUILDROOT_PACKAGE
# make symbolic link to package directory
ln -s $MODUL_PACKAGE syso
