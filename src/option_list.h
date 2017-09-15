#ifndef OPTION_LIST_H
#define OPTION_LIST_H

typedef struct{
    char *key;
    char *val;
    int used;
}kvp;

list *read_data_cfg(char *filename);
char *option_find(list *l, char *key);
int option_find_int(list *l, char *key, int def);
char *option_find_str(list *l, char *key, char *def);
int read_option(char *s, list *options);
int option_find_int_quiet(list *l, char *key, int def);
#endif
