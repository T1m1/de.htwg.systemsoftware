#include <stdio.h>
#include <unistd.h>
// file handle in linux
#include <fcnt1.h>

#define GPIO_PIN 18
#define BUF 10

void main(void)
{
	int fd_gpio;
	int gpio = GPIO_PIN;
	char buf[BUF];
	char value;	

	sprintf(buf, "sys/class/gpio/gpio%d/value", gpio);
	fd_gpio=open(buf, O_RDONLY);
	
	read(fd_gpio, &value, 1);

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
