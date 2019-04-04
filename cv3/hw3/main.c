#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
char *get_input() {
  int input_alloc = 10;
  char ch;
  char *string = (char *)malloc(input_alloc);
  int input_index = 0;
 
  while (1) {
    ch = getchar();
    if ((ch == '\n') || (ch == EOF) || (ch == '\0'))
      break;
    if (input_index >= input_alloc) {
      input_alloc *= 2;
      string = (char *)realloc(string, input_alloc);
    }
    string[input_index++] = ch;
  }
 
  string = (char *)realloc(string, (input_index + 1));
  string[input_index++] = '\0';
 
  return string;
}
 
int check_input(char *input, int len_of_input) {
  for (int i = 0; i < len_of_input; ++i) {
    if (!((input[i] >= 'A' && input[i] <= 'Z') ||
          (input[i] >= 'a' && input[i] <= 'z')))
      return -1;
  }
 
  return 0;
}
 
void fill_array(int *input) {
  for (int i = 0; i < 53; ++i) {
    *(input + i) = 0;
  }
}
 
int compare_strings(char *string1, char *string2) {
  int len = strlen(string1);
  int count = 0;
  for (int i = 0; i < len; ++i) {
    if (string1[i] == string2[i])
      count++;
  }
 
  return count;
}
 
int find_max_index(int *array, int size) {
  int max = 0;
  int index = 0;
  for (int k = 0; k < size; ++k) {
    if (array[k] > max) {
      max = array[k];
      index = k;
    }
  }
  return index;
}
 
int find_min(int *array, int size) {
  int min = array[0];
  for (int k = 0; k < size; ++k) {
    if (array[k] < min) {
      min = array[k];
    }
  }
  return min;
}
 
int find_min_index(int *array, int size) {
  int min = array[0];
  int index = 0;
  for (int k = 0; k < size; ++k) {
    if (array[k] <= min) {
      min = array[k];
      index = k;
    }
  }
  return index;
}
 
int lev_distance(char *inword, char *key, int lenght1, int lenght2) {
  int **word = (int **)malloc(sizeof(int *) * lenght1);
  for (int i = 0; i < lenght1; i++) {
    word[i] = (int *)malloc(sizeof(int) * lenght2);
  }
 
  for (int i = 0; i < lenght1; i++) {
    word[i][0] = i;
  }
 
  for (int i = 0; i < lenght2; i++) {
    word[0][i] = i;
  }
 
  for (int j = 1; j < lenght2; j++) {
    for (int i = 1; i < lenght1; i++) {
      if (inword[i - 1] == key[j - 1]) {
        word[i][j] = word[i - 1][j - 1];
      } else {
        int mini[3] = {word[i - 1][j] + 1, word[i][j - 1] + 1,
                       word[i - 1][j - 1] + 1};
        word[i][j] = find_min(mini, 3);
      }
    }
  }
  int ret = word[lenght1 - 1][lenght2 - 1];
  for (int i = 0; i < lenght1; i++) {
    free(word[i]);
  }
  free(word);
  return ret;
}
 
int main(int argc, char *argv[]) {
  // version switch, -prp -optional detection
  if (argc == 2) {
    if (strcmp(argv[1], "-prg-optional") == 0) {
      // optional part
      char *input1, *input2;
      int len1, len2, x, y;
      int distances[52];
      int start = 0;
      int final = 0;
      int index_of_min = 0;
 
      input1 = get_input();
      input2 = get_input();
      len1 = strlen(input1);
      len2 = strlen(input2);
      x = check_input(input1, len1);
      y = check_input(input2, len2);
      fill_array(distances);
 
      if ((x == -1) || (y == -1)) {
        fprintf(stderr, "Error: Chybny vstup!\n");
        free(input1);
        free(input2);
        return 100;
      }
 
      while (start < 53) {
        for (int j = 0; j < len1; ++j) {
          input1[j] = input1[j] + 1;
 
          if (input1[j] == 91)
            input1[j] = 97;
          if (input1[j] == 123)
            input1[j] = 65;
        }
        distances[start] = lev_distance(input1, input2, len1, len2);
        start++;
      }
 
      // printf("%s\n",input1);
      for (int j = 0; j < len1; j++) {
        input1[j] = input1[j] - 1;
      }
      index_of_min = find_min_index(distances, sizeof(distances) / sizeof(int));
 
      while (final <= index_of_min) {
        for (int j = 0; j < len1; ++j) {
          input1[j] = input1[j] + 1;
 
          if (input1[j] == 91)
            input1[j] = 97;
          if (input1[j] == 123)
            input1[j] = 65;
        }
        final++;
      }
      printf("%s\n", input1);
      free(input1);
      free(input2);
 
      return 0;
    }
  }
  // mandatory part
  char *input1, *input2;
  int distances[52];
  int len1, len2, x, y, index_of_max;
  int start = 0;
  int final = 0;
 
  input1 = get_input();
  input2 = get_input();
  len1 = strlen(input1);
  len2 = strlen(input2);
  x = check_input(input1, len1);
  y = check_input(input2, len2);
  fill_array(distances);
 
  if ((x == -1) || (y == -1)) {
    fprintf(stderr, "Error: Chybny vstup!\n");
    free(input1);
    free(input2);
    return 100;
  }
 
  if (len1 != len2) {
    fprintf(stderr, "Error: Chybna delka vstupu!\n");
    free(input1);
    free(input2);
    return 101;
  }
  while (start < 52) {
    for (int j = 0; j < len1; ++j) {
      input1[j] = input1[j] + 1;
 
      if (input1[j] == 91)
        input1[j] = 97;
      if (input1[j] == 123)
        input1[j] = 65;
    }
    // printf("%s \n", input1);
    distances[start] = compare_strings(input1, input2);
    start++;
  }
 
  index_of_max = find_max_index(distances, sizeof(distances) / sizeof(int));
 
  while (final <= index_of_max) {
    for (int j = 0; j < len1; ++j) {
      input1[j] = input1[j] + 1;
 
      if (input1[j] == 91)
        input1[j] = 97;
      if (input1[j] == 123)
        input1[j] = 65;
    }
    final++;
  }
  printf("%s\n", input1);
  free(input1);
  free(input2);
 
  return 0;
}
