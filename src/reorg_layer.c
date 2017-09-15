#include "reorg_layer.h"
#include "network.h"
#include "stdio.h"
#include "stdlib.h"

void reorg_cpu(float *x, int w, int h, int c, int stride, float *out)
{
    int i,j,k;
    int out_c = c/(stride*stride);
    int h_tmp_tmp = h*stride;
    for(k = 0; k < c; ++k){
        int h_tmp = h*k;
        int c2 = k % out_c;
        int c2_tmp = h_tmp_tmp*c2;
        int offset = k / out_c;
        int offset_tmp = offset % stride;
        for(j = 0; j < h; ++j){
            int w_tmp = (j + h_tmp)*w;
            int h2 = j*stride + offset_tmp;
            int h2_tmp = h2 + c2_tmp;
            int w_stride = w*stride*h2_tmp;
            for(i = 0; i < w; ++i){
                int in_index  = w_tmp++;
                int w2 = i*stride + offset_tmp;
                int out_index = w2 + w_stride;
                out[in_index] = x[out_index];
            }
        }
    }
}

void forward_reorg_layer(const layer l, network net)
{
    reorg_cpu(net.input, l.w, l.h, l.c, l.stride, l.output);
}

layer make_reorg_layer(int w, int h, int c, int stride)
{
    layer l = {0};
    l.type = REORG;
    l.stride = stride;
    l.h = h;
    l.w = w;
    l.c = c;

    l.out_w = w/stride;
    l.out_h = h/stride;
    l.out_c = c*(stride*stride);

    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = h*w*c;

    fprintf(stderr, "reorg              /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n",  stride, w, h, c, l.out_w, l.out_h, l.out_c);

    int output_size = l.outputs;
    l.output =  calloc(output_size, sizeof(float));

    l.forward = forward_reorg_layer;

    return l;
}
