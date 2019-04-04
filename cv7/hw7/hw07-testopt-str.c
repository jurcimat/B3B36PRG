/*
 * File name: hw06-testopt-str.c
 * Date:      2017/03/16 11:49
 * Author:    Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "queue.h"

#ifndef STR_SIZE
#define STR_SIZE 128
#endif

void str_clear(void* a) 
{
   if (a) {
      free(a); // nothing special is required for int
   }
}

int str_compare(const void* a, const void* b) 
{
   const char *ai = (const char*)a;
   const char *bi = (const char*)b;
   int comp_result = strcmp(ai, bi);
   return (comp_result < 0) ? -1 : (comp_result > 0);
}

char* read_value(void);

int main(int argc, char *argv[])
{
   int ret = EXIT_SUCCESS;
   int r;
   char c;
   char *v = NULL;
   void *q = create();
   _Bool quit = 0;

   if (q) {
      setClear(q, str_clear);
      setCompare(q, str_compare);
   }
   while (ret == EXIT_SUCCESS && !quit && (r = scanf("%c", &c)) > 0) {
      if (isspace(c)) { continue; }
      if (!isalpha(c)) {
         fprintf(stderr, "ERROR: Unexpected non-aplha character\n");
         ret = 101;
         continue;
      }
      switch(c) {
         case 'a': // add (push) item from the queue
            if ( (v = read_value()) ) {
               _Bool rl = push(q, v);
               printf("QUEUE PUSH '%s' RETURN VALUE %i\n", v, rl);
               if (!rl) {
                  free(v);
               }
               v = NULL;
            }
            break;
         case 'i': // insert item from the queue
            if ( (v = read_value()) ) {
               _Bool rl = insert(q, v);
               printf("QUEUE INSERT '%s' RETURN VALUE %i\n", v, rl);
               if (!rl) {
                  free(v);
               }
               v = NULL;
            }
            break;
         case 'g': // get (pop) item from the queue
            {
               char *v = pop(q);
               if (v) {
                  printf("QUEUE POP VALUE '%s'\n", v);
                  free(v); // Release the memory for the poped value
               } else {
                  printf("QUEUE POP VALUE 'NULL'\n");
               }
            }
            break;
         case 'r': // erase (remove) item from the queue
            if ( (v = read_value()) ) {
               _Bool rl = erase(q, v);
               printf("QUEUE ERASE '%s' RETURN VALUE %i\n", v, rl);
               free(v);
               v = NULL;
            }
            break;
         case 's': // print size of the queue
            printf("QUEUE SIZE: %d\n", size(q));
            break;
         case 'p': // print the queue
            {
               const int N = size(q);
               for (int i = 0; i < N; ++i) {
                  const char *v = getEntry(q, i);
                  if (v) {
                     printf("QUEUE PRINT ITEM[%d] = '%s'\n", i, v);
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
   } //end while loop
   if (r == 0) {
      fprintf(stderr, "ERROR: Unexpected input\n");
      ret = 100;
   }
   if (v) {
      free(v);
   }
   clear(q); // clear if there are some remaining entries
   free(q);
   return ret;
}

char* read_value(void) 
{
   char *str = malloc(sizeof(char)*(STR_SIZE+1));
   if (str) {
      int i = 0;
      int c = getchar();
      if (c != EOF && isspace(c)) { // there must be exactly one space between the command and the argument (string/token)
         while ( (c = getchar() ) != EOF && !isspace(c) 
               && (c >= 0 && c <= 0177) && i < STR_SIZE) {  // isascii(c)  0-0177
            str[i++] = c;
         }
      }
      if (c == EOF || i == 0) {
         free(str);
         str = NULL;
      } else {
         str[i] = '\0';
      }
   }
   return str;
}

/* end of hw06-testopt-str.c */
