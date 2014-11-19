#!/bin/sh


## defines
HOME_PATH=$PWD
V3ARM_APP=./v3arm


help ()
{
	echo "Usage: ./v3_2.sh [OPTIONS]"
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


buildroot()
{
	echo "download source..."
	$V3ARM_APP -e source_buildroot
}

compile()
{
	$V3ARM_APP -e make_br	
}

git_source()
{
	echo "kopieren der gitsource"
}

start_qemu()
{
	$V3ARM_APP -e v3_2_qemu
}


##################### main ########################


ARGS=`getopt -o hr:p:c:q: -- "$@"`

eval set -- "$ARGS"

argument=0

while true;
do
	case "$1" in
		'-run')
			if [ "$2" = "un" ] 
			then
				buildroot
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

