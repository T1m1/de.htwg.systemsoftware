#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>


#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define GPIO_PIN 17
#define MAX_BUF 20
#define WEB_BUTTON "/www/switchButton"

void sigHandler(int);
int done;

int main(void)
{
	int gpio_fd, web_button;
	int result;
	int gpio = GPIO_PIN;
	unsigned int oldValue, newValue;
	int count = 0;
	char value[MAX_BUF];
	
	struct pollfd button_poll[2];

	signal(SIGINT, sigHandler);
	
	/* register GPIO */
	gpio_export(GPIO_PIN);
	gpio_set_dir(GPIO_PIN, 1); /* set direction to "in" */
	gpio_set_edge(GPIO_PIN, "falling");
	gpio_fd = gpio_fd_open_read(GPIO_PIN);	
	
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

		if (button_poll[0].revents & POLLPRI)
		{
			printf("Hardware Button:\n");
			read(button_poll[0].fd, value, MAX_BUF);
			
			if (lseek(gpio_fd, 0, SEEK_SET) == -1)
			{
				perror("lseek");
				return -1;
			}
			count++;
		} 
		else if (button_poll[1].revents & POLLIN)
		{
			/* retrieve value and write it in newValue */
			gpio_get_value(gpio, &newValue);

			/* check if switch is pressed */
			if(newValue > oldValue)
			{
			count++;
			} 
			printf("value: %d, count: %d\n", newValue, count);
			oldValue = newValue;

		}

	}
	
	printf("Total count of pressed button: %d \n", count);
	return 0;
}

void sigHandler(int not_used)
{
	printf("\nEnding Programm\n");
	done = 1;
}

