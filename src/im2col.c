#include "im2col.h"
/*
void im2col_cpu(float* data_im,
                int channels,  int height,  int width,
                int ksize,  int stride, int pad, float* data_col)
{
    int c,h,w;
    int height_col = (height + 2*pad - ksize) / stride + 1;
    int width_col = (width + 2*pad - ksize) / stride + 1;
    int channels_col = channels * ksize * ksize;
    int ksize_tmp = ksize * ksize;

    unsigned u_height = (unsigned)height;
    unsigned u_width = (unsigned)width;
//#pragma omp parallel for

    for (c = 0; c < channels_col; ++c) {
        int col_index_tmp = c * height_col;
        int h_offset_del = (c / ksize) % ksize - pad;
        int w_offset_del_tmp = c % ksize - pad;
        int data_col_tmp = c / ksize_tmp * height;
        for (h = 0; h < height_col; ++h) {
            int col_index = (col_index_tmp + h)*width_col;
            int data_col_tmp2 = width*(h_offset_del + data_col_tmp);
            int w_offset_del = w_offset_del_tmp;
            unsigned u_im_row = (unsigned) h_offset_del;
            for(w = 0;w< width_col;++w){
                if (u_im_row >= u_height || ((unsigned)w_offset_del) >= u_width){
                    data_col[col_index++]=0;
                }else{
                    data_col[col_index++]=data_im[w_offset_del + data_col_tmp2];
                }
                w_offset_del += stride;
            }
            h_offset_del += stride;
        }
    }
}*/

void im2col_cpu(float* data_im, int channels,
                int height, int width, int size,
                int stride, int pad, float* data_col) {
    int h,w,c;
    int height_col = (height + 2 * pad - size) / stride + 1;
    int width_col = (width + 2 * pad - size) / stride + 1;
    int channels_col = channels * size * size;

    for (c = 0; c < channels_col; ++c) {
        int w_offset = c % size - pad;
        int h_offset = (c / size) % size - pad;
        int c_im = c / size / size;

        int height_col_tmp = c * height_col;
        int height_tmp = c_im * height;
        for (h = 0; h < height_col; ++h) {
            int h_pad = h * stride + h_offset;

            int row_offset = (height_col_tmp + h) * width_col;
            int srow_offset = (height_tmp + h_pad) * width;
            for (w = 0; w < width_col; ++w) {
                int w_pad = w * stride + w_offset;
                if ((((unsigned)h_pad) < ((unsigned)height)) && (((unsigned)w_pad) < ((unsigned)width)))
                    data_col[row_offset + w] = data_im[srow_offset + w_pad];
                else {
                    data_col[row_offset + w] = 0.;
                }
            }
        }
    }
}
