CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Werror
OPTFLAG = -O2
DEBUG_FLAGS = -g -DDEBUG

PROGRAM = lc3sim

all: build
build: $(PROGRAM)

$(PROGRAM): lc3sim.o lc3.o
	gcc lc3sim.o lc3.o -o $(PROGRAM)
	
lc3sim.o : lc3sim.c lc3.h
	gcc $(CFLAGS) $(OPTFLAG) -c lc3sim.c
	
lc3.o : lc3.h lc3.c
	gcc $(CFLAGS) $(OPTFLAG) -c lc3.c

clean:
	rm -f *.o $(PROGRAM)

.PHONY: all build clean
