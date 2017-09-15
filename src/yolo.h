#ifndef YOLO_H
#define YOLO_H

#define NUM_THREADS 5
#define QueueSize NUM_THREADS
#define AVERAGE 300

#include "image.h"
void yolo(char *cfgfile, char *weightfile, float thresh_obj, int cam_index, const char *filename, int classes_obj, char **names_obj, int fullscreen);

typedef struct{
    image queue[QueueSize];// 用于保存类型为DataType队列元素的数组
    int front,rear;// 用于保存队头和队尾下标信息
}SeqQueue_image;

typedef struct{
    IplImage* queue[QueueSize];// 用于保存类型为DataType队列元素的数组
    int front,rear;// 用于保存队头和队尾下标信息
}SeqQueue_Ipl;

void InitQueue(SeqQueue_image *SQ);
int QueueEmpty(SeqQueue_image SQ);
int EnterQueue(SeqQueue_image *SQ,image e);
int DeleteQueue(SeqQueue_image *SQ,image *e);

void InitQueue_Ipl(SeqQueue_Ipl *SQ);
int QueueEmpty_Ipl(SeqQueue_Ipl SQ);
int EnterQueue_Ipl(SeqQueue_Ipl *SQ,IplImage* e);
int DeleteQueue_Ipl(SeqQueue_Ipl *SQ,IplImage**e);
#endif
