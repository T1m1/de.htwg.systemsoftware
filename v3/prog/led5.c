#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>

// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define BUTTON 17
#define LED 18
#define BUFFER_MAX 64
#define DIRECTION_MAX 35
#define VALUE_MAX 30

struct thread_info
{
	int *button_pressed;
	struct timespec sleep_time;
};

void *blink(void *thread_info);
void sigHandler(int);

int
main(void)
{
	
	int value;
	int status;
	int gpio_fd, result;
	pthread_t blink_thread;
	struct thread_info *thread_struct = NULL;
	struct timespec sleep_time;
	struct pollfd button_poll;
	
	signal(SIGINT, sigHandler);
	
	// init time struct
	sleep_time.tv_sec = 1;
	sleep_time.tv_nsec = 200000000;
	status = 0;
	
	thread_struct = malloc(sizeof (struct thread_info));
	if (thread_struct == NULL) {
		fprintf(stderr, "Failed to allocate memory!\n");
		return(-1);
	}
	
	thread_struct->sleep_time = sleep_time;
	thread_struct->button_pressed = &status;	
	
	
	// start thread
	if (pthread_create(&blink_thread, NULL, blink, thread_struct) != 0) 
	{
		fprintf(stderr, "Failed to create thread. Terminating.\n");
		return(-1);
	}
	
	// register GPIO
	gpio_export(BUTTON);
	gpio_set_dir(BUTTON, 0); // set direction to "in"
	gpio_set_edge(BUTTON, "falling");
	gpio_fd = gpio_fd_open(BUTTON);	
	
	
	button_poll.fd = gpio_fd;
	button_poll.events = POLLIN;
	
	for (;;)
	{		
		
		result = poll(&button_poll, 1, 1000); // poll on one descriptor with 1 second timeout
		
		if (result < 0)
		{
			perror("poll failed\n");
			return result;
		}
		
		if (result == 0)
		{
			printf("still polling\n");
		}

		if (button_poll.revents && POLLIN)
		{
			printf("Button pressed\n");
			if (status)
			{
				status = 0;
			}
			else
			{
				status = 1;
			}
		}
		
		thread_struct->button_pressed = &status;	
	}
	
	
}

void *
blink (void *thread_info)
{
	struct thread_info *t = (struct thread_info *) thread_info;
	
	struct timespec request = t->sleep_time;
	int *b = t->button_pressed;
	
	for (;;)
	{
		// blink only when button was pressed
		if (*b == 1)
		{
			// led on
			printf("AN\n");
		
			clock_nanosleep(CLOCK_REALTIME, 0, &request, NULL);
		
			// led off
			printf("AUS\n");
		}
	}
} 

void 
sigHandler(int sig)
{
	printf("\nEnd led blinking\n");
	// led off
	
	//TODO free stuff
	exit(0);
		
}


