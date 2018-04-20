/*
NAME: Andrew Ding
EMAIL: andrewxding@ucla.edu	
ID: 504748356
*/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include "SortedList.h"

int numElems;
int numthreads = 1;
int numiterations = 1;
int opt_yield = 0;
char opt_sync;
char *yieldtypes = "";
SortedListElement_t* listElements;
SortedList_t* list;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int spinLock = 0;


//thread function to insert and delete from list
void *threadFunc(void *id){
	int tid = *(int*)id;
	int i;
	///protection
	if(opt_sync == 'm')pthread_mutex_lock(&lock);
	if(opt_sync == 's') while(__sync_lock_test_and_set(&spinLock, 1));
	//critical section
	for(i = tid; i < numElems; i += numthreads){
		SortedList_insert(list, &listElements[i]);
	}
	//make sure deletion is valid
	SortedListElement_t* cur;
	for(i = tid; i < numElems; i += numthreads){
		cur = SortedList_lookup(list, listElements[i].key);
		if(cur == NULL) {
			fprintf(stderr, "error: key not found\n");
			exit(1);
		}
		if (SortedList_delete(cur)!= 0){
			fprintf(stderr, "Error: cannot delete list element\n");
			exit(1);
		}
	}
	if(opt_sync == 'm')pthread_mutex_unlock(&lock);
	if(opt_sync == 's') __sync_lock_release(&spinLock);
	return NULL;
}


int main(int argc, char**argv) {

	int opt;
	struct timespec start, end;
	static struct option long_opts[] =
	{
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"threads", required_argument, 0, 't'},
		{"yield", required_argument, 0, 'y'},
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
				if (strlen(optarg) == 0 || strlen(optarg) > 3) {
					fprintf(stderr, "correct usage = ");
					exit(1);
				}
				yieldtypes = optarg;
				int l = strlen(optarg);
				for(int i = 0; i < l; i++) {
					if (optarg[i] == 'i')
						opt_yield |= INSERT_YIELD;
					else if (optarg[i] == 'd')
						opt_yield |= DELETE_YIELD;
					else if (optarg[i] == 'l')
						opt_yield |= LOOKUP_YIELD;
					else {
						fprintf(stderr, "incorrect yield options");
						exit(1);
					}
        		}	
				break;
			case 's':
				opt_sync = optarg[0];
				break;
			default:
				fprintf(stderr, "correct usage: ./lab2_add --iterations= --threads= --yield[idl]= --sync=m, s, or c.\n");
				exit(1); //Exit with status of failure
		}
	}
	//init linkedlist
	list = malloc(sizeof(SortedList_t));
	list->key = NULL;
	list->next = list;
	list->prev = list;
	//init the elements of the linkedlist
	numElems = numthreads*numiterations;
	listElements = malloc(sizeof(SortedListElement_t) * numElems);
	pthread_t *threads = malloc((sizeof(pthread_t)*numthreads));
	int* tids = malloc(sizeof(int)*numthreads);

	srand(time(NULL));

	int i, j;
	for(i = 0; i < numElems; i++) {
		//listElements[i] = malloc(sizeof(SortedListElement_t));
		listElements[i].prev = NULL;
		listElements[i].next = NULL;

		//generate the key
		char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv123456789";
		int aLength = 61; 
		// key will have 10 characters
		int keyLength = 15;
		char *key = malloc(keyLength*sizeof(char));  
		// initialize each character for the key
		for (j = 0; j < keyLength -1; j++) {
			key[j] = chars[rand() % aLength];
		}
		key[keyLength-1] = '\0';		
		listElements[i].key = key;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	//create threads and make sure they exit at same time
	for (i =0; i < numthreads; i ++){
		tids[i] = i;
		pthread_create(&threads[i], NULL, threadFunc, &tids[i]);
	}
	for (i =0; i < numthreads; i ++){
		int ret = pthread_join(threads[i], NULL);
		if(ret){
		  fprintf(stderr, "Error freeing threads. Error code %d\n", ret);
		  exit(-1);
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &end);

	//check if race condition occurred
	if(SortedList_length(list) != 0) {
		fprintf(stderr, "error: list length != 0\n");
		exit(1);
	}


	printf("list");
	if(opt_yield)
		printf("-%s", yieldtypes);
	if(!opt_yield)
		printf("-none");
	if(opt_sync == 'm')
		printf("-m");
	if(opt_sync == 's')
		printf("-s");
	if(!opt_sync)
		printf("-none");

	//csv
	long long totalTime =(end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
	int ops = 3 * numthreads * numiterations;
	printf(",%d,%d,%d,%d,%lld,%llu\n", numthreads, numiterations, 1, ops, totalTime, totalTime/ops);

	//maybe use at exit
	// free(threads);
	// free(list);
	free(listElements);

}
