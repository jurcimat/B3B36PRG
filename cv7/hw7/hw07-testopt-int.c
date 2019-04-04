/*
 * File name: hw06-testopt-int.c
 * Date:      2017/03/16 11:49
 * Author:    Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "queue.h"

void int_clear(void* a) 
{
   if (a) {
      free(a); // nothing special is required for int
   }
}

int int_compare(const void* a, const void* b) 
{
   const int *ai = (const int*)a;
   const int *bi = (const int*)b;
   return *ai == *bi ? 0 : ( *ai < *bi ? -1 : 1);
}

int main(int argc, char *argv[])
{
   int ret = EXIT_SUCCESS;
   int r;
   char c;
   void *q = create();
   _Bool quit = 0;

   if (q) {
      setClear(q, int_clear);
      setCompare(q, int_compare);
   }
   while (ret == EXIT_SUCCESS && !quit && (r = scanf("%c", &c)) > 0) {
      if (isspace(c)) { continue; }
      if (!isalpha(c)) {
         fprintf(stderr, "ERROR: Unexpected non-aplha character\n");
         ret = 101;
         continue;
      }
      int v;
      int n = 1;
      int *vp;
      switch(c) {
         case 'a': // add (push) item from the queue
            if ((n = scanf("%i", &v)) == 1 && (vp = malloc(sizeof(int))) ) {
               *vp = v; // copy the int value
               _Bool rl = push(q, vp);
               printf("QUEUE PUSH %d RETURN VALUE %i\n", v, rl);
               if (!rl) {
                  free(vp);
               }
            }
            break;
         case 'i': // insert item from the queue
            if ((n = scanf("%i", &v)) == 1 && (vp = malloc(sizeof(int))) ) {
               *vp = v; // copy the int value
               _Bool rl = insert(q, vp);
               printf("QUEUE INSERT %d RETURN VALUE %i\n", v, rl);
               if (!rl) {
                  free(vp);
               }
            }
            break;
         case 'g': // get (pop) item from the queue
            {
               int *pv = pop(q);
               if (pv) {
                  printf("QUEUE POP VALUE %d\n", *pv);
                  free(pv); // Release the memory for the poped value
               } else {
                  printf("QUEUE POP VALUE 'NULL'\n");
               }
            }
            break;
         case 'r': // erase (remove) item from the queue
            if ((n = scanf("%i", &v)) == 1) {
               _Bool rl = erase(q, &v);
               printf("QUEUE ERASE %d RETURN VALUE %i\n", v, rl);
            }
            break;
         case 's': // print size of the queue
            printf("QUEUE SIZE: %d\n", size(q));
            break;
         case 'p': // print the queue
            {
               const int N = size(q);
               for (int i = 0; i < N; ++i) {
                  int *pv = getEntry(q, i);
                  if (pv) {
                     printf("QUEUE PRINT ITEM[%d] = %d\n", i, *pv);
                  } else {
                     printf("QUEUE PRINT ITEM[%d] = 'NULL'\n", i);
                  }
               }
            }
            break;
         case 'c': // clear the whole queue
            printf("QUEUE CLEAR\n");
            clear(q);
            break;
	 case 'q': //exit
	    quit = 1;
	    printf("EXIT\n");
	    break;
	 default:
	    fprintf(stderr, "ERROR: unknown command, terminate parsing\n");
	    ret = 102;
	    break;
      } //end switch
      if (n != 1) {
	 fprintf(stderr, "ERROR: expected int value has not been parsed\n");
	 ret = 103;
      }
   } //end while loop
   if (r == 0) {
      fprintf(stderr, "ERROR: Unexpected input\n");
      ret = 100;
   }
   clear(q); // clear if there are some remaining entries
   free(q);
   return ret;
}

/* end of hw06-testopt-int.c */
