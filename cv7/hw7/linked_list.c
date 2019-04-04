#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
typedef struct linked_l{
   int data;
   int index;
   struct linked_l *next_list;
   struct linked_l *prev_list;
}linked_list;
 
int num_of_items;
linked_list* first = NULL;
linked_list* last = NULL;
 
_Bool push(int entry)
{
    if (entry < 0)
    {
        return false;
    }
 
    linked_list* temp = malloc(sizeof(linked_list));
    if (temp == NULL)
    {
        return false;
    }
    if (num_of_items == 0)
    {
        temp->data = entry;
        temp->next_list = NULL;
        temp->prev_list = NULL;
        first = temp;
        last = temp;
        num_of_items = 1;
        temp->index = 0;
        temp = NULL;
    }
    else
    {
        temp->data = entry;
        temp->next_list = NULL;
        temp->prev_list = last;
        last->next_list = temp;
        last = temp;
        temp->index = num_of_items;
        num_of_items++;
        temp = NULL;
    }
    return true;
}
 
 
 
int pop(void)
{
    if (num_of_items <= 0)
    {
        first = NULL;
        last = NULL;
        return -100;
    }
    if (num_of_items == 1)
    {
        int tmp_i = first->data;
        free(first);
        first = NULL;
        last = NULL;
        num_of_items--;
        return tmp_i;
    }
    linked_list *tmp = first->next_list;
    int tmp_i = first->data;
    free(first);
    first = tmp;
    first->prev_list = NULL;
    num_of_items--;
    return tmp_i;
}
 
 
_Bool insert(int entry)
{
    if (entry < 0)
    {
        return false;
    }
    if (num_of_items == 0)
    {
        push(entry);
        return true;
    }
    else if (first -> data <= entry)
    {
        linked_list* temp = malloc(sizeof(linked_list));
        temp->data = entry;
        temp->next_list = first;
        temp->prev_list = NULL;
        first->prev_list = temp;
        first = temp;
        num_of_items++;
        temp = NULL;
        return true;
    }
    else
    {
        for (linked_list *i = first; i != NULL; i = i->next_list)
        {
            if (i->data <= entry)
            {
                linked_list* temp = malloc(sizeof(linked_list));
                temp->data = entry;
                temp->next_list = i;
                temp->prev_list = i->prev_list;
                i->prev_list->next_list = temp;
                i->prev_list = temp;
                num_of_items++;
                temp = NULL;
                return true;
            }
        }
        if (last->data >= entry)
        {
            linked_list* temp = malloc(sizeof(linked_list));
            temp->data = entry;
            temp->prev_list = last;
            temp->next_list = NULL;
            last->next_list = temp;
            last = temp;
            num_of_items++;
            return true;
        }
    }
    return true;
}
 
 
_Bool erase(int entry)
{
    if (num_of_items <= 0)
    {
        return false;
    }
    int found_erased = 0;
    linked_list *i = first;
        while (i) {
            linked_list * tmp = i -> next_list;
            if (i->data == entry)
            {
                if (i->prev_list == NULL)
                {
                    first = i->next_list;
                }
                else
                {
                    i->prev_list->next_list = i->next_list;
                }
 
                if (i->next_list == NULL)
                {
                    last = i->prev_list;
                }
                else
                {
                    i->next_list->prev_list = i->prev_list;
                }               
                free(i);
                num_of_items--;
                found_erased = 1;   
            }
            i = tmp;
        }           
    if (found_erased == 0)
    {
        return false;
    }
    return true;
}
 
 
int getEntry(int idx)
{
    if (idx >=0 && idx < num_of_items)
    {
        int counter = 0;
        linked_list *i = first;
        while (i) {
            linked_list * tmp = i -> next_list;
            if (counter == idx)
            {
                return i->data;
            }
            counter++;
            i = tmp;
        }   
    }
    return -100;
}
 
 
int size(void)
{
    return num_of_items;
}
 
 
void clear()
{
    if (num_of_items > 0)
    {
            linked_list *i = first;
            while (i) {
                linked_list * tmp = i -> next_list;
                free(i);
                i = tmp;
            }   
        }
        first = NULL;
        last = NULL;
        num_of_items = 0;
         
}
