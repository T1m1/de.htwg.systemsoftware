#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gpio.h"

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define GPIO_PIN 17
#define BUF 10
#define VALUE_MAX 30

int main(void)
{
	int gpio = GPIO_PIN;
	unsigned int oldValue, newValue;
	int count = 0;

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
	} while (1);
	
	return 0;
}

int GPIOExport(int gpio)
{
	int status = 0;
	int fd;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	
	fd = open("/sys/class/gpio/export", O_WRONLY);
	// test fd = open("/home/tiruprec/gpio/export", O_WRONLY);	
	if (-1 == fd)
	{
		fprintf(stderr, "Failes to open exort for writing!\n");
		return(-1);
	}
	
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", gpio);
	write(fd, buffer, bytes_written);
	close(fd);
	
	return status;
}

int GPIODirection(int gpio) {
	
	int status = 0;
	int fd;
	char path[DIRECTION_MAX];

	//snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", gpio);
	snprintf(path, DIRECTION_MAX, "/home/tiruprec/gpio%d/direction", gpio);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		fprintf(stderr, "Failed to open gpio dirction for writing!\n");
		return(-1);
	}
	
	if(-1 == write(fd, "in", 2))
	{
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
	
	close(fd);
	return status;
}

int GPIOUnexport(int gpio)
{
	int fd;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(-1 == fd)
	{
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
	
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", gpio);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

int GPIORead(int gpio)
{
	char s_value[3];
	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", gpio);
	// test snprintf(path, VALUE_MAX, "/home/tiruprec/gpio%d/value2", gpio);
	fd = open(path, O_RDONLY);
	if (-1 == fd) 
	{
		fprintf(stderr, "Failed to open gpio value for reding!\n");
		return(-1);
	}

	if (-1 == read(fd, s_value, 3))
	{
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);
	return (atoi(s_value));;
}

