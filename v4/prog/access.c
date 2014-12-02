#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5;
#define DEFAULT_DURATION 1;
#define DEFAULT_REPEAT 5;

void *open_driver(void *info);

/*
	TODO
	- HELP text
	- paraleller zugriff -> Threading? -> done
	- option -o = (open Test) -> done
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
	repeat = DEFAULT_REPEAT;

	/* threads */
	pthread_t *threads;

	while(-1 != (opt = getopt (argc, argv, "d:oct:r:n:"))) {
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
		printf("Usage: ./access -d [DEVICE_PATH] [OPTIONS]\n");
		return EXIT_FAILURE;
	}
		
	threads = malloc(numberOfThreads * sizeof(pthread_t));
	if (threads == NULL) {
		return EXIT_FAILURE;
	}

	/* open test */
	int i;
	if(opentest) {
		printf("OpenTest:\n");
		
		for(i = 0; i < numberOfThreads; i++) {
			printf("Start Thread %d\n", i);
			pthread_create(&threads[i], NULL, open_driver, NULL);
		}	


		for(i = 0; i < numberOfThreads; i++) {
			printf("Join Thread %d\n", i);
			pthread_join(threads[i], NULL);
		}
		
	}
	
	if(closetest) {
		printf("Close Test:\n");
	}
	
	free(threads);

	return 0;
}

void *open_driver(void *info)
{
	int i, fd;
	struct timespec sleep_time;

	
	/* init time struct */
	sleep_time.tv_sec = 1;
	sleep_time.tv_nsec = 0;
	
	for(i = 0; i < repeat; i++ ){
		printf("repeat %d", i);

		printf("Try to open Driver...\n");
		fd = open(device, O_RDONLY);
		if (fd < 0) 
		{
			printf("error - open");
		} else {
			printf("Open Driver!\n");
		}
		
		printf("Sleep %d Millisecond\n", duration);
		/* wait -> sleep thread safe? !!!! sind sekunden!!! andere funktion*/
		clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);
		/*sleep(duration);*/

		/* close driver */
		printf("Try to close driver...\n");
		if(fd >= 0) {
			if(close(fd) >= 0) {
				printf("Driver closed!\n");
			} else {
				printf("Driver not closed -.-.-.-.- !\n");
			}
		}
	}
	printf("END THREAD\n");
	pthread_exit(NULL);
}

