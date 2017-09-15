#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "option_list.h"
#include "utils.h"

void option_insert(list *l, char *key, char *val)
{
    kvp *p = malloc(sizeof(kvp));
    p->key = key;
    p->val = val;
    p->used = 0;
    list_insert(l, p);
}

int read_option(char *s,list *options)
{
    size_t i;
    size_t len = strlen(s);
    char *val = 0;
    for(i=0;i<len;++i){
        if(s[i] == '='){
            s[i] = '\0';
            val = s+i+1;
            break;
        }
    }
    if(i == len-1) return 0;
    char *key = s;
    option_insert(options, key, val);
    return 1;
}

list *read_data_cfg(char *filename)
{
    FILE *file = fopen(filename,"r");
    if(file == 0){
        fprintf(stderr, "Couldn't open file: %s\n", filename);
        exit(0);
    }
    char *line;
    int nu = 0;
    list *options = make_list();
    while((line=fgetl(file)) != 0){
        ++nu;
        strip(line);
        switch (line[0]) {
        case '\0':
        case '#':
        case ';':
            free(line);
            break;
        default:
            if(!read_option(line,options)){
                fprintf(stderr,"Config file error line %d, could parse: %s\n",nu,line);
                free(line);
            }
            break;
        }
    }
    fclose(file);
    return options;
}

char *option_find(list *l, char *key)
{
    node *n = l->front;
    while(n){
        kvp *p = (kvp *)n->val;
        if(strcmp(p->key, key) == 0){
            p->used = 1;
            return p->val;
        }
        n = n->next;
    }
    return 0;
}

int option_find_int(list *l, char *key, int def)
{
    char *v = option_find(l, key);
    if(v) return atoi(v);
    fprintf(stderr, "%s: Using default '%d'\n", key, def);
    return def;
}

char *option_find_str(list *l, char *key, char *def)
{
    char *v = option_find(l, key);
    if(v) return v;
    if(def) fprintf(stderr, "%s: Using default '%s'\n", key, def);
    return def;
}

int option_find_int_quiet(list *l, char *key, int def)
{
    char *v = option_find(l, key);
    if(v) return atoi(v);
    return def;
}
