#Makefile

#*******************************************************
#
#	author: Timotheus Ruprecht / Steffen Gorenflo
#	date: 23:11.2014
#	
#	description: Makefile for compiling all C-Files
#
#
#*********************************************************

#Flag for the Compiler



CROSS_COMPILE=/group/labworks/toolchain/buildroot.git/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-

CC=$(CROSS_COMPILE)gcc

ARCH=arm

#Flag for Compiler-Optionen



CFLAGS=-g -Wall -Werror -static

THREADFLAG=-lpthread

#Flag for the Linker (dynamic by default)
LDFLAGS=-lrt

#Variablen
SOURCES=$(wildcard *.c) 
HEADERS=$(wildcard *.h)
OBJECTS=$(SOURCES:.c=.o)
TARGETS=$(SOURCES:.c=)

.PHONY: all clean nice

#Compiles all C-Files
all:$(TARGETS:gpio=)

#Remove all Files that are created with make
clean:
	$(RM) $(TARGETS) $(OBJECTS) depend

#Format all C-Files
nice:
	indent -gnu -bli0 -i8 *.c


%:%.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(THREADFLAG)



depend: $(SOURCES) $(HEADERS) 
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@ 

-include depend

led5 : led5.c gpio.o
switch : switch.c gpio.o

