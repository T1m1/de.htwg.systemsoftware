#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* blink thousand times */
#define MAX_BLINK            		1000

#define LED_ON                      "0"
#define LED_OFF                     "1"

/* fix path for mygpio driver */
#define DRIVER_PATH                 "/dev/mygpio"

void check(int err);
int calculate(struct timespec time_before, struct timespec time_after);

int main(void)
{
	int file, i, hz;
	int err;
	struct timespec time_before, time_after;
	
	
	/* open driver */
    file = open(DRIVER_PATH, O_WRONLY);
    if( file < 0 ) {
        printf("Open driver failed.\n");
         exit(EXIT_FAILURE);
    }

	err = clock_gettime(CLOCK_REALTIME, &time_before);
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
    err = clock_gettime(CLOCK_REALTIME, &time_after);
	check(err);
	
	hz = calculate(time_before, time_after);
	
	printf("The calculated max frequency is: %d\n", hz);

    /* led off */
    if (1 != write(file, LED_OFF, 1)) {
        printf("Switch off driver failed after loop!\n");
    }

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

int
calculate(struct timespec time_before, struct timespec time_after) 
{
	int s;
	/*int ns;*/
	int res;
	
	s = time_before.tv_sec - time_after.tv_sec;
	/*ns = time_before.tv_nsec - time_after.tv_nsec;*/
	
	res = MAX_BLINK / s;
	
	return res;
}
