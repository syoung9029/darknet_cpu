#ifndef BOX_H
#define BOX_H

typedef struct{
    float x, y, w, h;
} box;

void do_nms(box *boxes, float **probs, int total, int classes, float thresh);
#endif
