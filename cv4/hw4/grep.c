#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int length;
	char **array;
} array;

void print_lines(char *line, int lenght) {
  for (int i = 1; i < lenght; ++i) {
    printf("%c",line[i] );
  }
  printf("\n");
}


int str_len(char *input) {
  int count = 0;
	while (input[count] != '\0') {
		count ++;
	}
	return count;
}
int str_cmp(char *inp1, char *inp2){
  int index = 0;
  while (inp1[index] != '\0' || inp2[index] != '\0'){
    if (inp1[index] != inp2[index])
      return -1;
    index++;
  }
  return 0;
}

char* strCopy(char* input){
	int lenStr = str_len(input);
	char *str = (char*)malloc(sizeof(char)*(lenStr+1));
	for (int index = 0; index < lenStr; index++){
		str[index] = input[index];
	}
	str[lenStr] = '\0';
	
	return str;
}

char* getLine(FILE *f){
	char c = getc(f);
	int pointerSize = 20;
	int numberLength = 0; 
	char* pointer = (char*)malloc(pointerSize * sizeof(char));
	if (c == EOF){	
		free(pointer);
		return "EOF";
	}
	
	while (c != '\n')
	{
		pointer[numberLength] = c;
		numberLength++;
		if (numberLength == pointerSize){	
			pointerSize += 10;
			pointer = (char*)realloc(pointer,pointerSize*sizeof(char));
		}
		c = getc(f);
	}
	pointer = (char*)realloc(pointer,(numberLength+1)*sizeof(char));
	pointer[numberLength] = '\0';
	return pointer;
}

char* getLineStdin(void){
	char c = getchar();
	int pointerSize = 20;
	int numberLength = 0; 
	char* pointer = (char*)malloc(pointerSize * sizeof(char));
	if (c == EOF){	
		free(pointer);
		return "EOF";
	}
	
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


int find(char *longStr, char *wantedStr){
	int lenLong = str_len(longStr);
	int lenWanted = str_len(wantedStr);
	int match = 0;
	//printf("CURRENTLY PROCESSING:\n %s\n",longStr);
	//printf("variables:\n lenLong = %d ; lenWanted = %d\n",lenLong,lenWanted);
	//printf("WANTED: %s\n",wantedStr);	
	for (int index = 0; index < (lenLong - lenWanted + 1); index++){
		match = 0;
		for (int indexWant = 0; indexWant < lenWanted; indexWant++){
			if (longStr[index + indexWant] == wantedStr[indexWant]){
				match++;
				//printf("current index: %d ; current indexWant: %d\n",index, indexWant);
				//printf("left : %c right : %c\n",longStr[index + indexWant],wantedStr[indexWant]);
				if (match == lenWanted){
					//printf("MATCHED at index = %d indexWantes = %d \n",index, indexWant);
					return 1;
				}
			}else {
				match = 0;
			}
		}
	}
	return 0;
}

char identify(char *inputStr){
	int lenInp = str_len(inputStr);
	for (int index = 0; index < lenInp; index++){
		if (inputStr[index] == '?'){
			return '?';
		}
		if (inputStr[index] == '*'){
			return '*';
		}
		if (inputStr[index] == '+'){
			return '+';
		}
	}
	return 'X';

}

char* append(char* pointer, int length, char str){
	pointer = (char*)realloc(pointer,(length+1)*sizeof(char));
	pointer[length-1] = str;
	pointer[length] = '\0';
	return pointer;
} 	


char** questionSplit(char *input){
	int str1Len = 0;
	int str2Len;
	int lenInp = str_len(input);
	char *str1 = (char*)malloc(sizeof(char));
	char *str2 = (char*)malloc(sizeof(char));
	char **result = (char**)malloc(sizeof(char*)*2);
	int index = 0;
	str1[0] = '\0';
	str2[0] = '\0';
	for (; index < lenInp; index++){
		if (input[index] == '?'){
			break;
		}
		str1Len++;
		str1 = append(str1,str1Len,input[index]);
		str2 = append(str2,str1Len,input[index]);
	}
	str2Len = str1Len - 1;
	str2 = (char*)realloc(str2, sizeof(char)*(str2Len+1));
	str2[str2Len] = '\0';
	index++;
	for (; index < lenInp; index++){
		str1Len++;
		str2Len++;
		str1 = append(str1,str1Len,input[index]);
		str2 = append(str2,str2Len,input[index]);
	}
	result[0] = str1;
	result[1] = str2;
	//printf("%s ; %s\n",str1,str2);
	return result;
}

array* starSplit(char *input, int lineLen){
	array *arr = (array*)malloc(sizeof(array));
	arr->length = 0;
	arr->array = (char**)malloc(sizeof(char*));
	int beforeLen = 0;
	int afterLen = 0;
	int index = 0;
	int inpLen = str_len(input);
	char *beforeStr = (char*)malloc(sizeof(char));
	char *afterStr = (char*)malloc(sizeof(char));
	char last;
	beforeStr[0] = '\0';
	afterStr[0] = '\0';
	for(;index < inpLen; index++){
		if (input[index] == '*'){
			break;
		}
		beforeLen++;
		beforeStr = append(beforeStr, beforeLen, input[index]);
	}
	last = beforeStr[beforeLen-1];
	//printf("beforeStr: %s ; beforeLen: %d\n",beforeStr,beforeLen);
	//printf("This is last: %c\n",last);
	beforeStr = (char*)realloc(beforeStr, sizeof(char)*beforeLen);
	beforeLen--;
	beforeStr[beforeLen] = '\0';
	index++;
	for (; index < inpLen; index++){
		afterLen++;
		afterStr = append(afterStr, afterLen, input[index]);
	}
	for(int count = 0; (beforeLen+afterLen+count) <= lineLen; count++){
		int lastCount = 0;
		char *temp = strCopy(beforeStr);
		int tempLen = str_len(temp);
		while (lastCount != count){
			tempLen++;
			temp = append(temp, tempLen, last);
			lastCount++;
		}
		for (int add = 0; add < afterLen; add++){
			tempLen++;
			temp = append(temp, tempLen, afterStr[add]);
		}
		arr->length = count + 1;
		arr->array = (char**)realloc(arr->array,sizeof(char*)*(arr->length));
		arr->array[count] = temp;
		if (arr->length == 7){
			break;
		}

	}
	free(beforeStr);
	free(afterStr);
	return arr;
}

array* plusSplit(char *input, int lineLen){
	array *arr = (array*)malloc(sizeof(array));
	arr->length = 0;
	arr->array = (char**)malloc(sizeof(char*));
	int beforeLen = 0;
	int afterLen = 0;
	int index = 0;
	int inpLen = str_len(input);
	char *beforeStr = (char*)malloc(sizeof(char));
	char *afterStr = (char*)malloc(sizeof(char));
	char last;
	beforeStr[0] = '\0';
	afterStr[0] = '\0';
	for(;index < inpLen; index++){
		if (input[index] == '+'){
			break;
		}
		beforeLen++;
		beforeStr = append(beforeStr, beforeLen, input[index]);
	}
	last = beforeStr[beforeLen-1];
	//printf("beforeStr: %s ; beforeLen: %d\n",beforeStr,beforeLen);
	//printf("This is last: %c\n",last);
	index++;
	for (; index < inpLen; index++){
		afterLen++;
		afterStr = append(afterStr, afterLen, input[index]);
	}
	for(int count = 0; (beforeLen+afterLen+count) <= lineLen; count++){
		int lastCount = 0;
		char *temp = strCopy(beforeStr);
		int tempLen = str_len(temp);
		while (lastCount != count){
			tempLen++;
			temp = append(temp, tempLen, last);
			lastCount++;
		}
		for (int add = 0; add < afterLen; add++){
			tempLen++;
			temp = append(temp, tempLen, afterStr[add]);
		}
		arr->length = count + 1;
		arr->array = (char**)realloc(arr->array,sizeof(char*)*(arr->length));
		arr->array[count] = temp;
		if (arr->length == 7){
			break;
		}


	}
	free(beforeStr);
	free(afterStr);
	return arr;
}

void cleanMe(char** dirtyPointer, int length){
	for (int index = 0; index < length; index++){
		free(dirtyPointer[index]);		
	}
	free(dirtyPointer);
}


int main(int argc, char *argv[]) {

  if (argc == 4 && str_cmp(argv[1],"--color=always")== 0) {
    char *fname = argv[3];
    char *string = argv[2];
    FILE *f;
    if ((f = fopen(fname, "r")) == NULL) {
      return -1;
    }
    int indexes[200];
    for (int i = 0; i < 200; ++i){
      indexes[i] = 0;
    }
    int ind = 0;
    int count_index = 0;
    char c;
    int how_many = 0;
    int len = str_len(string);
    int zhoda = 0;
    int count = 1;
    int input_alloc = 10;
    int new_alloc = 0;
    char *line = (char *) calloc(input_alloc, sizeof(char));
    while ((c = getc(f)) != EOF) {
      if (count >= input_alloc){
        new_alloc = input_alloc * 2;
        line = (char *) realloc(line, new_alloc);
        for (int i = input_alloc; i < new_alloc; ++i) {
           line[i] = ' ';
        }
        input_alloc = new_alloc;
      }
      if (c == '\n') {
          //printf("count je %d\n", count );
          line = (char *) realloc(line, count);
          line = (char *) realloc(line, count+len+5);
          for (int i = count; i < count +len+5; ++i) {
             line[i] = ' ';
          }
          for (int i = 1; i < count; ++i) {
             for (int j = 0; j < len ; ++j) {
                  if(line[i+j] == string[j]){
                     zhoda ++;
                     if(j==0){
                       indexes[ind] = i;
                     }
                  }else{
                     zhoda = 0;

                  }
             }
             if (zhoda == len){
               ind++;
               count_index++;
               how_many++;
               zhoda = 0;

               }
          }
          if (count_index > 0){
            ind = 0;
              for (int i = 1; i < count ; ++i) {
                if (i != indexes[ind]) {
                  printf("%c",line[i] );

                }else{
                  printf("\033[01;31m\033[K%s\033[m\033[K", string);
                  i += len-1;
                  ind++;
                  count_index-- ;
                }
            }
            printf("\n");
          }//end if
          count = 0;
          c = ' ';
          input_alloc = 10;
          free (line);
          count_index = 0;
          ind = 0;
          line = (char *) calloc(input_alloc, sizeof(char));
          }
      line[count] = c;
      count++;
      }
      free(line);
      if (fclose(f) == EOF) {
        return -1;
      }
      if (how_many == 0)
        return 1;
      return 0;
  }
	if (argc == 4 && str_cmp(argv[1],"-E") == 0){
		int printed = 0;
		char *grepStr = argv[2];
		//printf("I am in corrext line\n");
		char *address = argv[3];
		FILE *f;
		char *line;
		if ((f = fopen(address, "r")) == NULL){
			return -1;
		}
		line = getLine(f);
		if (identify(grepStr) == '?'){
			char **result = questionSplit(grepStr);
			char *string1 = result[0];
			char *string2 = result[1];
			//printf("string1 = %s ; string2 = %s\n",string1,string2);
			do {
				if (find(line, string1) || find(line, string2)){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				line = getLine(f);
			}while (str_cmp(line,"EOF") != 0);
			cleanMe(result,2);
			fclose(f);	
		}
		if (identify(grepStr) == '*'){
			int lineLen;
			int match;
			array *result;
			//printf("string1 = %s ; string2 = %s\n",string1,string2);
			do {
				match = 0;
				lineLen = str_len(line);
				result = starSplit(grepStr,lineLen);
				//printf("result length: %d",result->length);
				for (int index = 0; index < result->length; index++){
					match += find(line, result->array[index]);
					//printf("INSIDE FOR: match: %d; line: %s; current_element: %s\n",match,line,result->array[index]);
				}
				//printf("line: %s; match: %d\n",line,match);
				if (match > 0){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				cleanMe(result->array,result->length);
				free(result);
				line = getLine(f);
			}while (str_cmp(line,"EOF") != 0);
			fclose(f);	
		}
		if (identify(grepStr) == '+'){
			int lineLen;
			int match;
			array *result;
			//printf("string1 = %s ; string2 = %s\n",string1,string2);
			do {
				match = 0;
				lineLen = str_len(line);
				result = plusSplit(grepStr,lineLen);
				//printf("result length: %d",result->length);
				for (int index = 0; index < result->length; index++){
					match += find(line, result->array[index]);
					//printf("INSIDE FOR: match: %d; line: %s; current_element: %s\n",match,line,result->array[index]);
				}
				//printf("line: %s; match: %d\n",line,match);
				if (match > 0){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				cleanMe(result->array,result->length);
				free(result);
				line = getLine(f);
			}while (str_cmp(line,"EOF") != 0);
			fclose(f);	
		}
		if (printed > 0){
			return 0;		
		}
		return 1;
	 }
	if (argc == 3 && str_cmp(argv[1],"-E") == 0){
		int printed = 0;
		char *grepStr = argv[2];
		//printf("I am in corrext line\n");
		char *line;
		line = getLineStdin();
		if (identify(grepStr) == '?'){
			char **result = questionSplit(grepStr);
			char *string1 = result[0];
			char *string2 = result[1];
			do {
				if (find(line, string1) || find(line, string2)){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				line = getLineStdin();
			}while (str_cmp(line,"EOF") != 0);
			cleanMe(result,2);
		}
		if (identify(grepStr) == '*'){
			int lineLen;
			int match;
			array *result;
			//printf("string1 = %s ; string2 = %s\n",string1,string2);
			do {
				match = 0;
				lineLen = str_len(line);
				result = starSplit(grepStr,lineLen);
				//printf("result length: %d",result->length);
				for (int index = 0; index < result->length; index++){
					match += find(line, result->array[index]);
					//printf("INSIDE FOR: match: %d; line: %s; current_element: %s\n",match,line,result->array[index]);
				}
				//printf("line: %s; match: %d\n",line,match);
				if (match > 0){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				cleanMe(result->array,result->length);
				free(result);
				line = getLineStdin();
			}while (str_cmp(line,"EOF") != 0);
		}
		if (identify(grepStr) == '+'){
			int lineLen;
			int match;
			array *result;
			//printf("string1 = %s ; string2 = %s\n",string1,string2);
			do {
				match = 0;
				lineLen = str_len(line);
				result = plusSplit(grepStr,lineLen);
				//printf("result length: %d",result->length);
				for (int index = 0; index < result->length; index++){
					match += find(line, result->array[index]);
					//printf("INSIDE FOR: match: %d; line: %s; current_element: %s\n",match,line,result->array[index]);
				}
				//printf("line: %s; match: %d\n",line,match);
				if (match > 0){
					printf("%s\n",line);
					printed++;
				}
				free(line);
				cleanMe(result->array,result->length);
				free(result);
				line = getLineStdin();
			}while (str_cmp(line,"EOF") != 0);
		}
		if (printed > 0){
			return 0;		
		}
		return 1;
	 }


  if (argc == 3 && str_cmp(argv[1],"--color=always")== 0) {
    char *string = argv[2];
    int indexes[20];
    for (int i = 0; i < 20; ++i){
      indexes[i] = 0;
    }
    int ind = 0;
    int count_index = 0;
    char c;
    int how_many = 0;
    int len = str_len(string);
    int zhoda = 0;
    int count = 1;
    int input_alloc = 10;
    int new_alloc = 0;
    char *line = (char *) calloc(input_alloc, sizeof(char));
    while ((c = getchar()) != EOF) {
      if (count >= input_alloc){
        new_alloc = input_alloc * 2;
        line = (char *) realloc(line, new_alloc);
        for (int i = input_alloc; i < new_alloc; ++i) {
           line[i] = ' ';
        }
        input_alloc = new_alloc;
      }
      if (c == '\n') {
          //printf("count je %d\n", count );
          line = (char *) realloc(line, count);
          line = (char *) realloc(line, count+len);
          for (int i = count; i < count +len; ++i) {
             line[i] = ' ';
          }
          for (int i = 1; i < count; ++i) {
             for (int j = 0; j < len ; ++j) {
                  if(line[i+j] == string[j]){
                     zhoda ++;
                     if(j==0){
                       indexes[ind] = i;
                     }
                  }else{
                     zhoda = 0;

                  }
             }
             if (zhoda == len){
               ind++;
               count_index++;
               how_many++;
               zhoda = 0;

               }
          }
          if (count_index > 0){
            ind = 0;
              for (int i = 1; i < count ; ++i) {
                if (i == indexes[ind]) {
                  printf("\033[01;31m\033[K%s\033[m\033[K", string);
                  i += len;
                  ind++;

                }
              printf("%c",line[i] );
            }
            printf("\n");

          }//end if
          count = 0;
          c = ' ';
          input_alloc = 10;
          free (line);
          count_index = 0;
          ind = 0;
          line = (char *) calloc(input_alloc, sizeof(char));
          }
      line[count] = c;
      count++;
      }
      free(line);

        if (how_many == 0)
           return 1;
      return 0;
  }


    if (argc == 3){
      char *fname = argv[2];
		  char *string = argv[1];
      FILE *f;
      if ((f = fopen(fname, "r")) == NULL) {
        return -1;
      }
      char c;
		  int how_many = 0;
		  int len = str_len(string);
		  int zhoda = 0;
      int count = 1;
      int input_alloc = 10;
		  int new_alloc = 0;
      char *line = (char *) calloc(input_alloc, sizeof(char));
      while ((c = getc(f)) != EOF) {
        if (count >= input_alloc){
          new_alloc = input_alloc * 2;
          line = (char *) realloc(line, new_alloc);
				  for (int i = input_alloc; i < new_alloc; ++i) {
					   line[i] = ' ';
				  }
				  input_alloc = new_alloc;
        }
        if (c == '\n') {
		    line = (char *) realloc(line, count);
            line = (char *) realloc(line, count+len);
            for (int i = count; i < count +len; ++i) {
					     line[i] = ' ';
				    }
				    for (int i = 1; i < count; ++i) {
			         for (int j = 0; j < len ; ++j) {
			              if(line[i+j] == string[j])
			                 zhoda ++;
			              else
			                 zhoda = 0;
			              }
			         if (zhoda == len){
			         print_lines(line, count);
						   how_many++;
			         zhoda = 0;
						   break;
					     }
			      }
				    count = 0;
				    c = ' ';
				    input_alloc = 10;
				    free (line);
				    line = (char *) calloc(input_alloc, sizeof(char));
			      }
        line[count] = c;
        count++;
        }
        free(line);
        if (fclose(f) == EOF) {
          return -1;
        }
		      if (how_many == 0)
			       return 1;
        return 0;
    }
    if (argc == 2) {

      char *string = argv[1];
      char c;
		  int how_many = 0;
		  int len = str_len(string);
		  int zhoda = 0;
      int count = 1;
      int input_alloc = 10;
		  int new_alloc = 0;
      char *line = (char *) calloc(input_alloc, sizeof(char));
      while ((c = getchar()) != EOF) {
        if (count >= input_alloc){
          new_alloc = input_alloc * 2;
          line = (char *) realloc(line, new_alloc);
				  for (int i = input_alloc; i < new_alloc; ++i) {
					   line[i] = ' ';
				  }
				  input_alloc = new_alloc;
        }
        if (c == '\n') {
				    line = (char *) realloc(line, count);
            line = (char *) realloc(line, count+len);
            for (int i = count; i < count +len; ++i) {
					     line[i] = ' ';
				    }
				    for (int i = 1; i < count; ++i) {
			         for (int j = 0; j < len ; ++j) {
			              if(line[i+j] == string[j])
			                 zhoda ++;
			              else
			                 zhoda = 0;
			              }
			         if (zhoda == len){
			         print_lines(line, count);
						   how_many++;
			         zhoda = 0;
						   break;
					     }
			      }
				    count = 0;
				    c = ' ';
				    input_alloc = 10;
				    free (line);
				    line = (char *) calloc(input_alloc, sizeof(char));
			      }
        line[count] = c;
        count++;
        }
        free(line);

		      if (how_many == 0)
			       return 1;
        return 0;

    }
}
