#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5;
#define DEFAULT_DURATION 500;

void *open_driver(void *info);

/*
	TODO
	- paraleller zugriff -> Threading?
	- option -o = (open Test)
	- option -c = (close Test)
	- option -t = (waint time)
	- option -n = anzahl threads (default 5)
	- option -d = path to device
	Vorraussetzung:
	- Treiber ist geladen
*/

static char *device;
static int repeat, duration;

int main(int argc, char *argv[])
{
	/* parse options */
	int opt;
	int numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
	int opentest, closetest = FALSE;
	duration = DEFAULT_DURATION;

	/* threads */
	pthread_t *threads;

	while(-1 != (opt = getopt (argc, argv, "d:oct:r:"))) {
		switch(opt){
			case 'o':
				printf("open Test: %s\n", optarg);
				opentest = TRUE;
				break;
			case 'c':
				printf("close Test: %s\n", optarg);
				closetest = TRUE;
				break;
			case 't':
				printf("waiting time: %s\n", optarg);
				duration = atoi(optarg);
				break;
			case 'n':
				printf("number of threads: %s\n", optarg);
				numberOfThreads = atoi(optarg);
				break;
			case 'd':
				printf("path to device: %s\n", optarg);
				device = optarg;
				break;
			case 'r':
				printf("number of repeats: %s\n", optarg);
				repeat = atoi(optarg);
				break;
		}
	}

	/* check if path ommitted */
	if(device == NULL){
		printf("Usage: ./access [DEVICE_PATH] [OPTIONS]\n");
		return EXIT_FAILURE;
	}
		
	threads = malloc(numberOfThreads * sizeof(pthread_t));
	if (threads == NULL) {
		return EXIT_FAILURE;
	}

	/* open test */
	int i;
	if(opentest) {
		for(i = 0; i < numberOfThreads; i++) {
			pthread_create(&threads[i], NULL, open_driver, NULL);
		}	


		for(i = 0; i < numberOfThreads; i++) {
			pthread_join(threads[i], NULL);
		}
	}
	
	if(closetest) {

	}
	
	free(threads);

	return 0;
}

void *open_driver(void *info)
{
	int *openptr;
	int ret, i;

	openptr = malloc(repeat * sizeof(int));
	if (NULL == openptr) {
		ret = EXIT_FAILURE;
		pthread_exit(&ret);		
	}
	
	/* open driver */
	for(i = 0; i < repeat; i++) {
		openptr[i] = open(device, O_RDONLY);
	}

	/* wait -> sleep thread safe? */
	sleep(duration);

	/* close driver */
	for(i = 0; i < repeat; i++) {
		if(0 <= openptr[i]) {
			close(openptr[i]);	
		}
	}

	pthread_exit(NULL);

}

