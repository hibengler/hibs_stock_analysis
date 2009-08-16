/* 
Input:
	companies.dat
	the.net
	sdtandard input - normalized float data
output:
	trained.net
	
Based on ticker_From_archive.c -- but this normalizes the data itno a stream of floats - 
first number - number of tickers
each line - one row of data- floats included



1.6 - changed normalize from log*10 to log*5
1 6.61
2 4.78
3 3.63
4
5 2.91


1.6 - same as 1.1
1 10.63
2 7.67
3 5.96
4 5.55
5 4.62

1.5 0.001
Pass 1 14.24
Pass 2: 9.5
Pass 3: 7.2
Pass 4: 7.1
Pass 5: 5.45


1.4 0.001
Pass 1 - 58.98

1.3 0.001
Pass 1 - 65.97
Pass2 - 51.42


1.3 0.0002
Pass 1 - 63.35
Pass2 - 45


Note - 1.2 and 1.1 were halving the amount of work done in each pass 1.3 does 401 per pass.

Results from 1.2 0.0002
Pass 1 - 23.071415
Pass 2 - 15.02
Pass 3 - 13.09
Pass 4 - 8.87
Pass 5 - 10.08
Pass 6 - 9.46
Pass 7 - 7.64
Pass 8 - 7.91

Results from 1.2 0.001
Pass 1 - 25.53
Pass 2 - 20.50
Pass 3 - 18.56
Pass 4 - 16.6
Pass 5 - 17.47
PAss 6 - 16.18
Pass 7 - 16.216
Pass 8 - 15.19
Pass 9 - 15.47


Results on out test data for Version 1.1 of creator:
At a learning rate of 0.001
Pass 1 - 4.39
Pass 2 - 3.5
Pass 3 - 2.46
Pass 4 - 2.36
Pass 5 - 2.17
Pass 6 - 2.13
Pass 7 - 1.82
Pass 8 - 1.54
Pass 9 - 1.64
Pass 10 - 1.37
Pass 11 - 1.22
Pass 12 - 1.28
Pass 13 - 1.35
Pass 14 - 1.06
Pass 15 - 1.08
Pass 16 - 1.09
Pass 17 - 1.02
Pass 18 - 1.06
Pass 19 - 0.91
Pass 20 - 1.02

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
int counter;
roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));

current = malloc(sizeof(float)*number_companies);
guess = malloc(sizeof(float)*number_companies);
fprintf(stderr,"train read...\n");
counter = 0;
fread((void *)current,sizeof(float),number_companies,stdin); /* read initial values line and ignore it */
while (fread((void *)current,sizeof(float),number_companies,stdin)) {
  pointer[counter] = current;
  current =  malloc(sizeof(float)*number_companies); 
  counter++;
  }
free(current); 

#define PASS1 401  
net = net_bload("the.net");
net_set_learning_rate(net,0.001);
 for (j=0;j<20;j++) {
  for (i=0;i<PASS1;i++) {
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
    if ((j||(i>=ERROR_COUNT))&&(i % 10 == 0)) fprintf(stderr,"pass %d,%d error is %f rand %d\n",j+1,i+1,average,r);
    net_train(net);
    }
  net_bsave("trained.net",net);  
  }


exit(0);
}



