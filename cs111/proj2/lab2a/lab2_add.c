/*
NAME: Andrew Ding
EMAIL: andrewxding@ucla.edu	
ID: 504748356
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>


int opt_yield;
pthread_mutex_t lock;
char opt_sync;
int synclock = 0;
int numthreads, numiterations;
//no protection add, this whole thing is a critical section
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}
//adding with mutex lock
void add_lock(long long *pointer, long long value) {
	pthread_mutex_lock(&lock);
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
    pthread_mutex_unlock(&lock);
}
//atomic add
void add_atomic(long long *pointer, long long value) {
	long long prev, sum;
	do {
		prev = *pointer;
		sum = prev + value;
		if(opt_yield)
			sched_yield();
	} while(__sync_val_compare_and_swap(pointer, prev, sum) != prev);
}
//spinlock add
void add_spin(long long *pointer, long long value) {
	while(__sync_lock_test_and_set(&synclock, 1));
	long long sum = *pointer + value;
	if (opt_yield)
    	sched_yield();
	*pointer = sum;
	__sync_lock_release(&synclock);
}
//thread function
void *func(void* ptr){
	long long *pointer = (long long*)ptr;
	int i;
	for(i = 0; i < numiterations; i ++){
		if(opt_sync == 'm'){
			add_lock(pointer, 1);
		}
		else if (opt_sync == 's'){
			add_spin(pointer, 1);
		}
		else if (opt_sync == 'c') {
			add_atomic(pointer, 1);
		}
		else {
			add(pointer, 1);
		}
	}
	
	for(i = 0; i < numiterations; i ++){
		if(opt_sync == 'm'){
			add_lock(pointer, -1);
		}
		else if (opt_sync == 's'){
			add_spin(pointer, -1);
		}
		else if (opt_sync == 'c') {
			add_atomic(pointer, -1);
		}
		else {
			add(pointer, -1);
		}
	}
	
	return NULL;
}

int main(int argc, char** argv)
{
	int opt;
	numiterations = 1;
	numthreads = 1;
	long long counter = 0;

	struct timespec start, end;
	static struct option long_opts[] =
	{
		{"sync", required_argument, NULL, 's'},
		{"yield", no_argument, NULL, 'y'},
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{0, 0, 0, 0}
	};

	while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 't':
				numthreads = atoi(optarg);
				if(numthreads < 1){
					fprintf(stderr, "Invalid number of threads\n");
					exit(1);
				}
				break;
			case 'i':
				numiterations = atoi(optarg);
				if(numiterations < 1){
					fprintf(stderr, "Invalid number of iterations\n");
					exit(1);
				}
				break;
			case 'y':
				opt_yield = 1;
				break;
			case 's':
				opt_sync = optarg[0];//add arg checker for ok values
				if(opt_sync != 'm' && opt_sync != 's' && opt_sync != 'c')
				{
					fprintf(stderr, "Error:--sync=m, s, or c.\n");
					exit(1);
				}
				break;
			default:
				fprintf(stderr, "correct usage: ./lab2_add --iterations= --threads= --yield --sync=m, s, or c.\r\n");
				exit(1); //Exit with status of failure
		}
	}

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
    //get starting time
	if (clock_gettime(CLOCK_MONOTONIC, &start)) {   
		fprintf(stderr, "Error with clock_gettime()\n");
		exit(1);
	}

	//create the threads
	pthread_t threads[numthreads];
	int i;
	for (i =0; i < numthreads; i ++){
		int ret = pthread_create(&threads[i], NULL, func, &counter);
		if (ret){
			fprintf(stderr, "Error creating theads %d", ret);
			exit(1);
		}
	}
	//wait for all threads to exit 
	for (i =0; i < numthreads; i ++){
		int ret = pthread_join(threads[i], NULL);
		if(ret){
		  fprintf(stderr, "Error freeing threads. Error code %d\n", ret);
		  exit(1);
		}
	}

	//finish time
	if (clock_gettime(CLOCK_MONOTONIC, &end)) {   // check to see if there was an error 
		fprintf(stderr, "Error with clock_gettime()\n");
		exit(1);
	}

	long long totalTime =(end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
	int ops = 2 * numthreads * numiterations;

	printf("add");
	if(opt_yield) printf("-yield");
	if(opt_sync == 'm') printf("-m");
	if(opt_sync == 's') printf("-s");
	if(opt_sync == 'c') printf("-c");
	if(!opt_sync) printf("-none");
	//csv print out
	printf(",%d,%d,%d,%lld,%lld,%llu\n", numthreads, numiterations, ops,totalTime, totalTime/ops, counter);
	if(counter)
		exit(1);
	exit(0);
}