/* 
Input:
	companies.dat
	trained.net
	trained.err

Try out the neural net we have trained on so hard


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
  












int compute_actual(float *actual,float *old_actual,float *normalized) {
int i;
for (i=0;i<number_companies;i++) {
  actual[i] = old_actual[i] * exp(normalized[i]/5.);
  }
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
float *initial;
float *pointer[100000];
float *actual[100000];
float *error;
#define ERROR_COUNT 50
float errors[ERROR_COUNT];
int counter;
roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));

initial =  malloc(sizeof(float)*number_companies);
current = malloc(sizeof(float)*number_companies);
guess = malloc(sizeof(float)*number_companies);
error = malloc(sizeof(float)*number_companies);
fprintf(stderr,"try read...\n");
{
  FILE *fd;
  fd = fopen("trained.err","r");
  fread((void *)error,sizeof(float),number_companies,fd);
  fclose(fd);
  }

counter = 0;
fread((void *)initial,sizeof(float),number_companies,stdin);
actual[0] =  malloc(sizeof(float)*number_companies);
memcpy(actual[0],initial,sizeof(float)*number_companies);
while (fread((void *)current,sizeof(float),number_companies,stdin)) {
  pointer[counter] = current;
  current =  malloc(sizeof(float)*number_companies); 
  counter++;
  actual[counter] =  malloc(sizeof(float)*number_companies);
  compute_actual(actual[counter],actual[counter-1],pointer[counter-1]);
  }
free(current); 

net = net_bload("trained.net");
for (i=counter;i<counter+13;i++) {
  current=pointer[i-1];
  /* The data is 5*log difference */
  net_compute(net,current,guess);
  actual[i+1] =  malloc(sizeof(float)*number_companies);
  pointer[i] = guess;
  
  compute_actual(actual[i+1],actual[i],guess);
  
  //    net_compute_output_error(net,current);
/*  for (k=0;k<number_companies;k++) {
    fprintf(stderr,"%s	%f	%f->%f\n",companies[k],error[k],current[k],guess[k]);
    }
  fprintf(stderr,"\n");
  */
  guess =  malloc(sizeof(float)*number_companies); 
  }



/* OK,  the amazing carmack has predicted the end of the day results. So lets show the winners
*/
{
int from;int to;
from = counter-1;
to = counter;
int best;
float bestscore;
bestscore=-1000.f;
best=-1;

for (i=0;i<number_companies;i++) {
  float score=0.;
  if (error[i] < 0.01) {
    for (j=from+1;j<=to;j++) {
      score += pointer[j][i];

      if (score > bestscore) {
        bestscore = score;
        best = i;
        }
      }
    }
  }
  
fprintf(stderr,"%s %lf percent\n",companies[best],(double)(exp(bestscore/5.0)*100. - 100.));
  for (j=from+1;j<=to;j++) {
    float score = pointer[j][best];
    fprintf(stderr,"%f ",(double)(exp(score/5.0)*100. - 100.));
    }
  fprintf(stderr,"\n");
}

exit(0);
}



