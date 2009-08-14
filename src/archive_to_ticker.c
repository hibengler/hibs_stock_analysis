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
  


struct dataline 
{
char name[100];
char symbol[12];
char quotedate[30];
double dateInSeconds;
double currentPrice;
double changeValue;
double changePercent;
double previousClose;
double open;
double volume;
double dayMinPrice;
double dayMaxPrice;
double priceToEarnings;
double marketCap;
double averageVolume;
double yearMinPrice;
double yearMaxPrice;
double dividendValue;
double earningsPerShare;
double dividendYield;
double currentPriceRank;
double changeValueRank;
double changePercentRank;
double volumeRank;
double priceToEarningsRank;
double marketCapRank;
double averageVolumeRank;
double yearMinPriceRank;
double yearMaxPriceRank;
double dividendValueRank;
double earningsPerShareRank;
double dividendYieldRank;
double currentPriceValue;
double changeValueValue;
double changePercentValue;
double volumeValue;
double priceToEarningsValue;
double marketCapValue;
double averageVolumeValue;
double yearMinPriceValue;
double yearMaxPriceValue;
double dividendValueValue;
double earningsPerShareValue;
double dividendYieldValue;
};


       
	      
                  
	   




int process_data_limit(char *filename,struct company **pcompanies, int *pnumber_companies) 

{
struct company *companies; /* stores the companies -- before we assign it to *pcompanies */
int number_companies; /* holds the number of companies - before we assign it to *pnumber_Companies */
int i;
FILE *pf;
char line[20000];
size_t n;
struct dataline dl;
char last_symbol[12];

hash_init();
/* set the environment to new york time */
putenv("TZ=EST5EDT");
last_symbol[0]='\0';

number_companies = 4000; /* guess */
companies = (struct company *)malloc(sizeof(struct company)*number_companies);

/* open the nnl_test.txt  to get the list of companies that we will look at */
pf = fopen(filename,"r");
if (!pf) exit(-1);
number_companies = 0;
while (fgets(line,19999,pf) != NULL) {
  int position;
  struct company *c;
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) continue;
  if(line[strlen(line)-1] <= ' ') line[strlen(line)-1]='\0';
  if(line[strlen(line)-1] <= ' ') line[strlen(line)-1]='\0';
  
  if ((position=hash_find(line))==-1) { /* if we cant find the ticker */
      hash_set(line,number_companies);
      position=number_companies;
      c = companies + position;
      strcpy(c->symbol,line);
      if (last_symbol[0] == '\0') {
        strcpy(last_symbol,line);
	}	
      c->number_points = 4096; /* guess */
      c->start=0;
      c->point = (double *)malloc(sizeof(double) * c->number_points);
      c->volume = (double *)malloc(sizeof(double) * c->number_points);
      c->seconds = (double *)malloc(sizeof(double) * c->number_points);
	c->number_points = 0;
	/* assume that the points come in order. :? */
      c->r = NULL;
      c->t = 0;
      c->timepoint = NULL;
      number_companies++;
    } /* if we are not duplicated*/
  }
fclose(pf);
  

/*pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
     ,"r");*/
pf = stdin;
if (!pf) return(-1);
i=0;
n=0;
while (fgets(line,19999,pf) != NULL) {
  if (strncmp(line,"name,symbol",11) != 0) { /* if we are not a header line */
    int position;
    struct company *c;
    char *x;
    /* convert the line to the big structure of data */    
    if (x=strtok(line,",")) strcpy(dl.name,x); else continue;
    if (x=strtok(NULL,",")) strcpy(dl.symbol,x); else continue;
    if (x=strtok(NULL,",")) strcpy(dl.quotedate,x); else continue;
    if (x=strtok(NULL,",")) dl.currentPrice = atof(x); else continue;
    dl.dateInSeconds = str_to_date(dl.quotedate); 
    if (x=strtok(NULL,",")) dl.changeValue = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.changePercent = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.previousClose = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.open = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.volume = atof(x); else continue;

/*    fprintf(stderr,"\\%s,%s,%s,%s	%f %lf\n",
    dl.name,dl.symbol,dl.quotedate,x,dl.currentPrice,dl.dateInSeconds);
  */   
        
    /* ok now lets convert this to a point */
    if ((position=hash_find(dl.symbol))==-1) {
      /* if we cant find the ticker */
      continue; /* then ignore it */
      }
    else {
      c = companies + position;
      }

    c->point[(c->number_points+c->start)&4095] = dl.currentPrice;
    c->seconds[(c->number_points+c->start)&4095] = dl.dateInSeconds;
    c->volume[(c->number_points+c->start)&4095] = dl.volume;
    if (c->number_points<4096) c->number_points++;
    else c->start = (c->start+1)&4095;
    
    
    if (strcmp(dl.symbol,last_symbol)==0) { /* if it is time to do processing */
        send_run(dl.dateInSeconds,number_companies,companies);
	}
	
    
    } /* if we are a point line and not a title line */
  
  n=0;
  } /* while reading lines */


*pcompanies = companies;
*pnumber_companies = number_companies;
}

FILE *outfile;



int send_run1(char *action,double date,int offset,int number_companies,struct company *companies) {
int i;
double real_date;
real_date=date;
for (i=0;i<number_companies;i++) {
  struct company *c;
  int j;
  c=companies+i;
  j = position_at_time(c,date);
  if (j==-1) return(-1); /* error if cant find time */
  if (offset &&(i==0)) { /* figure out the REAL date */
    j = j + offset;
    j = j & 4095;
    real_date=c->seconds[j];
    }
  }



for (i=0;i<number_companies;i++) {
  struct company *c;
  int pos;
  int j;
  c=companies+i;
  pos = position_at_time(c,date);
  {
    double point;
    double volume;
    float data[2];
    j=0;
    point = c->point[(run_offsets[j]+pos+offset)&4095]/1024.; /* 0 and 1024 max normalized */
    volume = log(c->volume[(run_offsets[j]+pos+offset)&4095]+1.0)/64.; /* normalize this between 0 and 1 as well */
    data[0]=point;
    data[1]=volume;
    fwrite((void *)data,sizeof(float),2,outfile);
    }
 
  }
return(0);
}



int send_run(double date,int number_companies,struct company *companies) {
int i;
i = send_run1("run",date,0,number_companies,companies);
}





double price_at_time(struct company *x,
   double time)
{
/* similart to compute logpoint,  except the intervals are bihourly 
and linear */
int i;
double ioff;
double *pt;
double *seconds;
int k1,k2;

pt = x->point; /* could be logpoint as well */
seconds = x->seconds;
  
k1=(x->start+x->number_points-1)&4095; /* k1 and k2 points are compared to determine a linear point in between them */
k2=(x->start+x->number_points-1)&4095;    
  
  /* k1 is the index of the prior k.  K2 is the index of the next K.  
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */
	
    
while ((k2!= x->start)&&(seconds[(k2-1)&4095] > time)) k2=(k2-1)&4095;
    
    
if (k2==x->start) { /* we are far back */
  k1 = k2;
  if (seconds[k2] > time) {
    return(-1.);
    }
  }
else { /* we are not all the way back */
  if (seconds[k2] <= time) {
    k1 = k2;
    }
  else 
    k1 = (k2 - 1) & 4095;
  }
      
if (seconds[k1]==seconds[k2]) /* dont divide by zero,  just pick one -- usually ka==k2 as well */
  {
  return(pt[k1]);    
  }
else {
  if  ((time -seconds[k1])> -1. && (time-seconds[k1] < 1.)) {
    return(pt[k1]);
    }
  else 
    return(  pt[k1]  + (pt[k2]-pt[k1]) *
            (time-seconds[k1]) /(seconds[k2]-seconds[k1]));
  }
}
                     

		     
				     
						     
int position_at_time(struct company *x,
   double time)
{
/* similart to compute logpoint,  except the intervals are bihourly 
and linear */
int i;
double ioff;
double *pt;
double *seconds;
int k1,k2;

pt = x->point; /* could be logpoint as well */
seconds = x->seconds;

k1=(x->start+x->number_points-1)&4095; /* k1 and k2 points are compared to determine a linear point in between them */
k2=(x->start+x->number_points-1)&4095;    
  
  /* k1 is the index of the prior k.  K2 is the index of the next K.  
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */
	
while (( ((k2+1) & 4095)!= x->start)&&(seconds[k2] > time)) k2=(k2-1)&4095;
    
if (((k2+1)&4095)==x->start) return(-1);    
return(k2);
}
                     
		     
		     






double derive_current_time_in_seconds(struct company *companies, 
int number_companies) 
{
int i;
double current_time_seconds;
current_time_seconds= 0.;
for (i=0;i<number_companies;i++)
  {                         
  double currenttime;     
  struct company *x;
  x = companies+i;
  if (x->number_points) {
  	currenttime = x->seconds[(x->start+x->number_points-1)&4095];
  	if (current_time_seconds < currenttime) 
	  current_time_seconds =currenttime;
  	}
  }  
return(current_time_seconds);
}





				      
								      		      	      	      
       
       


 
 
 


















main(int argc,char *argv[]) {
int i,j,k,l;
int flag;
network_t *net;
       int number_companies;
struct company *companies;        
FILE *roster;

roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));


outfile=fopen(argv[2],"wb");
fprintf(stderr,"read...\n");
process_data_limit(argv[1],&companies,&number_companies);
fprintf(stderr,"done reading...\n");
fclose(outfile);


exit(0);
}



