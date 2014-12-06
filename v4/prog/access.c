#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5;
#define DEFAULT_DURATION 1;
#define DEFAULT_REPEAT 5;
#define NANO_TO_MILI 1000

void *open_driver(void *threadarg);
void help(void);

struct globalOptions {
	struct timespec sleep_time;
	int repeat;
	int duration;
	char *device;
};

struct thread_info {
	struct globalOptions *global;
	int threadNumber;
};


int main(int argc, char *argv[])
{
	/* parse options */
	int opt;
	int numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
	int opentest, closetest = FALSE;
	char *minorOneDevice;
	int minortest=FALSE;
	int duration = DEFAULT_DURATION;
	int repeat = DEFAULT_REPEAT;
	char *device;
	
	struct globalOptions *global;
    global = malloc (sizeof (struct globalOptions));
    if (global == NULL) {
                perror ("malloc");
                exit (EXIT_FAILURE);
    }


	while(-1 != (opt = getopt (argc, argv, "d:oct:r:n:m:h"))) {
		switch(opt){
			case 'o':
				opentest = TRUE;
				break;
			case 'c':
				closetest = TRUE;
				break;
			case 't':
				printf("waiting time: %s\n", optarg);
				global->duration = atoi(optarg);
				break;
			case 'n':
				printf("number of threads: %s\n", optarg);
				numberOfThreads = atoi(optarg);
				break;
			case 'd':
				printf("path to device: %s\n", optarg);
				global->device = optarg;
				break;
			case 'r':
				printf("number of repeats: %s\n", optarg);
				global->repeat = atoi(optarg);
				break;
			case 'm':
				printf("multi-testing for minor number with device: %s\n", optarg);
				minorOneDevice = optarg;
				minortest = TRUE;
				break;
			case 'h':
				help();
				break;
		}
	}

	/* check if path ommitted */
	if(global->device == NULL) {
		help();
		return EXIT_FAILURE;
	}	
	
		
	/********************************************
    *       struct initialisiation              *
    *********************************************/

	struct thread_info *thread_struct = NULL;	
	thread_struct = malloc(numberOfThreads * sizeof (struct thread_info));
		
	/* threads */
	pthread_t *threads;
	pthread_attr_t *attr;
	
	threads = malloc(numberOfThreads * sizeof(pthread_t));
	attr = malloc(numberOfThreads * sizeof(pthread_attr_t));
	
	/* check memory allocation */
	if (thread_struct == NULL || attr == NULL || threads == NULL){
		fprintf(stderr, "Failed to allocate memory!\n");
		perror ("malloc");
        exit (EXIT_FAILURE);
	}

	/* open test */
	int i;
	if(opentest) {
		printf("OpenTest:\n");
		for(i = 0; i < numberOfThreads; i++) {
			thread_struct[i].global = global;
			thread_struct[i].threadNumber = i;
			
			/* if minortest is active
			 * 	-> every 2nd time use another minor number */
			if(minortest && (i%2)) {
				thread_struct[i].global->device = minorOneDevice;
			}
			//thread_struct->threadNumber= i;
			if (pthread_attr_init (&attr[i]) == -1)
            {
				perror ("error in pthread_attr_init");
            }
            
			printf("Start Thread %d\n", i);
			pthread_create(&threads[i], &attr[i], open_driver, (void *) &thread_struct[i]);
			printf("Thread %d created\n", i);
		}	

		printf("Alles losgeschickt\n");
		for(i = 0; i < numberOfThreads; i++) {
			printf("Join Thread %d\n", i);
			pthread_join(threads[i], NULL);
			
			if (pthread_attr_destroy (&attr[i]) == -1)
			{
				perror ("error in pthread_attr_init");
				exit (1);
			}
		}
	}
	
	/* close test */
	if(closetest) {
		printf("Close Test:\n");
	}
	
	free(threads);
	free(attr);
	free(thread_struct);
	free(global);
	
	printf("End program\n");
	
	return 0;
}

void *open_driver(void *threadarg)
{
	int i, fd;
	struct thread_info *t = (struct thread_info *) threadarg;
	struct timespec sleep_time;
	int duration = t->global->duration;
	int repeat = t->global->repeat;
	int threadNumber = t->threadNumber;
	
	printf("Device: %s", t->global->device);

	sleep_time.tv_sec = NANO_TO_MILI * duration;

	for(i = 0; i < repeat; i++ ){
		printf("THREAD %d: repeat %d\n", threadNumber, i );

		printf("THREAD %d: Try to open Driver...\n", threadNumber);
		fd = open(t->global->device, O_RDONLY);
		if (fd < 0) {
			printf("THREAD %d: Could not open", threadNumber);
		} else {
			printf("THREAD %d: Open Driver!\n", threadNumber);
		}
		
		printf("THREAD %d: Sleep %d Millisecond\n", threadNumber, duration *NANO_TO_MILI);
		clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);

		printf("weiter gehts");
		printf("THREAD %d: Try to close driver...\n", threadNumber);
		if(fd >= 0) {
			if(close(fd) >= 0) {
				printf("THREAD %d: Driver closed!\n", threadNumber);
			} else {
				printf("THREAD %d: Driver not closed -.-.-.-.- !\n", threadNumber);
			}
		}
	}
	printf("THREAD %d: END THREAD\n", threadNumber);
	pthread_exit(NULL);
}
/*
	TODO
	- HELP text
	- verbose output
	- paraleller zugriff  -> done
	- option -o = (open Test) -> done
	- option -c = (close Test) -> why? :D
	- option -t = (waint time)
	- option -n = anzahl threads (default 5)
	- option -r = repeat of test
	- option -d = path to device
	- option -m = multitesting with different minor
	Vorraussetzung:
	- Treiber ist geladen
*/
void help(void)
{
	printf(	"\nUsage: ./access -d [DEVICE_PATH] [OPTIONS]\n"
			"\n"
			"\n"
			"\t OPTIONS:\n"
			"\t-o		open test\n"
			"\t-c		close test\n"
			"\t-t [TIME]	time in millisecond to sleep between open and close\n"
			"\t-n [NR]	number of threads\n"
			"\t-r [NR]	number of repeats\n"
			"\t-m [PATH]	start tests with different minor numbers\n"
			"\t		[PATH] to device with different minor\n"
			"\t-v 		verbose\n"
			"\t\n");
}

