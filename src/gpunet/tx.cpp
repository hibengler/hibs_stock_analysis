#include "lwneuralnet.h"
#include <stdio.h>


#include "network.c"


void net_compute_fast(struct network_ts *net, const float *input, float *output);

float net_compute_output_error_fast(
	 struct network_ts *net, const float *target);

void net_train_fast(struct network_ts *net);

void net_learn_cycle_fast(
	struct network_ts *net, 
	  const float *input, 
	  const float *target,
	  int loops,
	  int iterations,
	  int next_input_offset,
	  int next_output_offset,
	  int accuracy_start,
	  int accuracy_first_output,
	  int accuracy_skip_offset,
	  int no_accuracies
	  );

void net_learn_cycle(
	struct network_ts *net, 
	  const float *input, 
	  const float *target,
	  int loops,
	  int iterations,
	  int next_input_offset,
	  int next_output_offset,
	  int accuracy_start,
	  int accuracy_first_output,
	  int accuracy_skip_offset,
	  int no_accuracies
	  );

void net_run_batch(
        struct network_ts *net,
          const float *input,
          float *output,      
          float *accuracies,
          int iterations,
          int next_input_offset, 
          int next_output_offset,
          int accuracy_start,
          int no_accuracies
          );



main() {
network_t *net,*net2;
int i;

float input[10000];
float output[10000];
float accuracies[10000];
float *target=input+6;

net = net_allocate(3,6,2000,2);
net2 = net_copy(net);
net_use_bias(net,1);
net_use_bias(net2,1);

for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  output[i]=sin(((double)i)*.30);
  }
/* set the weights by hand */
 net_set_learning_rate(net,.001);
 net_set_learning_rate(net2,.001);
 net_learn_cycle_fast(net,input,target,10,2000
                                      ,1,1,
				       1,0,1,1);
 
 net_run_batch(net,output,output+6,accuracies,50,1,1,1,1);

 for (i=0;i<50;i++) {
   printf("%d %3.2f	%3.2f	+-%f\n",i,input[i],output[i],accuracies[i]);
   }

exit(0);

printf("fast is still %f %f %f %f\n",*output,output[1],output[2],output[3]);
net_learn_cycle(net2,input,target,100,20,1,1,2,0,1,2);
net_compute(net2,input,output);
printf("\nregular is %f %f %f %f\n",*output,output[1],output[2],output[3]);
exit(0);
for (i=1;i<2;i++) {
 net_set_learning_rate(net,.0001);
 net_set_learning_rate(net2,.0001);
 // if ((i % 2) ==0) printf("\nrun on iteration %d is %f\n",i,*output);
 net_compute(net,input,output);
 if ((i % 99) ==0) printf("\nrun on iteration2 %d is %f\n",i,*output);
 }
}
