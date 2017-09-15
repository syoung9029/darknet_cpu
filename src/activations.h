#ifndef ACTIVATION_H
#define ACTIVATION_H

typedef enum{
    LOGISTIC, LINEAR, LEAKY, BLANK_AC
}ACTIVATION;

ACTIVATION get_activation(char *s);
void activate_array(float *x, const int n, const ACTIVATION a);
#endif
