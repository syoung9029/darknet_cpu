#include "layer.h"
#include <stdlib.h>

void free_layer(layer l)
{
    if(l.indexes)            free(l.indexes);
    if(l.input_layers)       free(l.input_layers);
    if(l.input_sizes)        free(l.input_sizes);
    if(l.binary_weights)     free(l.binary_weights);
    if(l.biases)             free(l.biases);
    if(l.scales)             free(l.scales);
    if(l.weights)            free(l.weights);
    if(l.output)             free(l.output);
    if(l.rolling_mean)       free(l.rolling_mean);
    if(l.rolling_variance)   free(l.rolling_variance);
    if(l.binary_input)       free(l.binary_input);
}
