#include "region_layer.h"
#include "layer.h"
#include "string.h"
#include "network.h"
#include "stdio.h"
#include "math.h"
#include "box.h"
#include "stdlib.h"

int entry_index(layer l, int location, int entry)
{
    int n =   location / (l.w*l.h);
    int loc = location % (l.w*l.h);
    return n*l.w*l.h*(l.coords+l.classes+1) + entry*l.w*l.h + loc;
}

void forward_region_layer(const layer l, network net)
{
    int n;
    memcpy(l.output, net.input, l.outputs*sizeof(float));

    for(n = 0; n < l.n; ++n){
        int index = entry_index(l, n*l.w*l.h, 0);
        activate_array(l.output + index, 2*l.w*l.h, LOGISTIC);
        index = entry_index(l, n*l.w*l.h, 4);
        activate_array(l.output + index,   l.w*l.h, LOGISTIC);
    }
}

layer make_region_layer(int w, int h, int n, int classes, int coords)
{
    layer l = {0};
    l.type = REGION;

    l.n = n;
    l.h = h;
    l.w = w;
    l.c = n*(classes + coords + 1);
    l.out_w = l.w;
    l.out_h = l.h;
    l.out_c = l.c;
    l.classes = classes;
    l.coords = coords;
    l.biases = calloc(n*2, sizeof(float));
    l.outputs = h*w*n*(classes + coords + 1);
    l.inputs = l.outputs;
    l.output = calloc(l.outputs, sizeof(float));
    int i;
    for(i = 0; i < n*2; ++i){
        l.biases[i] = .5;
    }

    l.forward = forward_region_layer;

    fprintf(stderr, "detection\n");

    return l;
}

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h, int stride)
{
    box b;
    b.x = (i + x[index]) / w;
    b.y = (j + x[index + stride]) / h;
    b.w = exp(x[index + 2*stride]) * biases[2*n]   / w;
    b.h = exp(x[index + 3*stride]) * biases[2*n+1] / h;
    return b;
}

void correct_region_boxes(box *boxes, int n, int w, int h, int netw, int neth)
{
    int i;
    int new_w=0;
    int new_h=0;
    if (((float)netw/w) < ((float)neth/h)) {
        new_w = netw;
        new_h = (h * netw)/w;
    } else {
        new_h = neth;
        new_w = (w * neth)/h;
    }
    for (i = 0; i < n; ++i){
        box b = boxes[i];
        b.x =  (b.x - (netw - new_w)/2./netw) / ((float)new_w/netw);
        b.y =  (b.y - (neth - new_h)/2./neth) / ((float)new_h/neth);
        b.w *= (float)netw/new_w;
        b.h *= (float)neth/new_h;
        boxes[i] = b;
    }
}

void get_region_boxes(layer l, int w, int h, int netw, int neth, float thresh, float **probs, box *boxes)
{
    int i,j,n;
    float *predictions = l.output;
    int l_w_tmp = l.w*l.h;
    for (i = 0; i < l_w_tmp; ++i){
        int row = i / l.w;
        int col = i % l.w;
        for(n = 0; n < l.n; ++n){
            int n_tmp = n*l_w_tmp;
            int index = n_tmp + i;
            for(j = 0; j < l.classes; ++j){
                probs[index][j] = 0;
            }
            int obj_index = entry_index(l, index, 4);
            int box_index = entry_index(l, index, 0);
            float scale = predictions[obj_index];
            boxes[index] = get_region_box(predictions, l.biases, n, box_index, col, row, l.w, l.h, l_w_tmp);

            float max = 0;
            for(j = 0; j < l.classes; ++j){
                int class_index = entry_index(l, n*l.w*l.h + i, 5 + j);
                float prob = scale*predictions[class_index];
                probs[index][j] = (prob > thresh) ? prob : 0;
                if(prob > max) max = prob;
            }
            probs[index][l.classes] = max;
            probs[index][0] = scale;
        }
    }
    correct_region_boxes(boxes, l.w*l.h*l.n, w, h, netw, neth);
}
