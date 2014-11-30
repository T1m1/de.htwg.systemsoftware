#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
	TODO
	- paraleller zugriff -> Threading?
	- option -o = (open Test)
	- option -c = (close Test)
	- option -t = (waint time)

	Vorraussetzung:
	- Treiber ist geladen
*/


int main(int argc, char *argv[])
{
	int opt;
	while(-1 != (opt = getopt (argc, argv, "oct:"))) {
		switch(opt){
			case 'o':
				printf("open Test: %s\n", optarg);
				break;
			case 'c':
				printf("close Test: %s\n", optarg);
				break;
			case 't':
				printf("waiting time: %s\n", optarg);
				break;
		}
	}
	return 0;
}
