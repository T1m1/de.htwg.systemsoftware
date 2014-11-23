#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define GPIO_PIN 17
#define BUF 10
#define VALUE_MAX 30

int main(void)
{
	int fd_gpio;
	int gpio = GPIO_PIN;
	char value;	



	int repeat = 10;

	// Enable GPIO pin

	if (-1 == GPIOExport(gpio))
	{
		return(1);
	}
	printf("gpio export\n");

	

	if(-1 == GPIODirection(gpio))
	{
		return(2);
	}
	printf("gpio direction\n");



	do {

	printf("Test: %d\n", GPIORead(gpio));
	
	usleep(500 * 1000);

	} while (repeat--);
	// TODO loop x times
	
/*	sprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", gpio);
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
	
*/	
	return 0;
}

int GPIOExport(int gpio)
{
	int status = 0;
	int fd;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	
	//fd = open("/sys/class/gpio/export", O_WRONLY);
	fd = open("/home/tiruprec/gpio/export", O_WRONLY);	
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

	//snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", gpio);
	snprintf(path, VALUE_MAX, "/home/tiruprec/gpio%d/value", gpio);
	fd = open(path, O_RDONLY);
	if (-1 == fd) 
	{
		fprintf(stderr, "Failed to open gpio value for reding!\n");
		return(-1);
	}

	if (1 != read(fd, s_value, 3))
	{
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);
	return (atoi(s_value));;
}

