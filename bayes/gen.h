#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>

double str_to_date(char *);
int greatest(int a,int b);
int leastest(int a,int b);
double absd(double);
float absf(float);

#ifdef __APPLE__

size_t getline(char **lineptr, size_t *n, FILE *stream);
#endif
