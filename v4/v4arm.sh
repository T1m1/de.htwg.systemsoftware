#!/bin/bash

# architecture
ARCH=arm

# toolchain
CROSS_COMPILER=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

# paths
HOME_PATH=$PWD
BUILDROOT_PATH=../buildroot
BR2_CONFIG=../v4/myconfig.buildroot
BUILDROOT_BUILD_PATH=$BUILDROOT_PATH/output/build

# Names
LINUX_VERSION=linux-3.17.2
BUSYBOX_VERSION=busybox-1.22.1


################ buildroot ################
make_buildroot()
{
	cd $BUILDROOT_PATH && make source
	cd $BUILDROOT_PATH && make 
}

source_buildroot()
{
	# loads buildroot from git
	git clone git://git.buildroot.net/buildroot $BUILDROOT_PATH
	cd $BUILDROOT_PATH
	git reset --hard 80e406090893ab5a7b5d78e4d243d12fca2b22fb
	rm -rf .git
	cd $BUILDROOT_PATH && make defconfig BR2_DEFCONFIG=$BR2_CONFIG
}

save_configs()
{
	# saves the config of kernel and busybox to a separate location
	cp $BUILDROOT_BUILD_PATH/$LINUX_VERSION/.config $HOME_PATH/.config_kernel
	cp $BUILDROOT_BUILD_PATH/$BUSYBOX_VERSION/.config $HOME_PATH/.my_busybox_config
	cd $BUILDROOT_PATH && make savedefconfig BR2_DEFCONFIG=$BR2_CONFIG
}

startup()
{
	cd $BUILDROOT_PATH/output/images && QEMU_AUDIO_DRV=none qemu-system-arm -kernel zImage -m 128M -M vexpress-a9 \
	-nographic -append "root=/dev/ram initrd=/sbin/init console=ttyAMA0" \
	-initrd rootfs.cpio\
	-net nic,macaddr=00:00:00:00:00:1D,vlan=0\
	-net vde,sock="/tmp/vde2-tap0.ctl",vlan=0
}


################## HELP ######################
help()
{
	echo "--------------------------------------------------------"
	echo "usage:"
	echo "./v4arm -e [function]"
	echo ""
	echo "functions:"
	echo ""
	echo "-> source_buildroot:  get buildroot source"
	echo "-> make_buildroot:	compile buildroot"	
	echo "-> save_config:		save config of bb, kernel and buildroot to a seperate location"	
	echo "-> startup:			starts qemu"
	echo ""
	echo "  STUFF: (not supported yet)"
	echo "-> init:	    initialize all needed files for kernel"
	echo "-> qemu_net:  starts qemu with compiled kernel"
	echo "-> all:	    does all above, without starting qemu"
	echo "-> run:	    easy call for qemu_net"
	echo "-> clean:	    clean ALL generated files (busybox, kernel, ..)"
	echo "........................................................"
	echo "-> menu_<X>   either <busybox> or <kernel> of <buildroot> for the menuconfig"
	echo "--------------------------------------------------------"
	
	
}

while getopts ":e:h" opt; do
		case $opt in
			e)
				cmd=$OPTARG
				;;
			h)	
				help
				;;
			:)
				echo "missing argument"
				;;
			?)
				help
				;;
			*)
				echo "not yet implemented"
				help
				;;
		esac
done

$cmd
