#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>


int main (int argc, char *argv[])
{
	struct sysinfo sys_info;
	
	if(sysinfo(&sys_info) != 0) 
	{
		perror("sysinfo");
	}
	
	printf("\nHello User world");
	printf("\nUptime: %ld:%ld:%ld", sys_info.uptime/3600, 
								sys_info.uptime%3600/60, sys_info.uptime%60);
	printf("\nTotal RAM: %ldMB", (sys_info.totalram / 1024) / 1024 );
	printf("\nFree Ram: %ldMB", (sys_info.freeram / 1024) / 1024);
	printf("\nProcess count: %d", sys_info.procs);
	printf("\nPage size: %ld Bytes", getpagesize());
	printf("\n");
	
	return 0;
}
