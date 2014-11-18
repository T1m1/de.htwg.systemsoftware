#!/bin/sh




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


##################### main ########################


ARGS=`getopt -o hd:p:c:q: -- "$@"`

eval set -- "$ARGS"


while true;
do
	case "$1" in
		'-d')
			if [ "$2" = "n" ] 
			then
				echo "download source"
			else
				help
			fi
			shift 2;;
		'-p')
			if [ "$2" = "a" ] 
			then
				echo "patchen"
			else
				help
			fi
			shift 2;;
		'-c')
			if [ "$2" = "p" -o "$2" = "o" ] 
			then
				if [ "$2" = "o" ] 
				then
					echo "compilieren"
				else
					echo "kopieren der gitsource"
				fi
			else
				help
			fi
			shift 2;;
		'-q')
			if [ "$2" = "e" ] 
			then
				echo "qemu starten"
			else
				help
			fi
			shift 2;;
		'-h')
			help
			break;;
		*)
			break;;
	esac
done
