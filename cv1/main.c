#include <stdio.h>
#include <stdlib.h>



void line (int);
void emptyBody(int, int);
void roof(int width);
int fullBody(int width, int counter);
void fenceBuilder(int fence);
void fenceMirror(int fence);



/* The main program */
int main(int argc, char *argv[])
{
  int width, height;
  
  if (scanf("%d %d",&width, &height) == 2)
  {
  	
  	if (3<= height && height <= 69 && 3<= width && width <= 69)
  	{
  		if (width % 2 == 1)
  		{
  			if (width == height)
  			{	
  				int fence;
  				int temp;
  				if (scanf("%d",&fence) == 1)
  				{
  					 if (fence < height && fence > 0)
  					 {
  					 	int counter = 0;
  					 	roof(width);
  						line(width);
  						printf("\n");
  						for (int i = 2; i < height-fence+1; i++ )
  						{
  							printf("X");
  							temp = fullBody(width,counter);
  							counter = temp;
  							printf("\n");
  						}
  						if (fence % 2 == 1)
  						{
  							printf("X");
  						temp = fullBody(width,counter);
  						counter = temp;
  						fenceMirror(fence);
  						for (int i = 2; i < fence; i++)
  						{
  							printf("X");
  							temp = fullBody(width,counter);
  							counter = temp;
  							for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf("|");
  								}else
  								{
  									printf(" ");
  								}
  							}
  							printf("\n"); 
  						}		
  						line(width);
  						fenceMirror(fence);
  						return 0;
  						}else
  						{
  						printf("X");
  						temp = fullBody(width,counter);
  						counter = temp;
  						fenceBuilder(fence);
  						for (int i = 2; i < fence; i++)
  						{
  							printf("X");
  							temp = fullBody(width,counter);
  							counter = temp;
  							for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf(" ");
  								}else
  								{
  									printf("|");
  								}
  							}
  							printf("\n"); 
  						}		
  						line(width);
  						fenceBuilder(fence);
  						return 0;
  						}
  					 }else
  					 {
  					 	fprintf(stderr,"Error: Neplatna velikost plotu!\n");
  					 	return 103;
  					 }
  				}else
  				{

  					fprintf(stderr,"Error: Chybny vstup!\n");
  					return 100;
  				}
  				 
  			}else
  			{
  				roof(width);
				line(width);
				printf("\n");
				emptyBody(width,height);
				line(width);
				printf("\n");
  				return 0;
  			}
  		}else
  		{
  			fprintf(stderr,"Error: Sirka neni liche cislo!\n");
  			return 102;
  		
  		}
  	}else
  	{
  		fprintf(stderr,"Error: Vstup mimo interval!\n");
  		return 101;
  	}
  }	
  else
  {

  	fprintf(stderr,"Error: Chybny vstup!\n");
  	return 100;
  }
}


void fenceMirror(int fence){
	for (int i = 0; i< fence; i++)
  	{
  		if (i%2 == 0)
  		{
  			printf("|");
  		}else{
  			printf("-");
  		}
  	}
  	printf("\n");

}


void fenceBuilder(int fence){
  	for (int i = 0; i< fence; i++)
  	{
  		if (i%2 == 0)
  		{
  		printf("-");
  		}else{
  		printf("|");
  		}
	}
	printf("\n");
}

int fullBody(int width, int counter){
	for (int i = 2; i< width; i++)
  	{
  		if (i%2 == counter%2)
  		{
  		printf("o");
  		}else{
  			printf("*");
  		}
  	}
  	printf("X");
  	counter+=1;
  	return counter;
}

void line (int width){
	for (int i = 0; i< width; i++)
	{
		printf("X");
	}
}

void emptyBody(int width, int height){
	for (int i = 2; i < height; i++ )
  	{
  	printf("X");
  		for (int i = 2; i< width; i++)
  		{
  		printf(" ");
  		}
  	printf("X\n");
	}
}

void roof(int width){
	int space = width / 2;
  	int roofSpace = 1;
  	for (int i = 0; i<space; i++)
  	{
  		printf(" ");
  	}
  	printf("X\n");
  	for (int i=space-1; i>0; i--)
  	{
  		for (int a= 0;a<i;a++)
  			{
  			printf(" ");
  			}
  		printf("X");
  		for (int b = 0; b< roofSpace; b++)
  		{
  			printf(" ");
  		}
  		roofSpace= roofSpace + 2;
  		printf("X\n");
  	}
}
