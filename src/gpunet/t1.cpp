#include "lwneuralnet.h"
#include <stdio.h>


#include "network.c"

void net_compute_special_error_fast4(
        struct network_ts *net1,
        struct network_ts *net2,
        struct network_ts *net3,
        struct network_ts *net4,
          const float *inputx,
          const float *targetx,
          int iterations,
          int next_input_offset,
          int next_output_offset,
          int accuracy_start,
          int accuracy_first_output,
          int accuracy_skip_offset,
          int no_accuracies
          );

void net_learn_cycle_fast4(
        struct network_ts *net1,
        struct network_ts *net2,
        struct network_ts *net3,
        struct network_ts *net4,
          const float *inputx,
          const float *targetx,
          int loops,
          int iterations,
          int next_input_offset,
          int next_output_offset,
          int accuracy_start,
          int accuracy_first_output,
          int accuracy_skip_offset,
          int no_accuracies
          );

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
void net_run_batch_fast(
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


															  
void net_run_batch_fast4(
        struct network_ts *net1,
        struct network_ts *net2,
        struct network_ts *net3,
        struct network_ts *net4,
          const float *inputx,
          float *output1x,float *output2x,float *output3x,float *output4x,
          float *accuracies1x,float *accuracies2x,float *accuracies3x,float *accuracies4x,
          int iterations,
          int next_input_offset,
          int next_output_offset,
          int accuracy_start,
          int no_accuracies
          );



void net_compute_fast4(struct network_ts *net1,
  struct network_ts *net2,
  struct network_ts *net3,
  struct network_ts *net4,
  const float *inputx, float *output1,
  float *output2,float *output3,float *output4);


main() {
network_t *net,*net2,*net1,*net3,*net4;
int i;

float input[10000];
float output[10000];
float accuracies[10000];
float output2[10000];
float accuracies2[10000];
float output3[10000];
float accuracies3[10000];
float output4[10000];
float accuracies4[10000];
float *target=input+6;
net = net_allocate(3,6,24,2);
/*net1 = net_copy(net);
net2 = net_copy(net);
net3 = net_copy(net);
net4 = net_copy(net);
*/
net1 = net_allocate(3,6,24,2);
net2 = net_allocate(3,6,24,2);
net3 = net_allocate(3,6,24,2);
net4 = net_allocate(3,6,24,2);

for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  output[i]=sin(((double)i)*.30);
  }
/* set the weights by hand */
 net_set_learning_rate(net,.0001);
 net_set_learning_rate(net1,.0001);
 net_set_learning_rate(net2,.0001);
 net_set_learning_rate(net3,.0001);
 net_set_learning_rate(net4,.0001);
 fprintf(stdout,"learn fast\n");

 net_learn_cycle(net,input,target,1,1
                                      ,1,1,
				       1,0,1,1);
 net_learn_cycle_fast4(net1,net2,net3,net4,input,target,1,12
                                      ,1,1,
				       1,0,1,1);
 net_compute_special_error_fast4(net1,net2,net3,net4,input,target,12
                                       ,1,1,
                                        1,0,1,1);
  fprintf(stdout,"n1 %f n2 %f n3 %f n4 %f\n",net1->global_error,net2->global_error,
             net3->global_error,net4->global_error);					
for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  output[i]=sin(((double)i)*.30);
  }
 fprintf(stdout,"batch 50\n");
 net_run_batch(net2,output,output+6,accuracies,16,1,1,1,1);
 for (i=0;i<16;i++) {
   printf("%d %3.2f	%3.2f	+-%f\n",i,input[i],output[i+6],accuracies[i]);
   }
for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  output[i]=sin(((double)i)*.30);
  accuracies[i]=-1.;
  }
 fprintf(stdout,"batch1 50\n");
 net_run_batch_fast4(net1,net2,net3,net4,output,output+6,output2+6,output3+6,output4+6,
         accuracies,accuracies2,accuracies3,accuracies4,15,1,1,1,1);
 for (i=0;i<16;i++) {
   printf("1 %d %3.2f	%3.2f	+-%f\n",i,input[i],output[i+6],accuracies[i]);
   }
 for (i=0;i<16;i++) {
   printf("2 %d %3.2f	%3.2f	+-%f\n",i,input[i],output2[i+6],accuracies2[i]);
   }
 for (i=0;i<16;i++) {
   printf("3 %d %3.2f	%3.2f	+-%f\n",i,input[i],output3[i+6],accuracies3[i]);
   }
 for (i=0;i<16;i++) {
   printf("4 %d %3.2f	%3.2f	+-%f\n",i,input[i],output4[i+6],accuracies4[i]);
   }
exit(0);
for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  output[i]=sin(((double)i)*.30);
  }
 net_run_batch_fast(net,output,output+6,accuracies,10,1,1,1,1);
 for (i=0;i<16;i++) {
   printf("%d %3.2f	%3.2f	+-%f\n",i,input[i],output[i],accuracies[i]);
   }
   exit(0);
 fprintf(stdout,"fastbatch 50\n");
for (i=0;i<10000;i++) {
  input[i]=sin(((double)i)*.30);
  if (i<6) output[i]=sin(((double)i)*.30);
  else output[i]=-1;
  accuracies[i]=i;
  
  }
 net_run_batch_fast(net,output,output+6,accuracies,10,1,1,1,1);
fprintf(stdout,"hi\n");
 for (i=0;i<16;i++) {
   printf("%d %3.2f	%3.2f	+-%f\n",i,input[i],output[i],accuracies[i]);
   }
 fprintf(stdout,"done\n");

exit(0);  
  for (i=0;i<1;i++) net_compute(net,input+4,output);
  printf("regular is %f\n",*output);
  net_compute_fast(net,input+4,output);
  printf("fast is %f\n",*output);
  net_compute_fast4(net,net,net,net,input+4,output,output+2,output+4,output+6);
  printf("fast4 is %f,%f,%f,%f\n",*output,output[2],output[4],output[6]);
  

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
exit(0); 
}
