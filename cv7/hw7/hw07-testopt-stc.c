/*
 * File name: hw06-testopt-stc.c
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

typedef struct my_struct {
   int v;
   char *str;
} my_struct;

void stc_clear(void* a) 
{
   my_struct *v = (my_struct*)a;
   if (v) {
      if (v->str) {
         free(v->str);
      }
      free(v);
   }
}

int stc_compare(const void* a, const void* b) 
{
   const my_struct *as = (const my_struct*)a;
   const my_struct *bs = (const my_struct*)b;
   int comp_result = strcmp(as->str, bs->str);
   comp_result = (comp_result < 0) ? -1 : (comp_result > 0);
   return as->v == bs->v ? comp_result : ( as->v < bs->v ? -1 : 1);
}

my_struct* read_value(void);

int main(int argc, char *argv[])
{
   int ret = EXIT_SUCCESS;
   int r;
   char c;
   my_struct *data = NULL;
   void *q = create();
   _Bool quit = 0;

   if (q) {
      setClear(q, stc_clear);
      setCompare(q, stc_compare);
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
            if ( (data = read_value()) ) {
               _Bool rl = push(q, data);
               printf("QUEUE PUSH (%d, '%s') RETURN VALUE %i\n", data->v, data->str, rl);
               if (!rl) {
                  stc_clear(data);
               }
               data = NULL;
            }
            break;
         case 'i': // insert item from the queue
            if ( (data = read_value()) ) {
               _Bool rl = insert(q, data);
               printf("QUEUE INSERT (%d, '%s') RETURN VALUE %i\n", data->v, data->str, rl);
               if (!rl) {
                  free(data);
               }
               data = NULL;
            }
            break;
         case 'g': // get (pop) item from the queue
            {
               data = pop(q);
               if (data) {
                  printf("QUEUE POP VALUE (%d, '%s')\n", data->v, data->str);
                  stc_clear(data); // Release the memory for the poped value
                  data = NULL;
               } else {
                  printf("QUEUE POP VALUE 'NULL'\n");
               }
            }
            break;
         case 'r': // erase (remove) item from the queue
            if ( (data = read_value()) ) {
               _Bool rl = erase(q, data);
               printf("QUEUE ERASE (%d, '%s') RETURN VALUE %i\n", data->v, data->str, rl);
               stc_clear(data);
               data = NULL;
            }
            break;
         case 's': // print size of the queue
            printf("QUEUE SIZE: %d\n", size(q));
            break;
         case 'p': // print the queue
            {
               const int N = size(q);
               for (int i = 0; i < N; ++i) {
                  const my_struct *data = getEntry(q, i);
                  if (data) {
                     printf("QUEUE PRINT ITEM[%d] = (%d, '%s')\n", i, data->v, data->str);
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
   if (data) {
      stc_clear(data);
   }
   clear(q); // clear if there are some remaining entries
   free(q);
   return ret;
}

my_struct* read_value(void) 
{
   int n = 0; // no. read tokens for parsing the 1st int value
   int v; // parsed int
   char *str = malloc(sizeof(char)*(STR_SIZE+1));
   my_struct *data = NULL;
   if (str) {
      int i = 0;
      if ( (n = scanf("%i", &v)) == 1) { //reading int has been succesful
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
   }
   if (str && n == 1) {
      if ( (data = malloc(sizeof(my_struct))) ) {
         data->v = v;
         data->str = str;
      } else {
         free(str); //release data for string
      }
   }
   return data;
}

/* end of hw06-testopt-str.c */
