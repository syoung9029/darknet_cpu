#include <stdio.h>
#include <string.h>
#include "list.h"
#include "option_list.h"
#include "utils.h"
#include "yolo.h"

void run_detector(int argc,char **argv);

int main(int argc, char **argv)
{
    if(strcmp(argv[1],"detector")==0){
        run_detector(argc,argv);
    }else{
        fprintf(stderr,"Not an option:%s\n",argv[1]);
    }
    return 0;
}

void run_detector(int argc,char **argv)
{
    float thresh = find_float_arg(argc, argv, "-thresh", .24);
    int cam_index = find_int_arg(argc, argv, "-c", 0);
    int fullscreen = find_arg(argc, argv, "-fullscreen");

    char *datacfg = argv[3];
    char *cfg = argv[4];
    char *weights = (argc > 5) ? argv[5] : 0;
    char *filename = (argc > 6) ? argv[6] : 0;
    if(strcmp(argv[2],"demo")==0){
        list *options = read_data_cfg(datacfg);
        int classes = option_find_int(options, "classes", 1);
        char *name_list = option_find_str(options, "names", "voc.names");
        char **names = get_labels(name_list);
        yolo(cfg,weights,thresh,cam_index,filename,classes,names,fullscreen);
    }
}
