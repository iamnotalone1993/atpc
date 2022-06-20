
CC=g++
CFLAGS=-O2

all : sequential 

sequential : sequential.cc
	$(CC) $(CFLAGS) -o $@ $+

tgz : sequential.cc Makefile
	tar -cvzf set.tgz sequential.cc Makefile

clean :
	rm -f *.o
	rm -f *~
	rm -f ./sequential
