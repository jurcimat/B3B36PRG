/*
 * File name: hw06-test.c
 * Date:      2017/03/16 07:55
 * Author:    Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "linked_list.h"

int main(int argc, char *argv[])
{
   int ret = EXIT_SUCCESS;
   int r;
   char c;
   _Bool quit = 0;

   while (ret == EXIT_SUCCESS && !quit && (r = scanf("%c", &c)) > 0) {
      if (isspace(c)) { continue; }
      if (!isalpha(c)) {
         fprintf(stderr, "ERROR: Unexpected non-aplha character\n");
         ret = 101;
         continue;
      }
      int v;
      int n = 1;
      switch(c) {
         case 'a': // add (push) item from the queue
            if ((n = scanf("%i", &v)) == 1) {
               printf("QUEUE PUSH %d RETURN VALUE %i\n", v, push(v));
            }
            break;
         case 'i': // insert item from the queue
            if ((n = scanf("%i", &v)) == 1) {
               printf("QUEUE INSERT %d RETURN VALUE %i\n", v, insert(v));
            }
            break;
         case 'g': // get (pop) item from the queue
            {
               int v = pop();
               if (v < 0) {
                  printf("QUEUE POP VALUE IS NEGATIVE, QUEUE SEEMS TO BE EMPTY\n");
               } else {
                  printf("QUEUE POP VALUE %d\n", v);
               }
            }
            break;
         case 'r': // erase (remove) item from the queue
            if ((n = scanf("%i", &v)) == 1) {
               _Bool rl = erase(v);
               printf("QUEUE ERASE %d RETURN VALUE %i\n", v, rl);
            }
            break;
         case 's': // print size of the queue
            printf("QUEUE SIZE: %d\n", size());
            break;
         case 'p': // print the queue
            {
               const int N = size();
               for (int i = 0; i < N; ++i) {
                  printf("QUEUE PRINT ITEM[%d] = %d\n", i, getEntry(i));
               }
            }
            break;
         case 'c': // clear the whole queue
            printf("QUEUE CLEAR\n");
            clear();
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
   clear(); // clear if there are some remaining entries
   return ret;
}
/* end of hw06-test.c */
