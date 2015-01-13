#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define CREATE_THREAD_SUCCESSFULL    0
#define THREAD_SUCCESSFULL_JOINED    0

#define LED_ON                      "0"
#define LED_OFF                     "1"
#define DRIVER_PATH                 "/dev/mygpio"

typedef struct {
    bool finishExec;
} userOptions;

static userOptions *free_ptr = NULL;

static void initUserOptions(userOptions*);

void signalHandler(int);


int main(void)
{
    userOptions options;
    int file;
    ulong max = 0;
    ulong cycles = 0;
    ulong freq = 0;    
    ulong tmp = 0;
    struct timespec time1, time2;
    time1.tv_sec = 0;
    time1.tv_nsec = 0;
    time2.tv_sec = 0;
    time2.tv_nsec = 0;

    initUserOptions(&options);
    free_ptr = &options;

    /* register signal handler */
    signal(SIGINT, signalHandler);

    file = open(DRIVER_PATH, O_WRONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("Start to measure, please wait.\n");

    while(!options.finishExec) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        /* led off */
        if (1 != write(file, LED_OFF, 1)) {
            printf("Write to driver failed!\n");
            break;
        }
        /* led on */
        if (1 != write(file, LED_ON, 1)) {
            printf("Write to driver failed!\n");
            break;
        }
	clock_gettime(CLOCK_MONOTONIC, &time2);

	tmp = time2.tv_sec - time1.tv_sec;
	tmp = tmp * 1000000000;
        tmp += time2.tv_nsec - time1.tv_nsec;
	tmp = 1.0 / tmp * 1000000000.0;
	freq += tmp;
	cycles++;
	
	/* save max freq */     
	if (tmp > max) {
            max = tmp;
	}

        /* max cycles reached */
        if (cycles >= 10000) {
	    printf("Max number of cycles reached. End now!\n");
	    options.finishExec = true;
	}
    }
    /* led off */
    if (1 != write(file, LED_OFF, 1)) {
        printf("Write to driver failed!\n");
    }

    /* close driver */
    if (0 != close(file)) {
        printf("Close failed!\n");
    }

    printf("LED off and end.\n");
        
    printf("Cycles     : %ld\n", cycles);
    printf("Freq Avg   : %ld HZ\n", freq / cycles);
    printf("Freq Max   : %ld HZ\n", max);


    printf("End of Programm.\n");
    return EXIT_SUCCESS;
}

static void initUserOptions(userOptions* o)
{
    o->finishExec = false;
}

void signalHandler(int signum)
{
    if (SIGINT == signum) {
        free_ptr->finishExec = true;
    }
}
