/*
NAME: Andrew Ding
EMAIL: andrewxding@ucla.edu	
ID: 504748356
*/
#define _GNU_SOURCE
#include "SortedList.h"
#include <pthread.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	SortedList_t *head = list;  // save the head of the list
	SortedList_t *currElement = list->next;

	// yield CPU before critical section
	if (opt_yield & INSERT_YIELD) {
		sched_yield();
	}

	// iterate through the list to find where to insert the element
	while (currElement != head) {
		// found the element to come after the element to be inserted
		if (strcmp(element->key, currElement->key) <= 0) {
			break;  
		}
		currElement = currElement->next;
	}

	// insert into the list
	element->prev = currElement->prev;
	element->next = currElement;
	currElement->prev->next = element;
	currElement->prev = element;
}

int SortedList_delete(SortedListElement_t *element)
{
	if(element == NULL) 
		return 1;
	if(opt_yield & DELETE_YIELD)
		sched_yield();
	//critical section
	if(element->prev->next == element->next->prev)
	{
		//delete the element
		element->prev->next = element->next;
		element->next->prev = element->prev;
		return 0;
	}
	return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	SortedListElement_t *cur = list->next;
	if(opt_yield & LOOKUP_YIELD)
		sched_yield();
	while(cur != list)
	{
		if(strcmp(cur->key, key) == 0)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

int SortedList_length(SortedList_t *list)
{
	int count = 0;
	if(list == NULL)
		return -1;
	SortedListElement_t *cur = list->next;
	if(opt_yield & LOOKUP_YIELD)
		sched_yield();
	//critical section
	while(cur != list)
	{
		count++;
		cur = cur->next;
	}
	return count;
}