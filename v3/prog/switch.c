#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define GPIO_PIN 18
#define BUF 10
#define VALUE_MAX 30

void main(void)
{
	int fd_gpio;
	int gpio = GPIO_PIN;
	char buffer[BUFFER_MAX];
	char value;	
	ssize_t bytes_written;
	char path[DIRECTION_MAX];
	
	
	// TODO loop x times
	
	sprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", gpio);
	fd_gpio=open(path, O_RDONLY);
	
	if(-1 == fd_gpio)
	{
			fprintf(stderr, "Failed to open gpio value for reading!\n");
			return(-1);
	}
	
	if( -1 == read(fd_gpio, value))
	{
			fprintf(stderr, "Failed to read value!\n");
			return(-1);
	}

	if(value == '0')
	{
		// aus
	}
	else
	{
		// an
	}

	close(fd);
	
	
	return 0;
}

int GPIOExport(int gpio)
{
	int status = 0;
	int fd;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	
	fd = open("/sys/class/gpio/export", O_WRONLY);
	
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

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(path, O_WRONLY);
	if (-1 == fd_gpio)
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
	int status = 0;
	int fd;
	
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(-1 == fd)
	{
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
	
	bytes_written = snprintf(bugger, BUGGER_MAX, "%d", gpio);
	write(fd, bugger, bytes_written);
	close(fd);
	return(status);
}
