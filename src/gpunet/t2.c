#include "lwneuralnet.h"
#include <stdio.h>


#include "network.c"

main() {
network_t *net;
int i;

float input[2] = {100.};
float output[2];
float target[2] = {100.};

net = net_allocate(4,1,2000,2000,1);
net_use_bias(net,1);

/* set the weights by hand */
for (i=1;i<2;i++) {
 net_set_learning_rate(net,.0001);
 net_compute(net,input,output);
 printf("output on iteration %d is %f\n",i,*output);
 net_compute_output_error(net,target);
 net_train(net);
 }

for (i=1;i<1000;i++) {
 net_set_learning_rate(net,.0001);
/* net_compute_fast_noback(net,input,output);*/
 if ((i % 20) ==0) printf("run on iteration %d is %f\n",i,*output);
 net_compute(net,input,output);
 /*if ((i % 10) ==0) printf("run on iteration2 %d is %f\n",i,*output);*/
 }
}
