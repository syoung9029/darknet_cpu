#include "network.h"
#include "stdlib.h"

network make_network(int n)
{
    network net = {0};
    net.n = n;
    net.layers = calloc(net.n, sizeof(layer));
    net.seen = calloc(1, sizeof(int));
    return net;
}

layer get_network_output_layer(network net)
{
    int i;
    for(i = net.n - 1; i >= 0; --i){
        if(net.layers[i].type != COST) break;
    }
    return net.layers[i];
}

void forward_network(network net)
{
    int i;
    for(i = 0; i < net.n; ++i){//run all types of forward layers
        layer l = net.layers[i];
        l.forward(l, net);//function pointer
        net.input = l.output;
    }
}

float *network_predict(network net, float *input)
{
    net.input = input;
    forward_network(net);//forward_network
    return net.output;
}

void free_network(network net)
{
    int i;
    for(i = 0; i < net.n; ++i){
        free_layer(net.layers[i]);
    }
    free(net.layers);
    if(net.input) free(net.input);
}
