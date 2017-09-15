#ifndef LAYER_H
#define LAYER_H

#include "activations.h"
#include "stddef.h"

typedef enum {
    CONVOLUTIONAL,
    MAXPOOL,
    ROUTE,
    COST,
    NETWORK,
    REGION,
    REORG,
    BLANK
} LAYER_TYPE;

typedef enum{
    SSE, MASKED, L1, SMOOTH
} COST_TYPE;

typedef struct network network;

typedef struct layer{ // layer
    LAYER_TYPE type; //define the type of layers
    ACTIVATION activation; //define the type of activation
    COST_TYPE cost_type; //regulation methods
    void (*forward) (struct layer, struct network);//function pointer to all layers of forward
    int batch_normalize; //batch normalization
    int inputs;//l.w*l.h*l.c
    int outputs;//l.out_w*l.out_h*l.out_c
    int nweights;//c*n*size*size,the number of weights in a layer
    int nbiases;//number of biases in a layer
    int h,w,c;//h:height of feature map;w:width of feature map;c:number of channels of feature map in a layer
    int out_h, out_w, out_c;//out_h:the height of feature map in the output of a layer;out_c:the channels of feature map in the output of a layer
    int n;//the number of filters(convs)
    int size;//the size of a filter
    int stride;//the stride of the movement of a filter
    int pad;//add 0 around a layer according to the size of a filter
    int binary;//binary of weights**************************************************
    int xnor;//binary weights and feature map****************************************
    int classes;//the number of classes of objects
    int coords;//coords of regions of objects
    int   * indexes;//maxpool_layer, calloc(output_size, sizeof(int));
    int   * input_layers;//route_layer, input_layers
    int   * input_sizes;//route_layer, input_sizes
    float * binary_weights;//the pointer to binary weights**********************************
    float * biases;//pointer to biases
    float * weights;//pointer to weights
    float * output;//pointer to output
    /***********************batch_normalize*****************************/
    float * scales;//pointer to scales,used to batch_normalize
    float * rolling_mean;//batch_normalize, pointer to mean
    float * rolling_variance;//batch_normalize, pointer to variance

    float * binary_input;//the pointer to binary inputs**********************************
    size_t workspace_size;//long unsigned int;workspace_size = l.out_h*l.out_w*l.size*l.size*l.c*sizeof(float)
}layer;

void free_layer(layer);

#endif
