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
#include "gpio.h"


/* blink million times */
#define MAX_BLINK            		1000000
#define LED 18
#define LOOP_TIMES 5


#define LED_ON                      "0"
#define LED_OFF                     "1"
#define DRIVER_PATH                 "/dev/mygpio"


void check(int err);
float calculate(struct timespec time_before, struct timespec time_after);
void blink(int file);



int main(void)
{
	int file, led_fd, i;
	float hz;
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
    
	printf("Let the LED blink a million times (5 times)\n");
	
	hz = 0;
	for (i = 0; i < LOOP_TIMES; i++) 
	{
		
		err = clock_gettime(clk_id, &time_before);
		check(err);
		
		blink(file);
		
		err = clock_gettime(clk_id, &time_after);
		check(err);
		
		hz = hz + calculate(time_before, time_after);
	}
	
	printf("Done blinking\n");
	
	hz = hz / LOOP_TIMES;
	
	printf("Average frequenzy for driver is: %.0f Hz\n", hz);

    /* close driver */
    if (0 != close(file)) {
        printf("Closing driver failed!\n");
    }
    
    printf("Testing now sysfs\n");
    
    gpio_export(LED);
	gpio_set_dir(LED, 1);
	led_fd = gpio_fd_open_write(LED);
	
	printf("Blink LED through sysfile\n");
	hz = 0;
	for (i = 0; i < LOOP_TIMES; i++) 
	{
		err = clock_gettime(clk_id, &time_before);
		check(err);
		
		blink(led_fd);
		
		err = clock_gettime(clk_id, &time_after);
		check(err);
		
		hz = hz + calculate(time_before, time_after);
	}
	printf("Done blinking\n");
	
	hz = hz / LOOP_TIMES;
	
	printf("Average frequenzy for sysfs is: %.0f Hz\n", hz);
	
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
	
float
calculate(struct timespec time_before, struct timespec time_after) 
{
	time_t s;
	time_t ns;
	float res;
	
	s = time_after.tv_sec -time_before.tv_sec ;
	ns = time_after.tv_nsec - time_before.tv_nsec ;
	
	res = ns / 10000000000.0;	
	res = s + res;
	res = MAX_BLINK / res;
	
	printf("Frequency: %.0f Hz\n", res);
	return res;

}

void
blink(int file) 
{	
	int i;
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
}
