#!/bin/sh

BUILDROOT_PACKAGE=../../buildroot/package
MODUL_PACKAGE=../../v5/modul-package
PACKAGE=syso

cd $BUILDROOT_PACKAGE
# make symbolic link to package directory
ln -s $MODUL_PACKAGE syso


sed -i -r '/	source \"package\/sound-theme-freedesktop\/Config.in\"/a\\x9source "package/syso/Config.in"' Config.in
