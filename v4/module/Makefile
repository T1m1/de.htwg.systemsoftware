LINUX_VERSION=linux-3.17.2
LINUX_PATH=../../buildroot/output/build/linux-3.17.2
export ARCH=arm
export CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

SOURCES=mod1.c template.c openclose.c
OBJECT=$(SOURCES:.c=.o)

.PHONY: clean

ifneq ($(KERNELRELEASE),)
ohj-m	:= $(OBJECTS)

else
KDIR	:= $(LINUX_PATH)
PWD		:= $(shell pwd)

default:
	$(MAKE) -C $(LINUX_PATH) SUBDIRS=$(PWD) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
endif
