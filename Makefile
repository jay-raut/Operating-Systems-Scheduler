# makefile for scheduling program        
CC=gcc
DEPS = scheduler.h list.h process.h
OBJ = scheduler.o list.o
CFLAGS=-Wall
STD_FLAG=-std=c99
LIBS = -lm

scheduler.o: scheduler.c scheduler.h 
	$(CC) $(CFLAGS) scheduler.c $(STD_FLAG)  -o scheduler $(LIBS)

list.o: list.c list.h process.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -rf *.o    

