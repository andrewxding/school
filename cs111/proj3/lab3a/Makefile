# NAME: Caleb Chau, Andrew Ding
# EMAIL: caleb.h.chau@gmail.com, andrewxding@ucla.edu
# ID: 204805602, 504748356

# Variables for files and file names
PROJECT_FILES = lab3a.c README Makefile ext2_fs.h
PROJECT_NAME = lab3a
STUDENT_ID = 204805602

# Variable for compiler flags
CFLAGS = -g -Wall -Wextra -std=gnu11

default:
	gcc $(CFLAGS) -o $(PROJECT_NAME) lab3a.c

clean:
	rm -f $(PROJECT_NAME) $(PROJECT_NAME)-$(STUDENT_ID).tar.gz

dist: default
	tar -czf $(PROJECT_NAME)-$(STUDENT_ID).tar.gz $(PROJECT_FILES)

