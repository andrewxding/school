#!bin/bash
# NAME:Andrew Ding
# EMAIL:andrewxding@ucla.edu
# ID:504748356

CC = gcc
CFLAGS = -Wall -Wextra -g -o
.SILENT: 

default:
	$(CC) $(CFLAGS) lab0 lab0.c
check: default
	# check if it can handle --input and --output
	echo "this is my test text" > in.txt
	./lab0 --input=in.txt --output=out.txt; \
	if [ $$? -eq 0 ] ; then \
		echo "Correct handling of --input and --output."; \
	else \
		echo "Error reported with --input and --output."; \
	fi
	# check to see if correctly copies input to output
	diff in.txt out.txt
	if [ $$? -eq 0 ] ; then \
		echo "Correctly copied from input to output."; \
	else \
		echo "Failed copying from input to output."; \
	fi
	#check to see if segfault and catch work properly
	./lab0 --catch --segfault &>/dev/null; \
	if [[ $$? -ne 4 ]] ; then \
		echo "Error: received SIGSEGV but signal wasn't caught correctly."; \
	else \
		echo "Succesfully caught SIGSEGV"; \
	fi
	rm -f in.txt out.txt 
clean:
	rm -f lab0 lab0-504748356.tar.gz
dist:
	tar -czvf lab0-504748356.tar.gz README Makefile lab0.c breakpoint.png backtrace.png
