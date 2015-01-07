#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* blink every 250 milliseconds */
#define BLINK_SPEED_IN_HZ            250.0

#define LED_ON                      "0"
#define LED_OFF                     "1"

/* fix path for mygpio driver */
#define DRIVER_PATH                 "/dev/mygpio"

typedef struct {
    bool finish;
    bool enabled;
} opt;
static opt *free_ptr = NULL;
static void* thread_code(void*);
void signalHandler(int);

int main(void)
{
    opt options;
    pthread_t thread;
    
    /* init */
    options.finish = false;
    options.enabled = false;

    free_ptr = &options;

    /* register signal handler */
    signal(SIGINT, signalHandler);

    printf("Create Thread.\n");
    if (0 != pthread_create(&thread, NULL, thread_code, (void*) &options)) {
        fprintf(stderr, "Error while creating thread! Exiting!\n");
        exit(EXIT_FAILURE);
    }

    printf("Join Thread to wait until the thread is closed.\n");
    if (0 != pthread_join(thread, NULL)) {
        fprintf(stderr, "Error while joining thread! Exiting!\n");
        exit(EXIT_FAILURE);
    }
    printf("End of Programm.\n");
    return EXIT_SUCCESS;
}

/* thread */
static void* thread_code(void* arg)
{
    int file;
    opt *options = (opt*) arg;
    struct timespec time;
    
    /* set blink speed */
    time.tv_sec = 0;
    time.tv_nsec = 1 / BLINK_SPEED_IN_HZ * 100000000;

	/* open driver */
    file = open(DRIVER_PATH, O_WRONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
        return (void*) 1;
    }

    while(!options->finish) {
        clock_nanosleep(CLOCK_MONOTONIC, 0, &time, NULL);

        /* led off */
        if (1 != write(file, LED_OFF, 1)) {
            printf("Write to driver failed!\n");
            break;
        }

		/* sleep */
        clock_nanosleep(CLOCK_MONOTONIC, 0, &time, NULL);

        /* led on */
        if (1 != write(file, LED_ON, 1)) {
            printf("Write to driver failed!\n");
            break;
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
    return (void*)NULL;
}

void signalHandler(int signum)
{
    if (SIGINT == signum) {
        free_ptr->finish = true;
    }
}
