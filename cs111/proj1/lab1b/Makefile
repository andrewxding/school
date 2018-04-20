#!bin/bash
CC = gcc
CFLAGS = -Wall -Wextra -g -o
.SILENT: 

all: client server

client:
	$(CC) $(CFLAGS) lab1b-client lab1b-client.c -lpthread -lmcrypt

server: 
	$(CC) $(CFLAGS) lab1b-server lab1b-server.c -lpthread -lmcrypt

clean:
	rm -f lab1b-client lab1b-server lab1b-504748356.tar.gz

dist:
	tar -cvzf lab1b-504748356.tar.gz *.c Makefile my.key README 
