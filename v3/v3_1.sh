#!/bin/sh


## defines
HOME_PATH=$PWD
V3ARM_APP=./v3arm


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
	echo "compile kernel..."
	# prepare config
	$V3ARM_APP -e config_kernel
	$V3ARM_APP -e config_busybox
	# init
	$V3ARM_APP -e init_v3_1
	# compile
	$V3ARM_APP -e compile_kernel
	$V3ARM_APP -e make_bcm_dtb
	
}

git_source()
{
	echo "kopieren der gitsource"
}

start_qemu()
{
	$V3ARM_APP -e qemu_net
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

