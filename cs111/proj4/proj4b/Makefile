#!bin/bash
# NAME:Andrew Ding
# EMAIL:andrewxding@ucla.edu


CC = gcc
CFLAGS = -Wall -Wextra -lmraa -lm -pthread -g -o 
.SILENT:

default:
	$(CC) $(CFLAGS) lab4b lab4b.c 

check: default
	bash check.sh

clean:
	rm -f lab4b lab4b-504748356.tar.gz
  
dist: default
	tar -zcvf lab4b-504748356.tar.gz lab4b.c README Makefile check.sh




