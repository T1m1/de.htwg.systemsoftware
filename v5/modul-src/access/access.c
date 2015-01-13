#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#define THREAD_COUNT 10

void *write_test(void *thread_info);
void *read_test(void *thread_info);
void verbPrintf (int verbosity, char *format, ...);

int file;

struct thread_info
{
	int id;
};

int main(int argc, char *argv[])
{
	int numberOfThreads, i;
	
	/* threads */
	pthread_t *threads;
	struct thread_info *thread_struct = NULL;
	
	numberOfThreads = THREAD_COUNT;
	
	threads = malloc(numberOfThreads * sizeof(pthread_t));
	thread_struct = malloc(numberOfThreads * sizeof (struct thread_info));
	
	/* check memory allocation */
	if (thread_struct == NULL || threads == NULL){
		fprintf(stderr, "Failed to allocate memory!\n");
		perror ("malloc");
        exit (EXIT_FAILURE);
	}

	file = open("/dev/buf", O_RDWR);
	if (file < 0)
	{
		perror("module device /dev/buf");
		return file;
	}
	
	srand(time(NULL));
	
	/* start threads */
	for(i = 0; i < numberOfThreads; i++) {
		thread_struct[i].id = i;		
		if (i%2) {
			pthread_create(&threads[i], NULL, write_test, (void *) &thread_struct[i]);
		}
		else 
		{
			pthread_create(&threads[i], NULL, read_test, (void *) &thread_struct[i]);
		}
	}	
	
	for(i = 0; i < numberOfThreads; i++) {
		printf("join thread number %d", i);
		pthread_join(threads[i], NULL);
	}
	
	free(threads);
	free(thread_struct);
	
	return 0;
	
	
	
}

void *
write_test(void *thread_info) 
{
	char* buf = "\nThread writes: hello\n";
	int r, i;

	for(i = 0; i < 5; i++) 
	{
		r = rand() % 2;
		write(file, buf, 22);
		sleep(r);
	}
	return (void*) 0;
}


void *
read_test(void *thread_info) 
{
	char buf[32];
	struct thread_info *t = (struct thread_info *) thread_info;
	int r, i;

	for(i = 0; i < 5; i++) 
	{
		r = rand() % 2;
		read(file, buf, 32);
		printf("Thread nr %d received data: %s\n", t->id, buf);
		sleep(r);
	}
	return (void*) 0;
}




void
verbPrintf (int verbosity, char *format, ...)
{
	va_list args;
	va_start (args, format);
	if (verbosity)
	{
			vfprintf (stdout, format, args);
	}
}

