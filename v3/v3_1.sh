#!/bin/sh


HOME_PATH=$PWD

V3ARM_APP=./v3arm
# patch file
PATCH_NAME=linux-smsc95xx_allow_mac_setting.patch
PATCH_FILE=$HOME_PATH/patch/$PATCH_NAME

# paths
KERNEL_PATH=../$LINUX_VERSION
BUSYBOX_PATH=../$BUSYBOX_VERSION

# versions
LINUX_VERSION=linux-3.17.2
BUSYBOX_VERSION=busybox-1.22.1

# --------
LINUX_PACKAGE=$LINUX_VERSION.tar.xz
LINUX_SOURCE=https://www.kernel.org/pub/linux/kernel/v3.x/$LINUX_PACKAGE
BUSYBOX_PACKAGE=$BUSYBOX_VERSION.tar.bz2
BUSYBOX_SOURCE=http://busybox.net/downloads/$BUSYBOX_PACKAGE


help ()
{
	echo "Usage: ./v3_1.sh [OPTIONS]"
	echo ""
	echo "excample call: ./v3_1.sh -dn -pa"
	echo ""
	echo " -dn		download sources"
	echo " -pa		patch"
	echo " -cp		copy gitlab sources"	
	echo " -co		compile"
	echo " -qe		start qemu"
	echo " -h		help"
	
}


download_source()
{
	echo "download source..."
	$V3ARM_APP -e source
}

patch()
{
	echo "patch kernel..."
	$V3ARM_APP -e patch_kernel
}

compile()
{
	echo "compilieren"
}

git_source()
{
	echo "kopieren der gitsource"
}

start_qemu()
{
	echo "qemu starten"
}


##################### main ########################


ARGS=`getopt -o hd:p:c:q: -- "$@"`

eval set -- "$ARGS"

argument=0

while true;
do
	case "$1" in
		'-d')
			if [ "$2" = "n" ] 
			then
				download_source
			else
				help
			fi
			argument=1
			shift 2;;
		'-p')
			if [ "$2" = "a" ] 
			then
				patch
			else
				help
			fi
			argument=1
			shift 2;;
		'-c')
			if [ "$2" = "p" -o "$2" = "o" ] 
			then
				if [ "$2" = "o" ] 
				then
					compile
				else
					git_source
				fi
			else
				help
			fi
			argument=1
			shift 2;;
		'-q')
			if [ "$2" = "e" ] 
			then
				start_qemu
			else
				help
			fi
			argument=1
			shift 2;;
		'-h')
			help
			argument=1
			break;;
		*)
			break;;
	esac
done

if [ $argument -eq 0 ]
then
	echo "argument required"
	help
fi

