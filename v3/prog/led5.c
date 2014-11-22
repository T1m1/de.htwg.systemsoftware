#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

// file handle in linux
#include <fcnt1.h>
#include <sys/stat.h>
#include <sys/types.h>

void *blink(void *thread_info);
void sigHandler(int);

void main(void)
{
	int threadStarted;
		
	threadStarted = 0;
	signal(SIGINT, sigHandler);
	for (;;)
	{
		// read value from switch
		
		if (value == '1')
		{
			if (threadStarted != 1)
			{
				threadStarted = 1;
				// start thread
			}
		}
		else
		{
			// stop thread
			threadStarted = 0;
		}
		
	}
	
}

void *
blink (void *thread_info)
{
	int s;
	struct timespec *remain;
	struct timespec *request = thread_info->timespec;
	
	for (;;)
	{
		// led on
		
		s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, &remain);
		
		// led off
	}
} 

void 
sigHandler(int sig)
{
	printf("End led blinking");
	// led off
		
}

