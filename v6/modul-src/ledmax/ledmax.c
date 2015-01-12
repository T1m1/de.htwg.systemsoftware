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

#define MAX_SPEED	6
float blinkSpeed[] = { 5.0, 10.0, 15.0, 20.0, 25.0, 30.0};

typedef struct {
    bool finish;
     int speed;
} opt;
static opt *free_ptr = NULL;
static void* thread_code(void*);
void signalHandler(int);

int main(void)
{
    opt options;
    pthread_t thread;
    int file;
    char last = '0';
    char actual;

    /* init */
    options.finish = false;
    options.speed = 0;

    free_ptr = &options;

    /* register signal handler */
    signal(SIGINT, signalHandler);

    file = open(DRIVER_PATH, O_RDONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Create Thread.\n");
    if (0 != pthread_create(&thread, NULL, thread_code, (void*) &options)) {
        fprintf(stderr, "Error while creating thread! Exiting!\n");
        /* close driver */
        if (0 != close(file)) {
            printf("Close failed!\n");
        }
        exit(EXIT_FAILURE);
    }
    
    /* schalter initial abfragen */
    if (1 != read(file, &actual, 1)) {
		options.finish = true;
		printf("Read from driver failed!\n");
    }
    last = actual;
    /* schalter abfragen*/
    while(!options.finish) {
        /* led off */
        if (1 != read(file, &actual, 1)) {
            options.finish = true;
            printf("Read from driver failed!\n");
	    break;
        }
        if (last != actual) {
            if ('1' == actual) {
                options.speed = (options.speed >= MAX_SPEED) ? 0 : options.speed + 1;
                printf("Blink now with %fs HZ.\n", blinkSpeed[options.speed]);
            }
            last = actual;
        }
    }
    

    printf("Join Thread to wait until the thread is closed.\n");
    if (0 != pthread_join(thread, NULL)) {
        fprintf(stderr, "Error while joining thread! Exiting!\n");
        /* close driver */
        if (0 != close(file)) {
            printf("Close failed!\n");
        }
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
   
	/* open driver */
    file = open(DRIVER_PATH, O_WRONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
        return (void*) 1;
    }

    while(!options->finish) {
		
		/* set new blink speed */
		time.tv_sec = 0;
		time.tv_nsec = 1.0 / blinkSpeed[options->speed] * 100000000;
    
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
