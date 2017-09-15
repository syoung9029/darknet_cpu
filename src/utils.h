#ifndef UTILS_H
#define UTILS_H

#include "stdio.h"
#define TWO_PI 6.2831853071795864769252866

float find_float_arg(int argc, char **argv, char *arg, float def);
int find_int_arg(int argc, char **argv, char *arg, int def);
char *fgetl(FILE *fp);
void strip(char *s);
float rand_normal();
void error(const char *s);
int max_index(float *a, int n);
#endif
