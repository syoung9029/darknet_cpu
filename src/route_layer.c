#include "route_layer.h"
#include "network.h"
#include "stdio.h"
#include "stdlib.h"

void copy_cpu(int N, float *X, float *Y)
{
    int i;
    for(i = 0; i < N; ++i)
        *(Y++) = *(X++);
}

void forward_route_layer(const route_layer l, network net)
{
    int i;
    int offset = 0;
    for(i = 0; i < l.n; ++i){
        int index = l.input_layers[i];
        float *input = net.layers[index].output;
        int input_size = l.input_sizes[i];
        copy_cpu(input_size, input, l.output + offset);
        offset += input_size;
    }
}

route_layer make_route_layer(int n, int *input_layers, int *input_sizes)
{
    fprintf(stderr,"route ");
    route_layer l = {0};
    l.type = ROUTE;
    l.n = n;
    l.input_layers = input_layers;
    l.input_sizes = input_sizes;
    int i;
    int outputs = 0;
    for(i = 0; i < n; ++i){
        fprintf(stderr," %d", input_layers[i]);
        outputs += input_sizes[i];
    }
    fprintf(stderr, "\n");
    l.outputs = outputs;
    l.inputs = outputs;
    l.output = calloc(outputs, sizeof(float));;

    l.forward = forward_route_layer;
    return l;
}
