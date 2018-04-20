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


pthread_mutex_t *sublistlocks;
int *sublistspins;
SortedList_t **sublists;
int numlists = 1;
int keyLength = 15;
long long * waittimes;

//  djb2 hash function
unsigned long hash(const char *key) {
    unsigned long val = 5381;
    for (int i = 0; i < keyLength; i++)
        val = ((val << 5) + val) + key[i];
    return val;
}
//thread function to insert and delete from list
void *threadFunc(void *id){
	int tid = *(int*)id;
	int i;
	struct timespec mstart, mend;

	///insertion critical section
	if(opt_sync == 'm'){
		if (clock_gettime(CLOCK_MONOTONIC, &mstart)) {   
			fprintf(stderr, "Error with clock_gettime()\n");
			exit(1);
		}
		pthread_mutex_lock(&lock);
		if (clock_gettime(CLOCK_MONOTONIC, &mend)) {   
			fprintf(stderr, "Error with clock_gettime()\n");
			exit(1);
		}
		waittimes[tid] += (mend.tv_sec - mstart.tv_sec) * 1000000000 + (mend.tv_nsec - mstart.tv_nsec);
	}
	if(opt_sync == 's') 
		while(__sync_lock_test_and_set(&spinLock, 1));
	//critical section
	for(i = tid; i < numElems; i += numthreads){
		SortedList_insert(sublists[hash(listElements[i].key) % numlists], &listElements[i]);
	}
	if(opt_sync == 'm')
		pthread_mutex_unlock(&lock);
	if(opt_sync == 's')
		__sync_lock_release(&spinLock);


	//looking up length of sublists critical section
	for(i = 0; i < numlists; i++){
		if(opt_sync == 'm'){
			if (clock_gettime(CLOCK_MONOTONIC, &mstart)) {   
				fprintf(stderr, "Error with clock_gettime()\n");
				exit(1);
			}
			pthread_mutex_lock(&sublistlocks[i]);
			if (clock_gettime(CLOCK_MONOTONIC, &mend)) {   
				fprintf(stderr, "Error with clock_gettime()\n");
				exit(1);
			}
			waittimes[tid] += (mend.tv_sec - mstart.tv_sec) * 1000000000 + (mend.tv_nsec - mstart.tv_nsec);
		}
		if(opt_sync == 's'){
			 while (__sync_lock_test_and_set(&sublistspins[i], 1));
		}
		if (SortedList_length(sublists[i]) < 0) {  //  Check for list corruption
	        fprintf(stderr, "*** Error: SortedList_length: list is corrupted.\n");
	        exit(1);
	    }
	    if(opt_sync == 'm'){
			pthread_mutex_unlock(&sublistlocks[i]);
		}
		if(opt_sync == 's'){
			 __sync_lock_release(&sublistspins[i]);
		}
	}


	//make sure deletion is valid, deletion critical section
	if(opt_sync == 'm'){
		if (clock_gettime(CLOCK_MONOTONIC, &mstart)) {   
			fprintf(stderr, "Error with clock_gettime()\n");
			exit(1);
		}
		pthread_mutex_lock(&lock);
		if (clock_gettime(CLOCK_MONOTONIC, &mend)) {   
			fprintf(stderr, "Error with clock_gettime()\n");
			exit(1);
		}
		waittimes[tid] += (mend.tv_sec - mstart.tv_sec) * 1000000000 + (mend.tv_nsec - mstart.tv_nsec);
	}
	if(opt_sync == 's') 
		while(__sync_lock_test_and_set(&spinLock, 1));
	SortedListElement_t* cur;
	for(i = tid; i < numElems; i += numthreads){
		cur = SortedList_lookup(sublists[hash(listElements[i].key) % numlists], listElements[i].key);
		if(cur == NULL) {
			fprintf(stderr, "error: key not found\n");
			exit(1);
		}
		if (SortedList_delete(cur)!= 0){
			fprintf(stderr, "Error: cannot delete list element\n");
			exit(1);
		}
	}
	if(opt_sync == 'm')
		pthread_mutex_unlock(&lock);
	if(opt_sync == 's')
		__sync_lock_release(&spinLock);
	return NULL;
}


int main(int argc, char**argv) {

	int opt;
	struct timespec start, end;
	int i, j;
	static struct option long_opts[] =
	{
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"threads", required_argument, 0, 't'},
		{"yield", required_argument, 0, 'y'},
		{"lists", required_argument, 0, 'n'},
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
			case 'n':
				numlists = atoi(optarg);
				break;
			default:
				fprintf(stderr, "correct usage: ./lab2_add --iterations= --threads= --yield[idl]= --sync=m, s, or c.\n");
				exit(1); //Exit with status of failure
		}
	}
	//init all the linkedlist
	list = malloc(sizeof(SortedList_t));
	list->key = NULL;
	list->next = list;
	list->prev = list;

	//create sublists, array of waittimes and mutex/spin locks for each sublist
	sublists = malloc(numlists * sizeof(SortedList_t*));
	sublistlocks = malloc(numlists * sizeof(pthread_mutex_t));
	sublistspins = malloc(numlists * sizeof(int));
	waittimes = malloc(numthreads * sizeof(long long));

	for (i = 0; i < numlists; i++) {
		sublists[i] = malloc(sizeof(SortedList_t));
		sublists[i]->prev = sublists[i];
		sublists[i]->next = sublists[i];
		sublists[i]->key = NULL;

		if(opt_sync == 'm'){
			if (pthread_mutex_init(&sublistlocks[i], NULL)) {
               fprintf(stderr, "Error: pthread_mutex_init failed.\n");
                exit(1);
            }
		}
		if(opt_sync == 's')
			sublistspins[i] = 0;
	}

	//init the elements of the linkedlist
	numElems = numthreads*numiterations;
	listElements = malloc(sizeof(SortedListElement_t) * numElems);
	pthread_t *threads = malloc((sizeof(pthread_t)*numthreads));
	int* tids = malloc(sizeof(int)*numthreads);
	///init the elements of the list
	srand(time(NULL));
	for(i = 0; i < numElems; i++) {
		listElements[i].prev = NULL;
		listElements[i].next = NULL;

		//generate the key
		char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv123456789";
		int aLength = 61; 
		// key will have 10 characters
		
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
		waittimes[i] = 0;
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

	for (i = 0; i < numlists; i++) {
	//check if race condition occurred
		if(SortedList_length(sublists[i]) != 0) {
			fprintf(stderr, "error: list length != 0\n");
			free(threads);
			free(list);
			free(listElements);
			free(tids);
			free(waittimes);
			exit(1);
		}
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


	//sum up thread wait times
	long long blockedtime = 0;
	for( i = 0; i < numthreads; i ++){
		blockedtime += waittimes[i];
	}
	//csv
	long long totalTime =(end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
	int ops = 3 * numthreads * numiterations;
	printf(",%d,%d,%d,%d,%lld,%llu, %lld\n", numthreads, numiterations, numlists, ops, totalTime, totalTime/ops, blockedtime/ops);
	//maybe use at exit
	free(threads);
	free(list);
	free(listElements);
	free(tids);
	free(waittimes);

}
