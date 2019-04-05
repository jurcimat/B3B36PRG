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
	if (((linked_list*)queue)->num_of_items <= 0){
		return NULL;
	}
	if (((linked_list*)queue)->num_of_items == 1){
		void* data = ((linked_list*)queue)->first->data;
		free(((linked_list*)queue)->first);
		((linked_list*)queue)->first = NULL;
		((linked_list*)queue)->last = NULL;
		((linked_list*)queue)->num_of_items = 0;
		return data;
	}
	linked_chain* temp = ((linked_list*)queue)->first->next_list;
	void* data = ((linked_list*)queue)->first->data;
	free(((linked_list*)queue)->first);
	((linked_list*)queue)->first = temp;
	temp->prev_list = NULL;
	((linked_list*)queue)->num_of_items--;
	return data;
}

_Bool insert(void *queue, void *entry){
	int (*compare_ptr)(const void *, const void *) = (((linked_list*)queue)->compare_ptr);
	if (entry == NULL){
		return false;
	}
	if (((linked_list*)queue)->num_of_items == 0){
		push(queue,entry);	
		return true;
	}
	else if ((*compare_ptr)(((linked_list*)queue)->first->data,entry) <= 0){
		linked_chain *temp = (linked_chain*)malloc(sizeof(linked_chain));
		temp->data = entry;
		temp->next_list = (((linked_list*)queue)->first);
		temp->prev_list = NULL;
		temp->index = 0;
		(((linked_list*)queue)->first)->prev_list = temp;
		(((linked_list*)queue)->first) = temp;
		((linked_list*)queue)->num_of_items++;
		temp = NULL;
		return true;
	} else {
		for (linked_chain* iter = (((linked_list*)queue)->first) ; iter != NULL ; iter = iter->next_list){
			if ((*compare_ptr)(iter->data,entry) <= 0){
				linked_chain *temp = (linked_chain*)malloc(sizeof(linked_chain));
				temp->data = entry;
				temp->next_list = iter;
				temp->prev_list = iter->prev_list;
				temp->index = iter->index - 1;
				iter->prev_list->next_list = temp;
				iter->prev_list = temp;
				((linked_list*)queue)->num_of_items++;
				temp = NULL;
				return true;
			}
		}
		if ((*compare_ptr)(((linked_list*)queue)->last->data,entry) >= 0){
			linked_chain* temp = (linked_chain*)malloc(sizeof(linked_chain));
			temp->data = entry;
			temp->prev_list = ((linked_list*)queue)->last;
			temp->next_list = NULL;
			temp->index = ((linked_list*)queue)->last->index + 1;
			((linked_list*)queue)->last->next_list = temp;
			((linked_list*)queue)->last = temp;
			((linked_list*)queue)->num_of_items++;
			temp = NULL;
			return true;
		}
	return false;
	
	}
	
}
_Bool erase(void *queue, void *entry){
	if (((linked_list*)queue)->num_of_items == 0){
		return false;	
	}
	int found_erased = 0;
	linked_chain *iter = ((linked_list*)queue)->first;
	void (*clear_ptr)(void*) = (((linked_list*)queue)->clear_ptr);
	int (*compare_ptr)(const void *, const void *) = (((linked_list*)queue)->compare_ptr);
	while (iter){
		linked_chain *temp = iter->next_list;
		if ((*compare_ptr)(iter->data,entry) == 0){
			if (iter->prev_list == NULL){
				((linked_list*)queue)->first = iter->next_list;
			} else {
				iter->prev_list->next_list = iter->next_list;			
			}
			if (iter->next_list == NULL){
				((linked_list*)queue)->last = iter->prev_list;
			} else {
				iter->next_list->prev_list = iter->prev_list;			
			}
			(*clear_ptr)(iter->data);
			free(iter);
			((linked_list*)queue)->num_of_items--;
			found_erased++;			
		}
		iter = temp;
	}
	if (found_erased == 0){
		return false;
	}
	return true;	
}

void* getEntry(const void *queue, int idx){
	if (idx >= 0 && idx < ((linked_list*)queue)->num_of_items){
		int counter = 0;
		linked_chain *iterative = ((linked_list*)queue)->first;
		while (iterative){
			linked_chain *temp = iterative->next_list;
			if (counter == idx){
				return iterative->data;
			}
			counter++;
			iterative = temp;		
		}
	
	}
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



