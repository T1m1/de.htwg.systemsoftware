LINUX_VERSION=linux-3.17.2
export KDIR=../../../buildroot/output/build/linux-3.17.2
export ARCH=arm
export CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

NAME=open_once
TARGET_DIR=/usr/bin/
SOURCES=$(NAME).c
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

install:
	scp $(NAME).ko root@192.168.29.31:$(TARGET_DIR)$(NAME).ko
	scp test_$(NAME).sh root@192.168.29.31:$(TARGET_DIR)test_$(NAME).sh
