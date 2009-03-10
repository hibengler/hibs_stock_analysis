/* creator
This creates 256 neural nets at random
argv[1]

*/
      
#include "gen.h"
#include "gpunet/lwneuralnet.h"      

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

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





int read_companies(FILE *pf) {
char line[20000];
number_companies=0;

if (! fgets(line,19999,pf)) return(0);
{
  int position;
  char *cp;
  fprintf(stderr,"line is %s\n",line);
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) return(read_companies(pf));
  cp=strtok(line," ");
  while (cp=strtok(NULL," ")) {
    companies[number_companies++] = strdup(cp);
    }
  }
return(1);
}
  



                            
                     

			    
							                            
                            

			    
							
										    
														
																	    






 
  


    


	
#define NETS 256

main(int argc,char *argv[]) {
/* argument 1 - file name for the list of companies
argument2 - file name template for the new files
argument 3 - training data in .flt format (floating point array)

*/
int i,j,k,l;
int flag;
network_t *resultnet[NETS];
int mode;
int size;
float *input;
float *output;
float *accuracy;
int number_spots;
network_t *net;
int number_input;
int number_output;
int number_base;
int max_input;
int total_sets;
int total_iterations;
FILE *roster;

roster=fopen(argv[1],"r");
while (read_companies(roster)) {
  total_sets= (size>>2)/(number_companies*2);
  max_input=1720;
  number_input = number_companies*2*(max_input / (number_companies*2));
  number_output = number_companies*2; /* not doing accuracy yet */

  number_base = 107;/* ups to 1712 which is close to our limit */
  number_spots = number_base;



  fprintf(stderr,"allocated.  number_input %d number_base %d number_output %d\n",number_input,number_base,number_output);
  /* create the neural networks */
  for (i=0;i<NETS;i++) {
    char name[2000];
    net = net_allocate (3,number_input,number_base,number_output);
     net_use_bias(net,0);
    net_set_learning_rate(net,1./number_base);
    sprintf(name,"%s/%s_%3.3d_%3.3d_%3.3d.net",companies[0],argv[2],0,0,i);
    net_bsave(name,net);
    net_free(net);
    }
  
  fprintf(stderr,"saved %s\n",companies[0]);
  for (i=0;i<number_companies;i++) {
    if (companies[i]) {
      free(companies[i]);
      companies[i]=NULL;
      }
    }
  number_companies=0;
  
  } /* for each company set */

}

