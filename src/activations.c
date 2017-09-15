#include "activations.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

ACTIVATION get_activation(char *s)
{
    if (strcmp(s, "logistic")==0) return LOGISTIC;
    if (strcmp(s, "linear")==0) return LINEAR;
    if (strcmp(s, "leaky")==0) return LEAKY;
    fprintf(stderr, "Couldn't find activation function %s, going with ReLU\n", s);
    return BLANK_AC;
}

void activate_array(float *x, const int n, const ACTIVATION a)
{
    int i;
    for(i = 0; i < n; ++i){
        switch(a){
            case LINEAR:
            break;
            case LEAKY:
                x[i] = (x[i]>0) ? x[i] : .1*x[i];
            break;
            case LOGISTIC:
                x[i] =  1./(1. + exp(-x[i]));;
            break;
            case BLANK_AC:
            break;
        }
    }
}
