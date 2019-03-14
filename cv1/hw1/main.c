#include <stdio.h>
#include <stdlib.h>



void

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
  				if (scanf("%d",&fence) == 1)
  				{
  					 if (fence < height && fence > 0)
  					 {
  					 	int space = width / 2;
  						int roofSpace = 1;
  						int counter = 0;
  						for (int i=0; i<space; i++)
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
  						for (int i = 0; i< width; i++)
  						{
  							printf("X");
  						}
  						printf("\n");
  						for (int i = 2; i < height-fence+1; i++ )
  						{
  							printf("X");
  							for (int i = 2; i< width; i++)
  							{
  								if (i%2 == counter%2)
  								{
  									printf("o");
  								}else
  								{
  									printf("*");
  								}
  							}
  							printf("X\n");
  							counter+=1;
  						}
  						if (fence % 2 == 1)
  						{
  							printf("X");
  						for (int i = 2; i< width; i++)
  							{
  								if (i%2 == counter%2)
  								{
  									printf("o");
  								}else
  								{
  									printf("*");
  								}
  							}
  							printf("X");
  							counter+=1;
  						for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf("|");
  								}else
  								{
  									printf("-");
  								}
  							}
  							
  							printf("\n");
  						for (int i = 2; i < fence; i++)
  						{
  							printf("X");
  							for (int i = 2; i< width; i++)
  							{
  								if (i%2 == counter%2)
  								{
  									printf("o");
  								}else
  								{
  									printf("*");
  								}
  							}
  							
  							printf("X");
  							counter += 1;
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
  						for (int i = 0; i< width; i++)
  						{
  							printf("X");
  						}
  						for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf("|");
  								}else
  								{
  									printf("-");
  								}
  							}
  						printf("\n");
  						return 0;
  						}else
  						{
  						printf("X");
  						for (int i = 2; i< width; i++)
  							{
  								if (i%2 == counter%2)
  								{
  									printf("o");
  								}else
  								{
  									printf("*");
  								}
  							}
  							printf("X");
  							counter += 1;
  						for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf("-");
  								}else
  								{
  									printf("|");
  								}
  							}
  							
  							printf("\n");
  						for (int i = 2; i < fence; i++)
  						{
  							printf("X");
  							for (int i = 2; i< width; i++)
  							{
  								if (i%2 == counter%2)
  								{
  									printf("o");
  								}else
  								{
  									printf("*");
  								}
  							}
  							counter += 1;
  							printf("X");
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
  						for (int i = 0; i< width; i++)
  						{
  							printf("X");
  						}
  						for (int i = 0; i< fence; i++)
  							{
  								if (i%2 == 0)
  								{
  									printf("-");
  								}else
  								{
  									printf("|");
  								}
  							}
  						printf("\n");
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
  				int space = width / 2;
  				int roofSpace = 1;
  				for (int i=0; i<space; i++)
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
  				for (int i = 0; i< width; i++)
  				{
  					printf("X");
  				}
  				printf("\n");
  				for (int i = 2; i < height; i++ )
  				{
  					printf("X");
  					for (int i = 2; i< width; i++)
  					{
  						printf(" ");
  					}
  					printf("X\n");
  				}
  				for (int i = 0; i< width; i++)
  				{
  					printf("X");
  				}
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
