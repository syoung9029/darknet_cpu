#ifndef NETWORK_H
#define NETWORK_H

#include "layer.h"

typedef struct network{
    int n;//the number of layers in a network
    int *seen;
    layer *layers;//pointer to a layer of a network
    float *output;//pointer to the output of a network

    int inputs;//the input of a network
    int outputs;//the number of outputs of a network
    int h, w, c;//the height,width and channels of a network(input layer)

    float *input;//pointer to feature map
    float *workspace;//pointer to the output of im2col_cpu
} network;

network make_network(int n);
layer get_network_output_layer(network net);
float *network_predict(network net, float *input);
void free_network(network net);
#endif
