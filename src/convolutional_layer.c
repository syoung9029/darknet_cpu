#include "convolutional_layer.h"
#include "math.h"
#include "utils.h"
#include "im2col.h"
#include "gemm.h"
#include "stdlib.h"
#include "network.h"

void swap_binary(convolutional_layer *l)
{
    float *swap = l->weights;
    l->weights = l->binary_weights;
    l->binary_weights = swap;
}

int convolutional_out_height(convolutional_layer l)
{
    return (l.h + 2*l.pad - l.size) / l.stride + 1;
}

int convolutional_out_width(convolutional_layer l)
{
    return (l.w + 2*l.pad - l.size) / l.stride + 1;
}

void normalize_cpu(float *x, float *mean, float *variance, int filters, int spatial)
{
    int f, i;
    for(f = 0; f < filters; ++f){
        int index_tmp = f*spatial;
        float mean_f = mean[f];
        float sqrt_variance_f = sqrt(variance[f]) + .000001f;
        for(i = 0; i < spatial; ++i){
            int index = index_tmp++;
            x[index] = (x[index] - mean_f)/sqrt_variance_f;
        }
    }
}

void scale_bias(float *output, float *scales, int n, int size)
{
    int i,j;
    for(i = 0; i < n; ++i){
        int i_tmp = i*size;
        float scales_tmp = scales[i];
        for(j = 0; j < size; ++j){
            output[i_tmp + j] *= scales_tmp;
        }
    }
}

void add_bias(float *output, float *biases, int n, int size)
{
    int i,j;
    for(i = 0; i < n; ++i){
        int i_tmp = i*size;
        float biases_tmp = biases[i];
        for(j = 0; j < size; ++j){
            output[i_tmp + j] += biases_tmp;
        }
    }
}

void forward_batchnorm_layer(layer l)
{
    normalize_cpu(l.output, l.rolling_mean, l.rolling_variance, l.out_c, l.out_h*l.out_w);
    scale_bias(l.output, l.scales, l.out_c, l.out_h*l.out_w);
    add_bias(l.output, l.biases, l.out_c, l.out_h*l.out_w);
}

void forward_convolutional_layer(convolutional_layer l, network net)
{
    int out_h = l.out_h;
    int out_w = l.out_w;

    int i;
    for(i = 0; i < l.outputs; ++i) l.output[i] = 0;

/*    if(l.xnor){
        binarize_weights(l.weights, l.n, l.c*l.size*l.size, l.binary_weights);
        swap_binary(&l);
        binarize_cpu(net.input, l.c*l.h*l.w, l.binary_input);
        net.input = l.binary_input;
    }*/

    int m = l.n;
    int k = l.size*l.size*l.c;
    int n = out_h*out_w;


    float *a = l.weights;
    float *b = net.workspace;
    float *c = l.output;

    im2col_cpu(net.input, l.c, l.h, l.w,
            l.size, l.stride, l.pad, b);//pull image to col according to the convs
    gemm(m,n,k,1,a,k,b,n,c,n);//weights multiply image col matrix
    c += n*m;
    net.input += l.c*l.h*l.w;

    if(l.batch_normalize){
        forward_batchnorm_layer(l);//forward_batch_normalize
    } else {
        add_bias(l.output, l.biases, l.n, out_h*out_w);
    }

    activate_array(l.output, m*n, l.activation);//activation
//    if(l.binary || l.xnor) swap_binary(&l);
}

static size_t get_workspace_size(layer l){
    return (size_t)l.out_h*l.out_w*l.size*l.size*l.c*sizeof(float);
}

convolutional_layer make_convolutional_layer(int h, int w, int c, int n, int size, int stride, int padding, ACTIVATION activation, int batch_normalize, int binary, int xnor)
{
    int i;
    convolutional_layer l = {0};
    l.type = CONVOLUTIONAL;

    l.h = h;
    l.w = w;
    l.c = c;
    l.n = n;
    l.binary = binary;
    l.xnor = xnor;
    l.stride = stride;
    l.size = size;
    l.pad = padding;
    l.batch_normalize = batch_normalize;

    l.weights = calloc(c*n*size*size, sizeof(float));
    l.biases = calloc(n, sizeof(float));

    l.nweights = c*n*size*size;
    l.nbiases = n;
    float scale = sqrt(2./(size*size*c));
    for(i = 0; i < c*n*size*size; ++i) l.weights[i] = scale*rand_normal();
    int out_w = convolutional_out_width(l);
    int out_h = convolutional_out_height(l);
    l.out_h = out_h;
    l.out_w = out_w;
    l.out_c = n;
    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = l.w * l.h * l.c;
    l.output = calloc(l.outputs, sizeof(float));

    l.forward = forward_convolutional_layer;
    if(binary){
        l.binary_weights = calloc(c*n*size*size, sizeof(float));
        l.scales = calloc(n, sizeof(float));
    }
    if(xnor){
        l.binary_weights = calloc(c*n*size*size, sizeof(float));
        l.binary_input = calloc(l.inputs, sizeof(float));
    }

    if(batch_normalize){
        l.scales = calloc(n, sizeof(float));
        for(i = 0; i < n; ++i){
            l.scales[i] = 1;
        }

        l.rolling_mean = calloc(n, sizeof(float));
        l.rolling_variance = calloc(n, sizeof(float));
    }

    l.workspace_size = get_workspace_size(l);
    l.activation = activation;

    fprintf(stderr, "conv  %5d %2d x%2d /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", n, size, size, stride, w, h, c, l.out_w, l.out_h, l.out_c);

    return l;
}
