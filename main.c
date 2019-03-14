#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
/* The main program */
 
 
int * erathostenes()
{
    static int r[1000000];
    int i,j;
    static int primes[78498];
    for (i = 0; i < 1000000;i++)
    {
        r[i] = i + 2;
     
    }
    for (i = 0; i < 1000000;i++)
    {
        if (r[i] != -1)
        {
            for (j=2*r[i]-2; j < 1000000; j+=r[i])
            {
                r[j] = -1;          
            }
        }
    }
    j=0;
    for (i=0; i<1000000 && j < 78498;i++)
    {
        if (r[i] != -1)
        {
            primes[j++] = r[i];
        }   
    }
    return primes;
 
}
 
 
 
 
int main(int argc, char *argv[])
{ 
  long inputNumber = 1;
  int * p;
  p = erathostenes();
  while (1)
  {
    if (scanf("%ld",&inputNumber) != 1)
    {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return 100;
    }
    else if (inputNumber < 0)
    {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return 100; 
    }else if (inputNumber == 1)
    {
        fprintf(stdout,"Prvociselny rozklad cisla 1 je:\n");
        fprintf(stdout,"1\n");
     
    }  
    else if (inputNumber == 0)
    {
        break;  
    }else
    {
 
  
        //int limit = (int)sqrt((double)inputNumber);
        printf("Prvociselny rozklad cisla %ld je:\n",inputNumber);
        int alreadyPrinted = 0;
        //int pitchovina = p[limit];
        //pri   ntf("limit je : %d\n prime = %d\n",limit,pitchovina);
        int i = 0;
        while (inputNumber != 1)
        {
            int counter=0;
            while (inputNumber % *(p+i) == 0)
            {
            //printf("%d\n",*(p+i));
            inputNumber = inputNumber / *(p+i);
            counter++;
             
            }
  
            if (counter > 1 && alreadyPrinted == 0)
            {
                printf("%d^%d",*(p+i),counter);
                alreadyPrinted = 1;
             
            }else if (counter == 1 && alreadyPrinted == 0)
            {
                printf("%d",*(p+i));
                alreadyPrinted = 1;
            }else if (counter > 1 && alreadyPrinted == 1)
            {
                printf(" x %d^%d",*(p+i),counter);
                alreadyPrinted = 1;
             
            }else if (counter == 1 && alreadyPrinted == 1)
            {
                printf(" x %d",*(p+i));
            }
            i++;
         
        } 
        if (alreadyPrinted == 0)
        {
            printf("%ld",inputNumber);
        }       
        fprintf(stdout,"\n");       
    }
   
   
   
   
  }
   
  return 0;
}
