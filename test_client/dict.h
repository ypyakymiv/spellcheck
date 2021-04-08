#ifndef DICT_HEADER_INCLUDED
#define DICT_HEADER_INCLUDED

#define DEFAULT_DICTIONARY "/usr/share/dict/words"

struct dict {
  char **words;
  int sz;
};

void load_dict(struct dict *, char *);
void free_dict(struct dict *);
int is_in_dict(struct dict *, char *);

struct dict dictionary;

#endif
