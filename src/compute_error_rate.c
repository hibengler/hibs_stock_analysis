/* 
Input:
	companies.dat
	trained.net
	sdtandard input - normalized float data
output:
	trained.err  - array of floats containing the average error rate for the given data set.
	
Based on ticker_From_archive.c -- but this normalizes the data itno a stream of floats - 
first number - number of tickers
each line - one row of data- floats included



Its pretty good.
But it did bad on predicting the next day.  It was overtrained.


Version 1.2 is much bigger - 5 times the numbe rof hiddne nodes.  The 5 hidden variables
are dedicated for each output variable.

*/
      
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>


#include "lwneuralnet.h"      
#include "gen.h"
#include "hash.h"	    
#include "ai_constant.h"
            
      struct company { /* shorter company for this purpose */
         double *point;  
         double *volume;  
         char  symbol[12];
         double *seconds;  
         int number_points;
	 int start;
         double *r;      
	 int number_timepoints;
	 double *timepoint;
         double t;
	 };
         

char *companies[10000];
int number_companies = 0;

/* read the list of companies */
int read_companies(FILE *pf) {
char line[20000];

if (! fgets(line,19999,pf)) return(0);
{
  int position;
  char *cp;
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) return(read_companies(pf));
  companies[number_companies++] = strdup(line);
  }
return(1);
}
  



















main(int argc,char *argv[]) {
int i,j,k,l;
int flag;
network_t *net;
       int number_full_companies;
struct company *full_companies;        
FILE *roster;
float *current;
float *previous;
float *guess;
float *pointer[100000];
#define ERROR_COUNT 50
float errors[ERROR_COUNT];
float error[100000];
int counter;
roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));

current = malloc(sizeof(float)*number_companies);
guess = malloc(sizeof(float)*number_companies);
fprintf(stderr,"compute_error_rate read...\n");
counter = 0;
fread((void *)current,sizeof(float),number_companies,stdin); /* read initial values line and ignore it */
while (fread((void *)current,sizeof(float),number_companies,stdin)) {
  pointer[counter] = current;
  current =  malloc(sizeof(float)*number_companies); 
  counter++;
  }
free(current); 

for (i=0;i<number_companies;i++) {
  error[i]=0.;
  }   

net = net_bload("trained.net");
#define ITERATIONS 300
  for (i=0;i<ITERATIONS;i++) {
    float average;
    int r= (rand() % (counter-1) );
    previous=pointer[r];
    current=pointer[r+1];
    /* The data is 10*log difference */
    net_compute(net,previous,guess);
    net_compute_output_error(net,current);
    errors[i%ERROR_COUNT] = net_get_output_error(net);
    average = 0;
    for (k=0;k<ERROR_COUNT;k++) average += errors[k];
    average = average / ERROR_COUNT;
    if ((i>=ERROR_COUNT)&&(i % 10 == 0)) fprintf(stderr,"pass %d error is %f\n",i+1,average);
    
    for (j=0;j<net->output_layer->no_of_neurons;j++) {
      error[j] += net->output_layer->neuron[j].error;
      }
    
    
    }

  for (i=0;i<number_companies;i++) {
    error[i]= error[i] / ITERATIONS;
    }   

  {
  FILE *err;
  err=fopen("trained.err","w");
  fwrite((void *)error,sizeof(float),number_companies,err); /* read initial values line and ignore it */  
  fclose(err);
  }
exit(0);
}



