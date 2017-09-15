#ifndef REGION_LAYER_H
#define REGION_LAYER_H

#include "layer.h"
#include "box.h"

layer make_region_layer(int w, int h, int n, int classes, int coords);
void get_region_boxes(layer l, int w, int h, int netw, int neth, float thresh, float **probs, box *boxes);
#endif
