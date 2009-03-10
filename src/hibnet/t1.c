#include "lwneuralnet.h"
#include <stdio.h>

main() {
network_t *net;
int i;

float input[2] = {1000.};
float output[2];
float target[2] = {1000.};

net = net_allocate(3,1,2000,1);
net_use_bias(net,0);

/* set the weights by hand */
for (i=1;i<100;i++) {
 net_set_learning_rate(net,.001);
 net_compute(net,input,output);
 printf("output on iteration %d is %f\n",i,*output);
 net_compute_output_error(net,target);
 net_train(net);
 }
}
