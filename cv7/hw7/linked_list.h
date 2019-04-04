#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

/*
 * Push the entry value to the queue. The value of the entry must be >= 0.
 * return: true on success and false otherwise.
 */
_Bool push(int entry);

/*
 * Pop an entry from the head of the queue
 * return: the stored int value or value less than 0 indicating the queue is empty
 */
int pop(void);

/*
 * Insert the given entry to the queue in the InsertSort style.
 *
 * Since push and insert functions can be combined, it cannot be 
 * guaranteed, the internal structure of the queue is always sorted.
 *
 * The expected behaviour is that insert proceeds from the head of
 * the queue to the tail in such a way that it is insert before the entry 
 * with the lower value, i.e., it becomes a new head if its value is the
 * new maximum or a new tail if its value is a new minimum of the values
 * in the queue.
 *
 * return: true on success; false otherwise
 */
_Bool insert(int entry);

/*
 * Erase all entries with the value entry, if such exists
 * return: true on success; false to indicate no such value has been removed
 */
_Bool erase(int entry);

/*
 * For idx >= 0 and idx < size(queue), it returns the particular item
 * stored at the idx-th position of the queue. The head of the queue
 * is the entry at idx = 0.
 *
 * return: the particular value of the entry at the idx-th position or
 * value less than 0 to indicate the requested position is not presented
 * in the queue 
 */
int getEntry(int idx);

/*
 * return: the number of stored items in the queue
 */
int size(void);

/*
 * Remove all entries in the linked list
 */
void clear();
 
#endif
