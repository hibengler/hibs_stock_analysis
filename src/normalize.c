/* 
Based on ticker_From_archive.c -- but this normalizes the data itno a stream of floats - 
first number - number of tickers
each line - one row of data- floats included

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
float *delta;

roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));

current = malloc(sizeof(float)*number_companies);
previous = malloc(sizeof(float)*number_companies);
delta = malloc(sizeof(float)*number_companies);
fprintf(stderr,"normalize read...\n");
fread((void *)previous,sizeof(float),number_companies,stdin);
/* print the starting values */
fwrite((void *)previous,sizeof(float),number_companies,stdout);
while (fread((void *)current,sizeof(float),number_companies,stdin)) {
  for (i=0;i<number_companies;i++) {
    if ((current[i]==0.)||(previous[i]==0.)) delta[i]=0.;
    else {
      delta[i] = 5. *(logf(current[i]) - logf(previous[i])); 
      if (delta[i] >1.) delta[i] = 1.;
      if (delta[i] <-1.) delta[i] = -1.;
      if (isnan(delta[i])) {
        delta[i] = 0.;
        fprintf(stderr,"\nNAN %f - %f  log(%f) - log(%f)\n", logf(current[i]), logf(previous[i]),current[i],previous[i]);
        }
      }
    }
  fwrite((void *)delta,sizeof(float),number_companies,stdout);
  }



exit(0);
}



