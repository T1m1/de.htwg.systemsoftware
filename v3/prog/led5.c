#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
/* file handle in linux */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define BUTTON 17
#define LED 18
#define WEB_BUTTON "/www/switchButton"

struct thread_info
{
	int *button_pressed;
	struct timespec sleep_time;
};

void *blink(void *thread_info);
int set_button(int, char*);
void sigHandler(int);

int done;

int
main(void)
{
	int status;
	int gpio_fd, result;
	int web_button;
	pthread_t blink_thread;
	struct thread_info *thread_struct = NULL;
	struct timespec sleep_time;
	struct pollfd button_poll[2];
	char value[20];	
	
	signal(SIGINT, sigHandler);
	
	/* init time struct */
	sleep_time.tv_sec = 0;
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
	
	
	/* start thread*/
	if (pthread_create(&blink_thread, NULL, blink, thread_struct) != 0) 
	{
		fprintf(stderr, "Failed to create thread. Terminating.\n");
		return(-1);
	}
	
	/* register GPIO */
	gpio_export(BUTTON);
	gpio_set_dir(BUTTON, 0); /* set direction to "in" */
	gpio_set_edge(BUTTON, "falling");
	gpio_fd = gpio_fd_open_read(BUTTON);	
	
	/* open button for web application */
	web_button = open(WEB_BUTTON, O_RDONLY | O_NONBLOCK);
	
	if (web_button < 0) 
	{
		perror("web_button");
		return web_button;
	}
	
	button_poll[0].fd = gpio_fd;
	button_poll[0].events = POLLPRI;
	

	button_poll[1].fd = web_button;
	button_poll[1].events = POLLIN;

	done = 0;
	while (!done)
	{		
		
		result = poll(button_poll, 2, 1000); /* poll on one descriptor with 1 second timeout */
		
		if (result < 0)
		{
			perror("poll failed\n");
			return result;
		}
		
		if (result == 0)
		{
			printf("still polling\n");
		}

		if (button_poll[0].revents > 0)
		{
			printf("Hardware Button:\n");
			read(gpio_fd, value, 20);
			
			/*TODO maybe set seek */
					
			status = set_button(status, value); /*maybe not necessary because of "falling"*/
			
		} 
		else if (button_poll[1].revents & POLLIN)
		{
			printf("Web Button:\n");
			
			read(web_button, value, 20);		
			status = set_button(status, value);
		}
		thread_struct->button_pressed = &status;	
	}
	
	/* wait for thread and free */
	pthread_join(blink_thread, NULL);
	free(thread_struct);
	
	/* close and unexport fd's */
	close(web_button);
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
	int on = 0;
	gpio_export(LED);
	gpio_set_dir(LED, 1);

	led_fd = gpio_fd_open_write(LED);
	
	while(!done)
	{		
		/* blink only when button was pressed */
		if (*b == 1)
		{
			/* led on */
			on = 1;
			write(led_fd, "0", 2);
			printf("AN\n");
			
		
			clock_nanosleep(CLOCK_REALTIME, 0, &request, NULL);
		
			/* led off */
			write(led_fd, "1", 2);
			printf("AUS\n");
			
		}
		else if (on)
		{
			/* turn led off */
			write(led_fd, "1", 2);
			printf("AUS\n");
			on = 0;
		}
	}
	/* turn led off and close fd */
	write(led_fd, "1", 2);
	printf("AUS\n");
	gpio_fd_close(led_fd);
	gpio_unexport(LED);

	return (void*) 0;
} 

void 
sigHandler(int sig)
{
	printf("\nEnd led blinking\n");
	/* led off */
	done = 1;	
}

int 
set_button(int status, char* value)
{
	if (value[0] == '0')
	{
		printf("Button pressed\n");
		printf("Button value is: %s\n", value);
	
	}
	else if (value[0] == '1')
	{
		printf("Button released\n");
		printf("Button value is: %s\n", value);
		if (status == 1)
		{
			status = 0;
		}
		else 
		{
			status = 1;
		}
	}
	
	return status;	

}
