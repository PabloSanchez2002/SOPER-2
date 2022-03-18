
CC = gcc
CFLAGS =  -pedantic -g -Wall
all: voting

voting: main.o candidato.o votante.o
	$(CC) -pthread -o voting main.o candidato.o votante.o
	
candidato.o: candidato.c candidato.h inc.h
	$(CC) -c $(CFLAGS) $<

main.o: main.c candidato.h inc.h
	$(CC) -c $(CFLAGS) $<


votante.o: votante.c votante.h inc.h
	$(CC) -c $(CFLAGS) $<


.PHONY: clean

clean: 
	rm *.o voting

