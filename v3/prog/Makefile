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
#TODO insert cross compiler
CC=clang

#Flag for Compiler-Optionen
CFLAGS=
THREADFLAG=-lpthread
REALTIMEFLAG=-lrt

#Flag for the Linker (dynamic by default)
LDFLAGS=-lm

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
	$(CC) $(CFLAGS) $(LDFLAGS) $(THREADFLAG) $(REALTIMEFLAG) $^ -o $@

depend: $(SOURCES) $(HEADERS) 
	$(CC) $(CFLAGS) -MM $(SOURCES) > $@ 

-include depend

led5 : led5.c gpio.o
switch : switch.c cpio.o

