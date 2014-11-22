#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

// file handle in linux
#include <fcnt1.h>
#include <sys/stat.h>
#include <sys/types.h>

struct thread_info
{
	int *button_pressed;
	struct timespec *sleep_time;
}

void *blink(void *thread_info);
void sigHandler(int);

void main(void)
{
	pthread_t blink_thread;
	signal(SIGINT, sigHandler);
	struct thread_info *thread_struct;
	
	// init thread_struct
	
	// start thread
	if (pthread_create(&blink_thread, NULL, blink, &thread_struct) != 0) 
	{
		fprintf(stderr, "Failed to create thread. Terminating.\n");
		return(-1);
	}
	
	for (;;)
	{
		// read value from switch
		
		if (value == '1')
		{				
			thread_struct->b = 1;
		}
		else
		{
			thread_struct->b = 0;
		}
		
	}
	
}

void *
blink (void *thread_info)
{
	int s, b;
	struct timespec *remain;
	struct timespec *request = thread_info->timespec;
	
	b = thread_info->button_pressed;
	for (;;)
	{
		// blink only when button was pressed
		if (b == 1)
		{
			// led on
		
			s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, &remain);
		
			// led off
		}
	}
} 

void 
sigHandler(int sig)
{
	printf("End led blinking");
	// led off
	
	exit(0);
		
}

