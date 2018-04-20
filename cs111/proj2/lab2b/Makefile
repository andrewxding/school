#NAME: Andrew Ding
#EMAIL: andrewxding@ucla.edu	
#ID: 504748356

CC=gcc
CFLAGS=-pthread -Wall -Wextra -g -lprofiler
.SILENT: clean dist

build: lab2_list.c
	$(CC) $(CFLAGS) -o lab2_list lab2_list.c SortedList.c

tests:
	rm -f *.csv
	bash sample.sh

graphs:
	chmod +x lab2b.gp
	./lab2b.gp

dist: build tests graphs profile
	rm -rf lab2b-504748356.tar.gz
	tar -czf lab2b-504748356.tar.gz SortedList.h SortedList.c lab2_list.c  Makefile lab2b_list.csv profile.out *.png sample.sh lab2b.gp README 
profile:
	CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=threadFunc ./lab2_list ./raw.gperf >> profile.out

clean: 
	rm -rf lab2_list lab2b-504748356.tar.gz raw.gperf
