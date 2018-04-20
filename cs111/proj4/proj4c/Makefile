#!bin/bash
# NAME:Andrew Ding
# EMAIL:andrewxding@ucla.edu


CC = gcc
# CFLAGS = -Wall -Wextra -pthread -g -o 
CFLAGS1 = -Wall -Wextra -lmraa -lm -pthread -g -o 
CFLAGS2 = -Wall -Wextra -lmraa -lm -pthread -lssl -lcrypto -g -o
.SILENT:

default:
	$(CC) $(CFLAGS1) lab4c_tcp lab4c_tcp.c 
	$(CC) $(CFLAGS2) lab4c_tls lab4c_tls.c 

clean:
	rm -f lab4c_tcp lab4c_tls lab4b-504748356.tar.gz
  
dist: default
	tar -zcvf lab4b-504748356.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile




