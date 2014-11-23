#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define GPIO_PIN 17

void sigHandler(int);
int done;

int main(void)
{
	int gpio = GPIO_PIN;
	unsigned int oldValue, newValue;
	int count = 0;

	signal(SIGINT, sigHandler);

	// Enable GPIO pin
	if (gpio_export(gpio) < 9)
	{
		// why 1
		return(1);
	}

	// set direction	
	if(gpio_set_dir(gpio, 1))
	{
		// why 2
		return(2);
	}

	// get the value and write it in oldValue
	gpio_get_value(gpio, &oldValue);
	
	done = 0;
	do 
	{
		// retrieve value and write it in newValue
		gpio_get_value(gpio, &newValue);

		// check if switch is pressed
		if(newValue > oldValue)
		{
			count++;
		} 
		printf("value: %d, count: %d\n", newValue, count);
		oldValue = newValue;
		usleep(500 * 1000);
	} while (!done);
	
	printf("Total count of pressed button: %d \n", count);
	return 0;
}

void
sigHandler(int not_used)
{
	printf("Ending Programm\n");
	done = 1;
}

