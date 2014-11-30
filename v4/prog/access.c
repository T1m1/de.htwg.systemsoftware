#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5;

void *open_driver(void *info);

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


int main(int argc, char *argv[])
{
	/* parse options */
	int opt;
	int numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
	int opentest, closetest, duration;

	/* threads */
	pthread_t *threads;

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

		
	threads = malloc(numberOfThreads * sizeof(pthread_t));
	if (threads == NULL) {
		return EXIT_FAILURE;
	}

	/* open test */
	int i;
	for(i = 0; i < numberOfThreads; i++) {
		pthread_create(&threads[i], NULL, open_driver, NULL);
	}	



	return 0;
}

void *open_driver(void *info)
{

	pthread_exit(NULL);

}
