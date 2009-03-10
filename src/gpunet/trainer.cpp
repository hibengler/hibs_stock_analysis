/* trainer
This tries an NNL on the stocks and tries to predict what will come next.
*/
      
/*include "gen.h"*/
#include "lwneuralnet.h"      
#include "network.c"      

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

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
          int epochs,
          int iterations,
          int next_input_offset,
          int next_output_offset,
          int accuracy_start,
          int accuracy_first_output,
          int accuracy_skip_offset,
          int no_accuracies
          );


char *companies[10000];
int number_companies;





int read_companies(char *file) {
FILE *pf;
char line[20000];
number_companies=0;

pf = fopen(file,"r");
if (!pf) {
  fprintf(stderr,"error cant find file %s\n",file);
  exit(-1);
  }
while (fgets(line,19999,pf) != NULL) {
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) continue;
  companies[number_companies++] = strdup(line);
  }
return(0);
}
  



                            
                     

			    
							                            
                            
int grow_nnl(network_t **resultnet)
{
network_t *oldnet;
//network_t *newnet;
oldnet = *resultnet;

//newnet = net_spawn(oldnet,0.25);
//net_jolt(newnet,.1,.1);
/*net_add_neurons(newnet,1,-1,100);
*/
//net_free(oldnet);

//*resultnet = newnet;
return(0);
}
                            
                            

			    
							
										    
														
																	    






 
  


    


	
#define NETS 256

int main(int argc,char *argv[]) {
/* argument 1 - file name for the list of companies
argument2 - file name template for the new files
argument 3 - training data in .flt format (floating point array)
argument 4 - universe number -- always 0 for now
argument 5 - generation number - 0,1,2,3,4,5.
Generation 0 is the first 256 records created by the epoch program.
all other generations startoff with 128 culled records -- and we 
make children from the parents.
*/
int i,j,k;
network_t *resultnet[NETS];
int size;
float *input;
int number_spots;
int number_input;
int number_output;
int number_base;
int max_input;
int total_sets;
int total_iterations;
int universe;
int read_in_count;
{
struct stat s;
stat(argv[3],&s);
size = s.st_size;
fprintf(stderr,"size for %sis %d\n",argv[3],size);
}


read_companies(argv[1]);

total_sets= (size>>2)/(number_companies*2);



max_input=1720;
number_input = number_companies*2*(max_input / (number_companies*2));
number_output = number_companies*2; /* not doing accuracy yet */

number_base = 107;/* ups to 1712 which is close to our limit */
number_spots = number_base;



fprintf(stderr,"allocated %s.  number_input %d number_base %d number_output %d\n",argv[3],number_input,number_base,number_output);
/* read the neural netowrks */
universe = atoi(argv[4]);
j = atoi(argv[5]);
if (j==0) {
  read_in_count=NETS;
  }
else {
  read_in_count=NETS/2;
  }
fprintf(stderr,"loading %d\n",j);
for (i=0;i<read_in_count;i++) {
    char name[2000];
    int k;
    sprintf(name,"%s_%3.3d_%3.3d_%3.3d.net",argv[2],universe,j,i);
    resultnet[i] = net_bload(name);
    for (k=0;k<resultnet[i]->no_of_layers;k++) {
      resultnet[i]->layer[k].activation=NET_ACT_TANH;
      }
    }


total_iterations=total_sets - (number_input/(number_companies*2)) - 1;

fprintf(stderr,"read... %d bytes %d floats which is %d sets and %d runs\n",size,size>>2,total_sets,total_iterations);

input = (float *)malloc(sizeof(float) * (size>>2));
{FILE *in;
in=fopen(argv[3],"rb");
fread(input,sizeof(float),(size>>2),in);
fclose(in);
}
fprintf(stderr,"done reading...\n");

if(j) {
  fprintf(stderr,"done loading.  Now spawn kids for the next generation %d\n",j);
  for (i=NETS/2;i<NETS;i++) {
    resultnet[i]=net_copy(resultnet[i-(NETS/2)]);
    net_jolt(resultnet[i],0.04,0.001);
    net_jolt(resultnet[i-(NETS/2)],0.04,0.001);
    }
  }
  
fprintf(stderr,"training...");

  /* Ok we have the input -- lets train them - 4 at a time */
  for (i=0;i<NETS;i+=4) {

    net_learn_cycle_fast4(
      resultnet[i],resultnet[i+1],resultnet[i+2],resultnet[i+3],
  input,
  input+ (number_companies*2),
  3,
  total_iterations,
  2,
  2,
  number_companies*2,
  0,
  2,
  0
  );
  fprintf(stderr,"z_");
    net_compute_special_error_fast4(
      resultnet[i],resultnet[i+1],resultnet[i+2],resultnet[i+3],
  input,
  input+ (number_companies*2),
  total_iterations,
  2,
  2,
  0,
  2,
  number_companies
  );
    fprintf(stderr,"%3.3d_%3.3d_%3.3d %f,%f,%f,%f\n",0,j,i,resultnet[i]->global_error*1024.,
               resultnet[i+1]->global_error*1024.,
               resultnet[i+2]->global_error*1024.,
               resultnet[i+3]->global_error*1024.);
    /* someday - we will figure out a better metric than global_error - like best predictability over all
     so that a predictor of one of the group of stocks is better than its brethren */

    }


  fprintf(stderr,"generation %d done training. Now kill the weak\n",j);
  /* ineffecient cull but with NETS nodes it is ok */
  for (i=NETS-1;i>=NETS/2;i--) {
    int max;
    float maxerror;
    max= -1;
    maxerror= -1000000.f;
    for (k=0;k<=i;k++) {
      if (resultnet[k]->global_error > maxerror) {
        maxerror=resultnet[k]->global_error;
        max=k;
        }
      }
    if (max ==-1) {
      fprintf(stderr,"shoot - the global errors are goofed up!\n");
      exit(-1);
      }
    net_free(resultnet[max]);
    resultnet[max]=resultnet[i];
    resultnet[i]=NULL;
    }



  

  fprintf(stderr,"saving %d\n",j);
  for (i=0;i<NETS/2;i++) {
    char name[2000];
    sprintf(name,"%s_%3.3d_%3.3d_%3.3d.net",argv[2],0,j+1,i);
    net_bsave(name,resultnet[i]);
    }
  

  


}

