#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define BUTTON 17
#define LED 18

struct thread_info
{
	int *button_pressed;
	struct timespec sleep_time;
};

void *blink(void *thread_info);
void sigHandler(int);
int done;

int
main(void)
{
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
	if (thread_struct == NULL)
	{
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
	gpio_fd = gpio_fd_open_read(BUTTON);	
	
	
	button_poll.fd = gpio_fd;
	button_poll.events = POLLPRI;

	done = 0; 
	while (!done)
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

		if (button_poll.revents & POLLPRI)
		{
			printf("Button pressed\n");
			char value[20];
			read(gpio_fd, value, 2);
			
			printf("%s\n",value);
			printf("Button value is: %d\n", status);
		}
		
		thread_struct->button_pressed = &status;	
	}
	

	pthread_join(blink_thread, NULL);
	free(thread_struct);
	gpio_fd_close(gpio_fd);
	gpio_unexport(BUTTON);
	return 0;	
	
}

void *
blink (void *thread_info)
{
	int led_fd;
	struct thread_info *t = (struct thread_info *) thread_info;
	
	struct timespec request = t->sleep_time;
	int *b = t->button_pressed;
	
	gpio_export(LED);
	gpio_set_dir(LED, 1);

	led_fd = gpio_fd_open_write(LED);
	
	while(!done)
	{
		// blink only when button was pressed
		if (*b == 1)
		{
			write(led_fd, "0", 2);
			printf("AN\n");
		
			clock_nanosleep(CLOCK_REALTIME, 0, &request, NULL);
		
			// led off
			write(led_fd, "1", 2);
			printf("AUS\n");
		}
		else 
		{
			// turn led off
			write(led_fd, "1", 2);
		}
	}
	
	gpio_fd_close(led_fd);
	gpio_unexport(LED);

	return (void*) 0;
} 

void 
sigHandler(int sig)
{
	printf("\nEnd led blinking\n");
	// led off
	done = 1;	
}
