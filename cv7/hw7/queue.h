#ifndef __QUEUE_H__
#define __QUEUE_H__
 
/* 
 * Allocate a new queue structure or return NULL on an error.
 * Particular type is implementation dependent
 */
void* create();
 
/*
 * Release all memory accessible from the queue, i.e., all dynamic
 * data entries stored in the individual queue entries. The clear 
 * function must be properly set, see setClear() below.
 */
void clear(void *queue);
 
/*
 * Push the given item into the queue. The pointer to the entry
 * should not be NULL, i.e., storing NULL entries is not allowed.
 * return: true on success and false otherwise.
 */
_Bool push(void *queue, void *entry);
 
/*
 * Pop an entry from the head of the queue
 * return: the stored pointer to the entry or NULL if the queue is empty
 */
void* pop(void *queue);
 
/*
 * Insert the given entry to the queue in the InsertSort style using 
 * the set compare function (by the setCompare() ). If such a function
 * is not set or an error occurs during the insertion it returns false.
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
_Bool insert(void *queue, void *entry);

/*
 * Erase all entries with the value entry, if such exists
 * return: true on success; false to indicate no such value has been removed
 */
_Bool erase(void *queue, void *entry);
 
/*
 * For idx >= 0 and idx < size(queue), it returns the particular item
 * stored at the idx-th position of the queue. The head of the queue
 * is the entry at idx = 0.
 *
 * return: pointer to the stored item at the idx position of the queue
 * or NULL if such an entry does not exists at such a position
 */
void* getEntry(const void *queue, int idx);
 
/*
 * return: the number of stored items in the queue
 */
int size(const void *queue);
 
/*
 * Set a pointer to function for comparing particular inserted items
 * to the queue. It is similar to the function used in qsort, see man qsort:
 * "The comparison function must return an integer less than, equal to, or
 * greater than zero if the first argument is considered to be respectively
 * less than, equal to, or greater than the second."
 */
void setCompare(void *queue, int (*compare)(const void *, const void *));
 
/*
 * Set a pointer to function which can properly delete the inserted 
 * items to the queue. If it is not set, all the items stored in the
 * queue are deleted calling standard free() in the clear() 
 */
void setClear(void *queue, void (*clear)(void *));
 
#endif
