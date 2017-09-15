#include "stdio.h"
#include "stdlib.h"
#include "utils.h"
#include "list.h"
#include "data.h"

list *get_paths(char *filename)
{
    char *path;
    FILE *file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Couldn't open file: %s\n", filename);
        exit(0);
    }
    list *lines = make_list();
    while((path=fgetl(file))){
        list_insert(lines, path);
    }
    fclose(file);
    return lines;
}

char **get_labels(char *filename)
{
    list *plist = get_paths(filename);
    char **labels = (char **)list_to_array(plist);
    free_list(plist);
    return labels;
}
