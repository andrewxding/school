#NAME: Andrew Ding
#EMAIL: andrewxding@ucla.edu	
#ID: 504748356

CC=gcc
CFLAGS=-pthread -Wall -Wextra

build: lab2_add.c lab2_list.c
	$(CC) $(CFLAGS) -o lab2_add lab2_add.c
	$(CC) $(CFLAGS) -o lab2_list lab2_list.c SortedList.c

tests:
	bash sample.sh

graphs:
	chmod +x lab2_add.gp
	chmod +x lab2_list.gp
	./lab2_add.gp 
	./lab2_list.gp 

dist:
	rm -rf lab2a-504748356.tar.gz
	tar -czf lab2a-504748356.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_add.csv lab2_list.csv Makefile README sample.sh lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp

clean: 
	rm -rf lab2_add lab2_list lab2a-504748356.tar.gz
