#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct linked_c{
   
   int index;
   void *data;
   struct linked_c *next_list;
   struct linked_c *prev_list;
}linked_chain;

typedef struct linked_l{
	int num_of_items;
	void (*clear_ptr)(void*);
	int (*compare_ptr)(const void *, const void *);
	struct linked_c *first;
	struct linked_c *last;	
}linked_list;
 
void* create(void){
	linked_list* new = (linked_list*)malloc(sizeof(linked_list));
	new->num_of_items = 0;
	new->first = NULL;
	new->last = NULL;
	if (new == NULL){
		return NULL;
	}else {
		return new;
	}
}

void clear(void *queue){
	if (((linked_list*)queue)->num_of_items > 0){
		linked_chain *temp = ((linked_list*)queue)->first;
		void (*clear_ptr)(void*) = (((linked_list*)queue)->clear_ptr);
		while (temp){
			linked_chain *next = temp->next_list;
			(*clear_ptr)(temp->data);
			free(temp);
			temp = next;
		} 	
		((linked_list*)queue)->first = NULL;
		((linked_list*)queue)->last = NULL;
		((linked_list*)queue)->num_of_items = 0;
	
	
	}
}  


_Bool push(void *queue, void *entry){


	if (entry == NULL){
		return false;
	}
	linked_chain *chain = (linked_chain*)malloc(sizeof(linked_chain));
	if (chain == NULL){
		return false;		
	}
	if (((linked_list*)queue)->num_of_items == 0){
		chain->index = 0;
		chain->data = entry;
		chain->next_list = NULL;
		chain->prev_list = NULL;
		((linked_list*)queue)->num_of_items = 1;
		((linked_list*)queue)->first = chain;
		((linked_list*)queue)->last = chain;
		chain = NULL;
	}else{
		chain->index = ((linked_list*)queue)->num_of_items;
		chain->data = entry;
		chain->next_list = NULL;
		chain->prev_list = ((linked_list*)queue)->last;
		((linked_list*)queue)->num_of_items++;
		((linked_list*)queue)->last->next_list = chain;
		((linked_list*)queue)->last = chain;
		chain = NULL;	
	}
	
	return true;
	
}

void* pop(void *queue){
	return NULL;
}

_Bool insert(void *queue, void *entry){
	return 0;
}
_Bool erase(void *queue, void *entry){
	return 0;
}

void* getEntry(const void *queue, int idx){
	return NULL;
}
int size(const void *queue){
	return ((linked_list*)queue)->num_of_items; 
}

void setCompare(void *queue, int (*compare)(const void *, const void *)){
	((linked_list*)queue)->compare_ptr = compare;
}

void setClear(void *queue, void (*clear)(void *)){
	((linked_list*)queue)->clear_ptr = clear;
}



