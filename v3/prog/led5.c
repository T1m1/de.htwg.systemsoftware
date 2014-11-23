#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <poll.h>

// file handle in linux
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUTTON 17
#define LED 18
#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

struct thread_info
{
	int *button_pressed;
	struct timespec sleep_time;
};

void *blink(void *thread_info);
void sigHandler(int);
int GPIOExport(int);
int GPIODirection(int, char *);
int GPIOUnexport(int);
char GPIORread(int);

int
main(void)
{
	
	int value;
	int status;
	pthread_t blink_thread;
	struct thread_info *thread_struct = NULL;
	struct timespec sleep_time;
	struct pollfd poll;
	
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
	GPIOExport(BUTTON);
	GPIODirection(BUTTON, "in");
	
	poll.fd = open("/sys/class/gpio/gpio17/value", O_RDONLY);
	poll.events = POLLIN;
	
	for (;;)
	{
		
		if (rv = poll(poll, 1, 20)) == -1)
		{
			// error
		}
		else if (rv.revents && POLLIN)
		{
			
		}
				
		if (value ==1)
		{				
			status = 1;
			sleep(4);
			printf("Setting status on 0\n");
			status = 0;
			sleep(4);
			
		}
		else
		{
			status = 0;
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

int 
GPIOExport(int gpio)
{
	
	int fd;
	char buffer[BUFFER_MAX];
	ssize_t bytes_to_write, bytes_written;
	
	
	fd = open("/sys/class/gpio/export", O_WRONLY);
	
	if (fd == -1)
	{
		fprintf(stderr, "Failes to open export for writing!\n");
		exit(-1);
	}
	
	bytes_to_write = snprintf(buffer, BUFFER_MAX, "%d", gpio);
	bytes_written = write(fd, buffer, bytes_to_write);
	
	if (bytes_written < 0)
	{
		fprintf(stderr, "Failed to write in export!\n");
		exit(-1);
	}
	close(fd);
	
	return 0;
}

int 
GPIODirection(int gpio, char * dir) 
{
	int fd;
	char *path;
	ssize_t bytes_to_write, bytes_written;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		fprintf(stderr, "Failed to open gpio dirction for writing!\n");
		return(-1);
	}
	
	bytes_to_write = strlen(dir);
	bytes_written = write(fd, dir, bytes_to_write);
	
	if(bytes_written < 0)
	{
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
	
	close(fd);
	return 0;
}

int 
GPIOUnexport(int gpio)
{
	char buffer[BUFFER_MAX];
	int fd;
	ssize_t bytes_to_write, bytes_written;
	
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(-1 == fd)
	{
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
	
	bytes_to_write = snprintf(buffer, BUFFER_MAX, "%d", gpio);
	bytes_written = write(fd, buffer, bytes_to_write);
	if(bytes_written < 0)
	{
		fprintf(stderr, "Failed to unexport!\n");
		return(-1);
	}
	close(fd);
	
	return 0;
}

