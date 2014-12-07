#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0

#define DEFAULT_NUMBER_OF_THREADS 5
#define DEFAULT_DURATION 1000
#define DEFAULT_REPEAT 5
#define NANO_TO_MILI 1000000
#define BUFFER_SIZE 128

void *open_driver(void *threadarg);
void help(void);
void verbPrintf (int verbosity, char *format, ...);

struct globalOptions {
	struct timespec sleep_time;
	int repeat;
	int duration;
	int verbose;
	int open;
	int close;
	int read;
	int write;
	char *writeFile;
};

struct thread_info {
	struct globalOptions *global;
	int threadNumber;
	char *device;
};


int main(int argc, char *argv[])
{
	/* parse options */
	int opt;
	int numberOfThreads = DEFAULT_NUMBER_OF_THREADS;
	char *minorOneDevice;
	int minortest = FALSE;
	char *device = NULL;
	
	struct globalOptions *global;
    global = malloc (sizeof (struct globalOptions));
    if (global == NULL) {
            perror ("malloc");
            exit (EXIT_FAILURE);
    }
    global->duration = DEFAULT_DURATION;
    global->repeat = DEFAULT_REPEAT;


	while(-1 != (opt = getopt (argc, argv, "d:oct:e:n:m:hvrw:"))) {
		switch(opt){
			case 'o':
				global->open = TRUE;
				break;
			case 'c':
				global->close = TRUE;
				break;
			case 'r':
				global->read = TRUE;
				break;
			case 'w':
				global->write = TRUE;
				global->writeFile = optarg;
				break;
			case 't':
				global->duration = atoi(optarg);
				break;
			case 'n':
				numberOfThreads = atoi(optarg);
				break;
			case 'd':
				device = optarg;
				break;
			case 'e':
				/* echo instead of repead, r=read */
				global->repeat = atoi(optarg);
				break;
			case 'm':
				minorOneDevice = optarg;
				minortest = TRUE;
				break;
			case 'v':
				global->verbose = TRUE;
				break;
			case 'h':
				help();
				break;
		}
	}

	/* check if path ommitted */
	if(device == NULL) {
		help();
		return EXIT_FAILURE;
	}	
	
	/* print verbose information */
	verbPrintf(global->verbose, "Verbose: ON\n");
	verbPrintf(global->verbose, "Waiting time: %d\n",
                            global->duration);
    verbPrintf(global->verbose, "Number of threads: %d\n", numberOfThreads);
    verbPrintf(global->verbose, "Path to device: %s\n", device);
    verbPrintf(global->verbose, "Number of repeats: %d\n", global->repeat);
    	
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
	verbPrintf(global->verbose, "Start tests:\n");
	for(i = 0; i < numberOfThreads; i++) {
		thread_struct[i].global = global;
		thread_struct[i].threadNumber = i;
		
		/* if minortest is active
		 * 	-> every 2nd time use another minor number */
		if(minortest && (i%2)) {
			verbPrintf(global->verbose, "Thread nr. %d start with device %s\n", i, minorOneDevice);
			thread_struct[i].device = minorOneDevice;
		} else {
			verbPrintf(global->verbose, "Thread nr. %d start with device %s\n", i, device);
			thread_struct[i].device = device;	
		}
		
		if (pthread_attr_init (&attr[i]) == -1)
		{
			perror ("error in pthread_attr_init");
		}
		
		pthread_create(&threads[i], &attr[i], open_driver, (void *) &thread_struct[i]);
	}	

	for(i = 0; i < numberOfThreads; i++) {
		verbPrintf(global->verbose, "Join thread nr. %d\n", i);
		pthread_join(threads[i], NULL);
		
		if (pthread_attr_destroy (&attr[i]) == -1)
		{
			perror ("error in pthread_attr_init");
			exit (1);
		}
	}
	verbPrintf(global->verbose, "Finish tests!\n");
	
	
	free(threads);
	free(attr);
	free(thread_struct);
	free(global);
	
	verbPrintf(global->verbose, "End of program!\n");
	
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

	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = (NANO_TO_MILI * duration);
	
	for(i = 0; i < repeat; i++ ){
		verbPrintf(t->global->verbose, "Thread %d: repeat %d\n", threadNumber, i);
		if(t->global->open || t->global->close | t->global->read) {	
			verbPrintf(t->global->verbose, "Thread %d: try to open driver..\n", threadNumber);
			fd = open(t->device, O_RDONLY);
			if (fd < 0) {
				verbPrintf(t->global->verbose, "Thread %d: could not open driver!\n", threadNumber);
			} else {
				verbPrintf(t->global->verbose, "Thread %d: open driver!\n", threadNumber);
			
					/* read test */
				if (t->global->read) {
					char buf[BUFFER_SIZE];
					int ret;
					int rounds = 0;
					
					while ((rounds < 50) && (ret = read(fd, buf, BUFFER_SIZE))) {
						verbPrintf(t->global->verbose, "Read data %s\n", buf);
						rounds++;
					}

					if (!ret || (rounds >= 50))
						printf("Read finish after 50 lines or EOF\n");
					else {
						perror("Error closing file.\n");
						pthread_exit(NULL);
					}
				}
			}
		}
		
		
		/* write test */
		if (t->global->write) {
			ssize_t nrd;
			char buf[BUFFER_SIZE];
			
			int wf_fd = open(t->global->writeFile, O_RDONLY);
			int w_fd = open(t->device, O_WRONLY);

			if ((w_fd < 0) || (wf_fd < 0)) {
				verbPrintf(t->global->verbose, "Thread %d: could not open driver for writing or file for reading!\n", threadNumber);
			} else {
				verbPrintf(t->global->verbose, "Thread %d: open driver for writing!\n", threadNumber);
				
				while((nrd = read(wf_fd, buf, 50))){
					 write(w_fd, buf, nrd);
					 verbPrintf(t->global->verbose, "Write data %s\n", buf);
				}
				close(wf_fd);
				close(w_fd);
			}
		}

		verbPrintf(t->global->verbose, "Thread %d: sleep %d miliseconds!\n", threadNumber, duration);
		clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);

		if(t->global->open || t->global->close | t->global->read) {	
			verbPrintf(t->global->verbose, "Thread %d: try to close driver!\n", threadNumber);		
			if(fd >= 0) {
				if(close(fd) >= 0) {
					verbPrintf(t->global->verbose, "Thread %d: driver closed!\n", threadNumber);
				} else {
					verbPrintf(t->global->verbose, "Thread %d: driver not closed!\n", threadNumber);
				}
			}
		}
	}
	verbPrintf(t->global->verbose, "Thread %d: end of thread!\n", threadNumber);
	pthread_exit(NULL);
}


void help(void)
{
	printf(	"\nUsage: ./access -d [DEVICE_PATH] [OPTIONS]\n"
			"\n"
			"\n"
			"\tOPTIONS:\n"
			"\t-d			path to device"
			"\t-o			open test\n"
			"\t-c			close test\n"
			"\t-r			read test\n"
			"\t-w [PATH]	write test - PATH to any file to read\n"
			"\t-t [TIME]	time in millisecond to sleep between open and close\n"
			"\t				default: 1000\n"
			"\t-n [NR]		number of threads\n"
			"\t				default: 5\n"
			"\t-e [NR]		number of echos (repeats)\n"
			"\t				default: 5\n"
			"\t-m [PATH]	start tests with different minor numbers\n"
			"\t	  [PATH]	to device with different minor\n"
			"\t-v			verbose\n"
			"\t\n");
}

void
verbPrintf (int verbosity, char *format, ...)
{
        va_list args;
        va_start (args, format);
        if (verbosity)
        {
                vfprintf (stdout, format, args);
        }

}

