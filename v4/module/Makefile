ifneq ($(KERNELRELEASE),)
ohj-m	:= module_skel.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)

default:
		$(Make) -C $(KDIR) SUBDIRS=$(PWD) modules

endif
