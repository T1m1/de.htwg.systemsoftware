#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


/* blink thousand times */
#define MAX_BLINK            		1000


#define LED_ON                      "0"
#define LED_OFF                     "1"
#define DRIVER_PATH                 "/dev/mygpio"


void check(int err);
ulong calculate(struct timespec time_before, struct timespec time_after);



int main(void)
{
	int file, i;
	ulong hz;
	int err;
	struct timespec time_before, time_after;
	clockid_t clk_id;
	
	printf("Check clock id\n");
	clk_id = CLOCK_REALTIME;
	if (clock_gettime(CLOCK_REALTIME, &time_before) != 0)
	{
		if (errno == EINVAL)
		{
			printf("Realtime not supported, switching to monotonic\n");
			clk_id = CLOCK_MONOTONIC;
			errno = 0;
		}
		else
		{
			printf("Other Failure occured\n");
			exit(EXIT_FAILURE);
		}
		
	}

	/* open driver */
	printf("Open driver\n");
    file = open(DRIVER_PATH, O_WRONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
         exit(EXIT_FAILURE);
    }
    
	printf("Let the LED blink thousand times\n");
	
	err = clock_gettime(clk_id, &time_before);
	check(err);
	for(i = 0; i < MAX_BLINK; i++) 
	{

        /* led off */
        if (1 != write(file, LED_OFF, 1)) 
        {
            printf("Switch off failed during %d!\n", i);
            break;
        }

        /* led on */
        if (1 != write(file, LED_ON, 1)) 
        {
            printf("Switch on failed during %d!\n", i);
            break;
        }

    }
    err = clock_gettime(clk_id, &time_after);
	check(err);
	printf("Done blinking\n");
	
	/* led off */
    if (1 != write(file, LED_OFF, 1)) {
        printf("Switch off driver failed after loop!\n");
    }
	
	hz = calculate(time_before, time_after);
	
	printf("Time before: %ld\n", time_before.tv_sec);
	printf("Time after: %ld\n", time_after.tv_sec);
	printf("The calculated max frequency is: %lu\n", hz);



    /* close driver */
    if (0 != close(file)) {
        printf("Closing driver failed!\n");
    }

    printf("End of Programm.\n");
    return EXIT_SUCCESS;
}


void
check(int err)
{
	if (err == -1) {
		printf("Error while getting time\n");
		exit(EXIT_FAILURE);
	}
}
	
ulong
calculate(struct timespec time_before, struct timespec time_after) 
{
	ulong s;
	ulong ns;
	ulong res;
	
	s = time_before.tv_sec - time_after.tv_sec;
	ns = time_before.tv_nsec - time_after.tv_nsec;
	
	res = s + (ns / 1000000000.0);
	
	printf("Duration: %lu\n", res);
	
	res = MAX_BLINK / res;
	
	return res;

}
