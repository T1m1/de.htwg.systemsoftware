#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5;

/*
	TODO
	- paraleller zugriff -> Threading?
	- option -o = (open Test)
	- option -c = (close Test)
	- option -t = (waint time)
	- option -n = anzahl threads (default 5)
	
	Vorraussetzung:
	- Treiber ist geladen
*/

int numberOfThreads = DEFAULT_NUMBER_OF_THREADS;

int main(int argc, char *argv[])
{
	/* parse options */
	int opt;
	int opentest, closetest, time = FALSE;	
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
			case 'n':
				printf("number of threads: %s\n", optarg);
				numberOfThreads = atoi(optarg);
				break;
		}
	}

		




	return 0;
}
