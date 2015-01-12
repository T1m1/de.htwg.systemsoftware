#!/bin/bash


SYSO_EXERCISE=v5

# versions
LINUX_VERSION=linux-3.17.2

# architecture
ARCH=arm

# toolchain
CROSS_COMPILER=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

# paths
HOME_PATH=$PWD
BUILDROOT_PATH=../buildroot
BR2_CONFIG=../$SYSO_EXERCISE/myconfig.buildroot
BUILDROOT_BUILD_PATH=$BUILDROOT_PATH/output/build
SERVER_PATH=/srv/tftp/rpi/3/
BR_KERNEL_PATH=$BUILDROOT_BUILD_PATH/$LINUX_VERSION

# dtb files and paths
DTB_BCM=bcm2835-rpi-b.dtb
DTB_BCM_PATH=$BR_KERNEL_PATH/arch/arm/boot/dts/$DTB_BCM

# Names
LINUX_VERSION=linux-3.17.2
BUSYBOX_VERSION=busybox-1.22.1


################ RPi ##############



copy_to_server()
{
	cp $BUILDROOT_PATH/output/images/rootfs.cpio.uboot $SERVER_PATH/rootfs.cpio.uboot
	cp $BUILDROOT_PATH/output/images/zImage $SERVER_PATH/zImage
	cp $HOME_PATH/uboot/tftpboot.scr $SERVER_PATH/tftpboot.scr
	cp $DTB_BCM_PATH $SERVER_PATH/$DTB_BCM
}

make_bcm_dtb() 
{
	# make the dtb for rpi
	cd $BR_KERNEL_PATH && make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILER $DTB_BCM
}



################ br packages ##############
prepare_br()
{
	cd $HOME_PATH/modul-package && ./makeLink.sh
	cd $HOME_PATH/modul-src && ./makeBuildrootPackets
}

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

################## QEMU ######################

qemu()
{
	cd $BUILDROOT_PATH/output/images && QEMU_AUDIO_DRV=none qemu-system-arm \
	-kernel zImage -m 128M -M vexpress-a9 \
	-nographic -append "root=/dev/ram initrd=/sbin/init console=ttyAMA0" \
	-initrd rootfs.cpio\
	-net nic,macaddr=00:00:00:00:00:1F,vlan=0\
	-net vde,sock="/tmp/vde2-tap0.ctl",vlan=0
}

qemu_serial()
{
	cd $BUILDROOT_PATH/output/images && QEMU_AUDIO_DRV=none qemu-system-arm \
	-kernel zImage -m 128M -M vexpress-a9 \
	-initrd rootfs.cpio\
	-nographic -serial pty \
	-append "root=/dev/ram initrd=/sbin/init console=ttyAMA0" \
	-net nic,macaddr=00:00:00:00:00:1F,vlan=0\
	-net vde,sock="/tmp/vde2-tap0.ctl",vlan=0
	
	# conect with ... screen /dev/pts/33 (Number can vary)
	# then... system_reset
}

qemu_debug()
{
	cd $BUILDROOT_PATH/output/images && QEMU_AUDIO_DRV=none qemu-system-arm \
	-kernel zImage -m 128M -M vexpress-a9 \
	-nographic -append "root=/dev/ram initrd=/sbin/init console=ttyAMA0" \
	-initrd rootfs.cpio \
	-S -gdb tcp::1234  
	
	# open new terminal and cd to ../output/build/linux-3.17.2/ then enter gdb
	# (gdb) file vmlinux
	# (gdb) target remote localhost:1234 (connects to gdb server)
	# (gdb) continue (or another debug cmd like next) 
}

################# ROOTFS #####################


make_program()
{
	cd $HOME_PATH/prog && make clean
	cd $HOME_PATH/prog && make

	cp $HOME_PATH/prog/systeminfo $HOME_PATH/overlay/usr/bin/systeminfo
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
	echo "-> prepare_br:		prepare buildroot to use own moduls"
	echo "-> make_buildroot:	compile buildroot"	
	echo "-> save_config:		save config of bb, kernel and buildroot to a seperate location"	
	echo "-> qemu:				starts qemu"
	echo "-> qemu_serial:		starts qemu with serial"
	echo "-> make_program		compiles user programs and copy it in overlay folder"
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
