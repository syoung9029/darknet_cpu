#ifndef IMAGE_H
#define IMAGE_H

#include "box.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/core/version.hpp"

typedef struct {
    int h;//height of the feature map
    int w;//width of the feature map
    int c;//width of the feature map
    float *data;//pointer to data of the feature map
} image;

image ipl_to_image(IplImage* src);
void rgbgr_image(image im);
void free_image(image m);
void draw_detections(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes);
void show_image(image p, const char *name);
#endif
