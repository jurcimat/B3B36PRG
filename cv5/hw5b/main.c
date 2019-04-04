#include <stdio.h>
#include <stdlib.h>
 
 
typedef struct {
    short r;
    short s;
    char namae;
    int **mat;
} matrix;
 
matrix * memoryCreator(int r, int s){
    matrix * matica = (matrix *)malloc(sizeof(matrix));
    matica->r = r;
    matica->s = s;
    matica->mat = (int **)malloc(sizeof(int *)*r);
    for (int i =0; i< r;i++ ){
        matica->mat[i]=(int*)calloc(sizeof(int),s);
    }
    return matica;
}
 
void cisticVictor(matrix* mat){
    for (int i =0; i< mat->r;i++){
        free(mat->mat[i]);
    }
    free(mat->mat);
    free(mat);
}
 
 
void fill(matrix* mat,char* inp_list){
 
    int curr = 0;
    mat->namae = inp_list[curr];
    curr += 2;
    for (int i = 0; i< mat->r; i++){
        for (int j = 0; j < mat->s;j++){
            int c_all = 10;
            char* new = (char*)malloc(sizeof(char)*c_all);
            int x = 0;
            while ((inp_list[curr] < 48 || inp_list[curr] > 57) && inp_list[curr] != '-'){
                curr++;
 
            }
            while((inp_list[curr] >= 48 && inp_list[curr] <= 57) || inp_list[curr] == '-'){
                new[x] = inp_list[curr];
                x++;
                if (x == c_all){
                    c_all += 10;
                    new = (char*)realloc(new,sizeof(char)*c_all);
                }
                curr++;
 
 
            }
 
            new = (char*)realloc(new,(x+1)*sizeof(char));
            new[x] = '\0';
            int a = 0;
             
            a = strtol(new,NULL,10);
 
            mat->mat[i][j] = a;
            free(new);
        }
    }
}
 
void printMatrix(matrix *mat){
 
    printf("[");
    for (int i = 0; i< mat->r; i++){
        for (int j = 0; j < mat->s;j++){
            (i == 0 && j == 0) ? printf("%i",mat->mat[i][j]) : printf(" %i",mat->mat[i][j]);
            }
        if (i != mat->r - 1){ 
        printf(";");
        }
        }
    printf("]\n");      
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
 
matrix* find(matrix** arr, int max, char c){
    for (int i = 0;i < max ;i++){
        if (arr[i]->namae == c){
            return arr[i];
        }
    }
    return NULL;
}
 
matrix* copy(matrix* mat){
     
    matrix* copy = (matrix*)malloc(sizeof(matrix));
    copy->r = mat->r;
    copy->s = mat->s;
    copy->namae = mat->namae;
    copy->mat = (int **)malloc(sizeof(int *)*copy->r);
    for (int i =0; i< copy->r; i++){
        copy->mat[i]=(int*)calloc(sizeof(int),copy->s);
    }
    for (int k = 0; k< mat->r; k++){
        for (int l = 0; l < mat->s;l++){
            copy->mat[k][l] = mat->mat[k][l];
        }
    }
    return copy;
}
void final_clean(matrix** arr, int num){
    for (int i = 0; i < num-1; i++){
        cisticVictor(arr[i]);
    }
    free(arr);
}
 
 
 
int main(int argc, char *argv[])
{
  char c;
  int struct_all = 1;
  matrix** struct_arr = (matrix**)malloc(sizeof(matrix*)*struct_all);
  int mat_num = 0;
  while ((c=getchar()) != '\n'){
    int mall_len = 30;
    int real_len = 0;
    char in;
    char* input_mat = (char*)malloc(mall_len*sizeof(char));
    input_mat[real_len] = c;
    real_len++;
    while (1){
        scanf("%c",&in);
        if (in == '\n'){
            break;
        }
         
        input_mat[real_len] = in;
        real_len++;
        if (real_len == mall_len){
            mall_len += 30;
            input_mat = (char*)realloc(input_mat,sizeof(char)*mall_len);        
        }
    }
    input_mat = (char*)realloc(input_mat,sizeof(char)*real_len);
 
    int n = 0;
    int r=0,s=0;
    while(input_mat[n] != ']'){
 
        if (input_mat[n] == ';'){
            r++;
        }
        if (input_mat[n] == ' '){
            s++;
        }
        n++;    
    }
    r++;
    s++;
    s/=r;
 
    matrix* first_mat = memoryCreator(r,s);
    fill(first_mat,input_mat);
 
    struct_arr[mat_num] = first_mat;
 
    mat_num++;
    if (mat_num == struct_all){
        struct_all++;
        struct_arr = (matrix**)realloc(struct_arr,sizeof(matrix)*struct_all);
     
    }
    free(input_mat);
    input_mat = NULL;
 
  }
  c = getchar();
  matrix* firstPoint = find(struct_arr,struct_all,c);
  matrix* first_mat = copy(firstPoint);
  char operator, operator2;
  scanf("%c",&operator);
  c = getchar();
  matrix* secPoint = find(struct_arr,struct_all,c);
  matrix* sec_mat = copy(secPoint);
  while (operator == '+' || operator == '-' || operator == '*'){
    operator2 = getchar();
    if (operator == '+' || operator == '-'){
        if (operator2 != '*'){
            if (operator == '+'){
                first_mat = addMatrix(first_mat,sec_mat);
            }else if (operator == '-'){
                first_mat = subMatrix(first_mat,sec_mat);
            }
        }else if (operator2 == '*'){
            while (operator2 == '*'){
 
                c = getchar();
 
                matrix* thirdMatrix = find(struct_arr,struct_all,c);
                matrix* third_mat = copy(thirdMatrix);
                sec_mat = mulMatrix(sec_mat,third_mat);
                operator2 = getchar();
            }
 
            if (operator == '+'){
                first_mat = addMatrix(first_mat,sec_mat);
            }else if (operator == '-'){
                first_mat = subMatrix(first_mat,sec_mat);
            }
        }       
    }else if (operator == '*'){
        first_mat = mulMatrix(first_mat,sec_mat);
    }
    operator = operator2;
    if (operator == '+' || operator == '-' || operator == '*'){
        c = getchar();
        matrix* sec_point = find(struct_arr,struct_all,c);
        sec_mat = copy(sec_point);
    }
 
 }
 printMatrix(first_mat);
 cisticVictor(first_mat);
 final_clean(struct_arr,struct_all);
 return 0;
}
