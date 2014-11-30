LINUX_VERSION=linux-3.17.2
export KDIR=../../../buildroot/output/build/linux-3.17.2
export ARCH=arm
export CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

SOURCES=zero.c
OBJECTS=$(SOURCES:.c=.o)

.PHONY: clean

ifneq ($(KERNELRELEASE),)
obj-m	:= $(OBJECTS)

else
KDIR	:= $(KDIR)
PWD		:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
endif

clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.order
