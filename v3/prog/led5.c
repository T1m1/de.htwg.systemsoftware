#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

// file handle in linux
//#include <fcnt1.h>
//#include <sys/stat.h>
//#include <sys/types.h>

struct thread_info
{
	int *button_pressed;
	struct timespec sleep_time;
};

void *blink(void *thread_info);
void sigHandler(int);

int
main(void)
{
	
	int value;
	int status;
	pthread_t blink_thread;
	struct thread_info *thread_struct = NULL;
	struct timespec sleep_time;
	
	signal(SIGINT, sigHandler);
	
	// init thread_struct
	sleep_time.tv_sec = 1;
	sleep_time.tv_nsec = 200000000;
	status = 1;
	
	thread_struct = malloc(sizeof (struct thread_info));
	//TODO fehlerbehandlung
	thread_struct->sleep_time = sleep_time;
	thread_struct->button_pressed = &status;	
	
	
	// start thread
	if (pthread_create(&blink_thread, NULL, blink, thread_struct) != 0) 
	{
		fprintf(stderr, "Failed to create thread. Terminating.\n");
		return(-1);
	}
	
	
	value = 1;
	for (;;)
	{
		// read value from switch
		
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
	
	int s;
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

