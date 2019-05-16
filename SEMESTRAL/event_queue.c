/*
 * File name: event_queue.c
 * Date:      2017/04/15 12:42
 * Author:    Jan Faigl
 */

#include <stdlib.h>

#include <pthread.h>

#include "event_queue.h"

#ifndef QUEUE_CAPACITY
#define QUEUE_CAPACITY 16 // queue for 16 messages
#endif

typedef struct {
   event queue[QUEUE_CAPACITY];  // message queue
   volatile int in; // pointer to the circular queue
   volatile int out; // pointer to the circular queue
   pthread_mutex_t mtx;
   pthread_cond_t cond;
} queue;

queue q = { .in = 0, .out = 0 };

// - function -----------------------------------------------------------------
void queue_init(void)
{
   pthread_mutex_init(&q.mtx, NULL);
   pthread_cond_init(&q.cond, NULL);
}

// - function -----------------------------------------------------------------
void queue_cleanup(void)
{
   while (q.in != q.out) {
      event ev = queue_pop();
      if (ev.type == EV_SERIAL && ev.data.msg) {
	 free(ev.data.msg);
      }
   }
}

// - function -----------------------------------------------------------------
event queue_pop(void)
{
   event ret = { .source = EV_NUM, .type = EV_TYPE_NUM };
   pthread_mutex_lock(&(q.mtx));
   while (q.in == q.out) {
      pthread_cond_wait(&(q.cond), &(q.mtx)); // wait for next event
   }
   if (q.in != q.out) {
      ret = q.queue[q.out];
      q.out = (q.out + 1) % QUEUE_CAPACITY;
      pthread_cond_broadcast(&(q.cond)); // notify if some thread is waitting in push 
   }
   pthread_mutex_unlock(&(q.mtx));
   return ret;
}

// - function -----------------------------------------------------------------
void queue_push(event ev)
{
   pthread_mutex_lock(&(q.mtx));
   while (((q.in + 1) % QUEUE_CAPACITY) == q.out) { // queue is full wait for pop
      pthread_cond_wait(&(q.cond), &(q.mtx)); // wait for some event is popped
   }
   q.queue[q.in] = ev;
   q.in = (q.in + 1) % QUEUE_CAPACITY;
   pthread_cond_broadcast(&(q.cond)); // notify if some thread is waitting on pop
   pthread_mutex_unlock(&(q.mtx));
}

/* end of event_queue.c */
