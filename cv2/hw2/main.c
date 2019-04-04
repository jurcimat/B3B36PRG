#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
 
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

char* getInput(void){
	char c = getchar();
	int pointerSize = 20;
	int numberLength = 0; 
	char* pointer = (char*)malloc(pointerSize * sizeof(char));
	while (c != '\n')
	{
		pointer[numberLength] = c;
		numberLength++;
		if (numberLength == pointerSize){	
			pointerSize += 10;
			pointer = (char*)realloc(pointer,pointerSize*sizeof(char));
		}
		c = getchar();
	}
	pointer = (char*)realloc(pointer,(numberLength+1)*sizeof(char));
	pointer[numberLength] = '\0';
	return pointer;
}

int getLength(char* pointer){
	int length = 0;
	while (pointer[length] != '\0')
	{
		length += 1;
	}
	return length;
}


char* append(char* pointer, int length, int number){
	pointer = (char*)realloc(pointer,(length+1)*sizeof(char));
	pointer[length-1] = number + '0';
	pointer[length] = '\0';

	return pointer;
} 	

char* divide(char* number, int dividor){
	int lengthOfNumber = getLength(number);
	int id = 0;
	int temp = number[id] - '0';
	int ansLong = 0;
	char* ans = (char*)malloc(ansLong *sizeof(char));
	while (temp < dividor){

		id++;
		if (lengthOfNumber == id){
			ans = (char*)realloc(ans,2*sizeof(char));
			ans[0] = '0';
			ans[1] = '\0';
			temp = temp % dividor;
			break;
		}
		temp = temp * 10 + (number[id] - '0');
	}
	while (lengthOfNumber > id){
		ansLong++;
		ans = append(ans,ansLong,(temp/dividor));
		id++;
		if (id == lengthOfNumber){
			temp = temp % dividor;
		}else{
			temp = (temp % dividor)*10 + (number[id] - '0');
		}
	}
	free(number);
	return ans;
}

int divisionRemainder(char* number, int dividor){
	int lengthOfNumber = getLength(number);
	int id = 0;
	int temp = number[id] - '0';
	int ansLong = 0;
	char* ans = (char*)malloc(ansLong *sizeof(char));
	while (temp < dividor){

		id++;
		if (lengthOfNumber == id){
			ans = (char*)realloc(ans,2*sizeof(char));
			ans[0] = '0';
			ans[1] = '\0';
			temp = temp % dividor;
			break;
		}

		temp = temp * 10 + (number[id] - '0');
	}
	while (lengthOfNumber > id){
		ansLong++;
		ans = append(ans,ansLong,(temp/dividor));
		id++;
		if (id == lengthOfNumber){
			temp = temp % dividor;
		}else{
			temp = (temp % dividor)*10 + (number[id] - '0');
		}
	}
	free(ans);
	return temp;
}

int main(int argc, char *argv[])
{ 	
	if (argc == 2) {
		char* number = getInput();
		while (number[0] != '0'){
			printf("Prvociselny rozklad cisla %s je:\n",number);
			int alreadyPrinted = 0;
			int* primes = erathostenes();
			int index = 0;
			while(strcmp(number,"1") != 0){
				int counter = 0;
				while(divisionRemainder(number,primes[index]) == 0){
					number = divide(number,primes[index]);
					counter++;				
				}
				if (counter > 1 && alreadyPrinted == 0)
				{
					printf("%d^%d",primes[index],counter);
					alreadyPrinted = 1;
				 
				}else if (counter == 1 && alreadyPrinted == 0)
				{
					printf("%d",primes[index]);
					alreadyPrinted = 1;
				}else if (counter > 1 && alreadyPrinted == 1)
				{
					printf(" x %d^%d",primes[index],counter);
					alreadyPrinted = 1;
				 
				}else if (counter == 1 && alreadyPrinted == 1)
				{
					printf(" x %d",primes[index]);
				}
				index++;
				
			}
			if (alreadyPrinted == 0)
			{
				printf("%s",number);
			}       
			fprintf(stdout,"\n");      
			
			
			
			free(number);
			//free(afterDivision);
			number = getInput();
		}
		free(number);
	}else{
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

			printf("Prvociselny rozklad cisla %ld je:\n",inputNumber);
			int alreadyPrinted = 0;
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
}
