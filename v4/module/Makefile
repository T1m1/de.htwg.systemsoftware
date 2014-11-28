LINUX_VERSION=linux-3.17.2
LINUX_PATH=../../buildroot/output/build/$LINUX_VERSION
ARCH=arm
CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

SOURCES=mod1.c
OBJECT=$(SOURCES:.c=.o)

.PHONY: clean

ifneq ($(KERNELRELEASE),)
ohj-m	:= $(OBJECTS)

else
KDIR	:= $(LINUX_PATH)
PWD		:= $(shell pwd)

default:
	$(Make) -C $(LINUX_PATH) SUBDIRS=$(PWD) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

endif
