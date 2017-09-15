#ifndef MAXPOOL_LAYER_H
#define MAXPOOL_LAYER_H

#include "layer.h"

typedef layer maxpool_layer;
maxpool_layer make_maxpool_layer(int h, int w, int c, int size, int stride, int padding);

#endif
