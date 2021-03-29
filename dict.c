#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dict.h"
#include "utils.h"

int streq(char *, char *);

void load_dict(struct dict *d, char *filename) {
  const int START_BUFFER_SIZE = 1024;
  
  int alloc_sz = START_BUFFER_SIZE;
  int curr_sz = 0;

  char **words = ec_malloc(sizeof(char *) * alloc_sz);

  FILE *f = ec_fopen(filename, "r");
  char *line = NULL;
  size_t line_sz = 0;
  while(getline(&line, &line_sz, f)) {
    if(curr_sz >= alloc_sz) { // try realloc
      int new_alloc_sz = 2 * alloc_sz;
      char **new_words = ec_malloc(sizeof(char *) * new_alloc_sz);
      memcpy(new_words, words, sizeof(char *) * curr_sz);
      free(words);
      words = new_words;
      alloc_sz = new_alloc_sz;
    }

    words[curr_sz] = line;
    curr_sz++;
 
    line = NULL;
    line_sz = 0;
  }
  
  d->words = words;
  d->sz = curr_sz;
}

void free_dict(struct dict *d) {
  for(int i = 0; i < d->sz; i++) {
    free(d->words[i]);
  }
  free(d);
}

int is_in_dict(struct dict *d, char *word) {
  for(int i = 0; i < d->sz; i++) {
     if(streq(d->words[i], word)) { // if list is guarenteed alpha sorted we can do better
       return 1;
     }
  }
  return 0;
}

int streq(char *a, char *b) {
  int i = 0;
  while(a[i]) {
    if(a[i] != b[i]) return 0;
    i++;
  }
  if(!b[i]) return 1;
  else return 0;
}
