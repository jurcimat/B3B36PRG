#include <stdio.h>
#include <stdlib.h>

/* The main program */
typedef struct {
	short r;
	short s;
	int **mat;
	

} matrix;

matrix * memoryCreator(int r, int s){
	matrix* matica = (matrix *)malloc(sizeof(matrix));
	matica->r = r;
	matica->s = s;
	matica->mat = (int **)malloc(sizeof(int *)*r);
	for (int i =0; i< r;i++ ){
		matica->mat[i]=(int*)calloc(sizeof(int),s);
	}
	return matica;
}

void cisticVictor(matrix* mat){
	for (int i =0; i< mat->r;i++ ){
		free(mat->mat[i]);
	}
	free(mat->mat);
	free(mat);
}

void fill(matrix* mat){
	for (int i = 0; i< mat->r; i++){
		for (int j = 0; j < mat->s;j++){
			if(scanf("%i",&mat->mat[i][j]) != 1){
			fprintf(stderr,"Error: Chybny vstup!\n");
			cisticVictor(mat);
			exit(100);
			}
		}
	}
	char err;
	scanf("%c",&err);
}

void printMatrix(matrix *mat){
	printf("%i %i\n",mat->r,mat->s);
	for (int i = 0; i< mat->r; i++){
		for (int j = 0; j < mat->s;j++){
			(j == mat->s - 1) ? printf("%i",mat->mat[i][j]) : printf("%i ",mat->mat[i][j]);
			}
		printf("\n");
		}		
}

matrix* mulMatrix(matrix* mat1,matrix* mat2){
	if (mat1->s != mat2->r){
		fprintf(stderr,"Error: Chybny vstup!\n");
		cisticVictor(mat1);
		cisticVictor(mat2);
		exit(100);
	}
	matrix* final = memoryCreator(mat1->r,mat2->s);
	int sum  = 0;
	for (int it1 = 0;it1 < mat1->r;it1++){
		for (int it2= 0; it2< mat2->s;it2++){
			for (int it3 = 0; it3 < mat2->r;it3++){
				sum += mat1->mat[it1][it3]*mat2->mat[it3][it2];
			}
			final->mat[it1][it2]=sum;
			sum = 0;
		}
	}
	cisticVictor(mat1);
	cisticVictor(mat2);
	return final;
}

matrix* addMatrix(matrix* mat1, matrix* mat2){
	if (mat1->r != mat2->r || mat1->s != mat2->s){
		fprintf(stderr,"Error: Chybny vstup!\n");
		cisticVictor(mat1);
		cisticVictor(mat2);
		exit(100);
	}
	matrix* final = memoryCreator(mat1->r,mat1->s);
	for (int row = 0;row < mat1->r;row++){
		for (int column = 0; column < mat1->s;column++){
			final->mat[row][column] = mat1->mat[row][column]+mat2->mat[row][column];
		}
	}
	cisticVictor(mat1);
	cisticVictor(mat2);
	return final;
}

matrix* subMatrix(matrix* mat1, matrix* mat2){
	if (mat1->r != mat2->r || mat1->s != mat2->s){
		fprintf(stderr,"Error: Chybny vstup!\n");
		cisticVictor(mat1);
		cisticVictor(mat2);
		exit(100);
	}
	matrix* final = memoryCreator(mat1->r,mat1->s);
	for (int row = 0;row < mat1->r;row++){
		for (int column = 0; column < mat1->s;column++){
			final->mat[row][column] = mat1->mat[row][column]-mat2->mat[row][column];
		}
	}
	cisticVictor(mat1);
	cisticVictor(mat2);
	return final;
}

int main(int argc, char *argv[])
{
  int r=0,s =0;
  scanf("%d %d",&r,&s);
  matrix* firstMatrix = memoryCreator(r,s);
  fill(firstMatrix);
  char operator, operator2='a';
  scanf("%c",&operator);
  if (operator != '+' && operator != '-' && operator != '*'){
  	fprintf(stderr,"Error: Chybny vstup!\n");
	cisticVictor(firstMatrix);
	return 100;
  }
  scanf("%d %d",&r,&s);
  matrix* secondMatrix = memoryCreator(r,s);
  fill(secondMatrix);

  while (operator == '+' || operator == '-' || operator == '*'){

  	operator2 = getchar();
  	if (operator == '+' || operator == '-'){
  		if (operator2 != '*'){
  			if (operator == '+'){
  				firstMatrix = addMatrix(firstMatrix,secondMatrix);

  			}else if (operator == '-'){
  				firstMatrix = subMatrix(firstMatrix,secondMatrix);
  			}
  		}else if (operator2 == '*'){

  			while (operator2 != '+' && operator2 != '-' && (int)operator2 != -1){
  				scanf("%d %d",&r,&s);

  				matrix* thirdMatrix = memoryCreator(r,s);
  				fill(thirdMatrix);
  				secondMatrix = mulMatrix(secondMatrix,thirdMatrix);
  				operator2= getchar();

  			}
  			if (operator == '+'){
  				firstMatrix = addMatrix(firstMatrix,secondMatrix);
  			}else if (operator == '-'){
  				firstMatrix = subMatrix(firstMatrix,secondMatrix);
  			}
  		} 		
  	}else if (operator == '*'){
  		firstMatrix = mulMatrix(firstMatrix,secondMatrix);
  	}
  	operator = operator2;
  	if (operator == '+' || operator == '-' || operator == '*'){
  	  	scanf("%d %d",&r,&s);
  		secondMatrix = memoryCreator(r,s);
  		fill(secondMatrix);

  	}
 }
 printMatrix(firstMatrix);
 cisticVictor(firstMatrix);
 return 0;
}

