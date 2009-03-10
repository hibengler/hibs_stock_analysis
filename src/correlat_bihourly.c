/* correlat_bihourly
This computes bihourly correlation between all bihourly companies (3000 or so) 
from a logarighmic point of view for a given date range.  
It outputs facts on all correlations bigger 
than a given time period.

The correlation is over the bihourly average during the entire duration

correlat_bihourly datefrom dateto company1 company2 amount

It prints out only interesting correlations of 95% or more.
It also (might) filter out correlation groups where the groups are 10 or higher
because then instead of being a correlation,  it is a factor 
*/
      
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

      
	    
struct correlation {
int c1;
int c2;
int number_correlations;
float *offsets;
float *correlations;
};

            
      struct company {
         float *point;  
         char  symbol[12];
         double *seconds;  
         int number_points;
         float *logpoint;
         float *timelogpoint;
         int number_log_points;    
         float *timelogpoint_over_average;
         float resolution_seconds;         
         float average;
         float standard_deviation;
         float variance;
         float *r;      
	 int number_timepoints;
	 float *timepoint;
         float t;
         };
         
       int number_companies;
       struct company *companies;        
                  
	   struct company average;

	   double current_time_seconds;
	   
#define NUMBER_LOG_POINTS 3000
#define LOG_MIN_RESOLUTION 600.
#define LOG_MAX_RESOLUTION     630720000.

/* one minute to ten years */
#define LOG_XFACTOR 2700.
/* the xfactor is 40000 for 60 second resolution */
/* the xfactor is 2700 for 60 second resolution */
/* this max factor gives us a final data point at 678018350 - 21 years back
with a final resolution of about 23 days. :) 
And that is close to LOG_MAX_RESOLUTION
It was picked out of a hat because i don't know how to curve fit lograthmicially with the summations correctly.
*/

int number_of_log_points;                   
double secondsforlogpoint[NUMBER_LOG_POINTS];
                            
        


			 
float general_correlation(float *x,float *y,int nx,int ny);
						  
									                            
                         

  
                         

struct dataline 
{
char name[100];
char symbol[12];
char quotedate[30];
double dateInSeconds;
float currentPrice;
float changeValue;
float changePercent;
float previousClose;
float open;
float volume;
float dayMinPrice;
float dayMaxPrice;
float priceToEarnings;
float marketCap;
float averageVolume;
float yearMinPrice;
float yearMaxPrice;
float dividendValue;
float earningsPerShare;
float dividendYield;
float currentPriceRank;
float changeValueRank;
float changePercentRank;
float volumeRank;
float priceToEarningsRank;
float marketCapRank;
float averageVolumeRank;
float yearMinPriceRank;
float yearMaxPriceRank;
float dividendValueRank;
float earningsPerShareRank;
float dividendYieldRank;
float currentPriceValue;
float changeValueValue;
float changePercentValue;
float volumeValue;
float priceToEarningsValue;
float marketCapValue;
float averageVolumeValue;
float yearMinPriceValue;
float yearMaxPriceValue;
float dividendValueValue;
float earningsPerShareValue;
float dividendYieldValue;
};






int read_data(struct company **pcompanies, int *pnumber_companies) 

{
struct company *companies; /* stores the companies -- before we assign it to *pcompanies */
int number_companies; /* holds the number of companies - before we assign it to *pnumber_Companies */
int i;
FILE *pf;
char *line;
size_t n;
struct dataline dl;

hash_init();
/* set the environment to new york time */
putenv("TZ=EST5EDT");
pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
     ,"r"); 
/*pf = fopen("simple.csv","r");*/
if (!pf) return(-1);
number_companies = 4000; /* guess */
companies = (struct company *)malloc(sizeof(struct company)*number_companies);
i=0;
number_companies = 0;
line = NULL;
n=0;
while (getline(&line,&n,pf) != -1) {

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

/*    fprintf(stderr,"\\%s,%s,%s,%s	%f %lf\n",
    dl.name,dl.symbol,dl.quotedate,x,dl.currentPrice,dl.dateInSeconds);
  */   
        
    /* ok now lets convert this to a point */
      
    if ((position=hash_find(dl.symbol))==-1) { /* if we cant find the ticker */
      
      hash_set(dl.symbol,number_companies);
      position=number_companies;
	
      c = companies + position;
      strcpy(c->symbol,dl.symbol);
	
      c->number_points = 5000; /* guess */
      c->point = (float *)malloc(sizeof(float) * c->number_points);
      c->seconds = (double *)malloc(sizeof(double) * c->number_points);
	c->number_points = 0;
	/* assume that the points come in order. :? */
	c->logpoint = NULL;
	c->timelogpoint = NULL;
	c->number_log_points = 0;
	
	c->timelogpoint_over_average = 0;
  	c->resolution_seconds = 1800.;
  	c->average = 0.;
  	c->standard_deviation = 0.;
  	c->variance = 0.;
      c->r = NULL;
      c->t = 0;
      c->timepoint = NULL;
	
      number_companies++;
	
      } /* new ticker found */
    else {

      c = companies + position;
      }

    c->point[c->number_points] = dl.currentPrice;
    c->seconds[c->number_points] = dl.dateInSeconds;
    c->number_points++;
    
    } /* if we are a point line and not a title line */
  
  if (line) free(line);
  line=NULL;
  n=0;
  } /* while reading lines */

if (line) free(line);

*pcompanies = companies;
*pnumber_companies = number_companies;
}
                            
                            
                            
                            
                            
                            
                            
int compute_logpoint(struct company *x, int number_companies,
   double current_time_in_seconds) 
{
int i;
float ioff;


number_of_log_points = NUMBER_LOG_POINTS;
/* first compute the seconds offset.  We will subtract this from the latest time possible */

/* adding is messy,  but it gets the results I want.  I need to study curve fitting or 
something -- or series.
Anyways -- instead of doing the full power application -
we will do it to LOG_XFACTOR
*/

secondsforlogpoint[0] =  0;

fprintf(stderr,"   offsetinseconds...%d\n",number_of_log_points);


for (i=1;i<number_of_log_points;i++)
  {
  /* adding the previous gives us the right aspread,  but the XFACTOR is picked out of a hat */
  secondsforlogpoint[i] = secondsforlogpoint[i-1] + LOG_MIN_RESOLUTION * 
   expf(   ((float)(i-1)) *
          logf(LOG_XFACTOR) /
          (float)(number_of_log_points-1));
  }



fprintf(stderr,"   find data points...\n");
/* find data points logarithmized */
for (i=0;i<number_companies;i++) {
  struct company *x;
  int j;
  float minpoint;
  
  x = companies+i;
  
  x->number_log_points = 0;
  
  x->logpoint = (float *)malloc(sizeof(float) * x->number_points);
    
  minpoint = x->point[0];
  for (j=1;j<x->number_points;j++) {
    if (x->point[j] < minpoint) minpoint = x->point[j]-1.;
    }                                                     

  if (minpoint<1.) {
  	for (j=0;j<x->number_points;j++) {
  	  x->logpoint[j] = logf(x->point[j]+minpoint+1.);
  	  }
  	} /* if we need to fudge the logpoints */
  else {
    for (j=0;j<x->number_points;j++) {
      x->logpoint[j] = logf(x->point[j]);
      } /* for each point */
    } /* if out minpoint is not out of the ordinary */
  } /* for each company */




fprintf(stderr,"   find adjusted log point...\n");
/* ok.  now find the time adjusted log of the points 
  this could be of points or logpoints -- right now,  I will do points so that we can see easily */
for (i=0;i<number_companies;i++) {
  struct company *x;
  int j;int k1,k2;
  float *pt;
  double *seconds;

  
  /* setup */
  x = companies+i;
  pt = x->point; /* could be logpoint as well */
  seconds = x->seconds;
  x->timelogpoint = (float *)malloc(sizeof(float) * NUMBER_LOG_POINTS);
  k1=x->number_points-1; /* k1 and k2 points are compared to determine a linear point in between them */
  k2=x->number_points-1;    
  number_of_log_points = 0; /* goes up as we have data */
  
  for (j=0;j<NUMBER_LOG_POINTS;j++) {
    double index = current_time_seconds - secondsforlogpoint[j]; 
     /* k1 is the index of the prior k.  K2 is the index of the next K.  
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */
	
    
    while ((k2>0)&&(seconds[k2-1] > index)) k2--;
    
    
    if (k2==0) { /* we are far back */
      k1 = k2;
      if (seconds[k2] > index) {
        break;
        }
      }
    else { /* we are not all the way back */
      if (seconds[k2] <= index) {
        k1 = k2;
        }
      else 
        k1 = k2 - 1;
      }
      
    if (seconds[k1]==seconds[k2]) /* dont divide by zero,  just pick one -- usually ka==k2 as well */
      {
      x->timelogpoint[j] = pt[k1];    
      }
    else {
      x->timelogpoint[j] =  pt[k1]  + (pt[k2]-pt[k1]) *
            (index-seconds[k1]) /(seconds[k2]-seconds[k1]);

    /* point 1 + delta point / delta seconds * number of seconds beyond point 1 */
      }     
    /* set the number of log points up -- we have a record! */
    number_of_log_points = j+1;
    } /* for each point */
  x->number_log_points = number_of_log_points;
  } /* for each company */
       
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
  	currenttime = x->seconds[x->number_points-1];
  	if (current_time_seconds < currenttime) 
	  current_time_seconds =currenttime;
  	}
  }  
return(current_time_seconds);
}





				      
								      		      	      	      
int compute_timepoint(struct company *x, int number_companies,
   double current_time_in_seconds)
{
/* similart to compute logpoint,  except the intervals are bihourly 
and linear */
int i;
float ioff;



fprintf(stderr,"   find data points...\n");

  

for (i=0;i<number_companies;i++) {
  struct company *x;
  int j;int k1,k2;
  float *pt;
  double *seconds;

  
  /* setup */
  x = companies+i;
  pt = x->point; /* could be logpoint as well */
  seconds = x->seconds;
  
  x->number_timepoints = 1+ (int)((current_time_in_seconds - x->seconds[0])
                              / 1800.); /* hack.  1800 should be minimum resolution in seconds */
  x->timepoint = (float *)malloc(sizeof(float) * x->number_timepoints );
  k1=x->number_points-1; /* k1 and k2 points are compared to determine a linear point in between them */
  k2=x->number_points-1;    
  
  for (j=0;j<x->number_timepoints;j++) {
    double index = current_time_seconds - 1800. * j; 
     /* k1 is the index of the prior k.  K2 is the index of the next K.  
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */
	
    
    while ((k2>0)&&(seconds[k2-1] > index)) k2--;
    
    
    if (k2==0) { /* we are far back */
      k1 = k2;
      if (seconds[k2] > index) {
        break;
        }
      }
    else { /* we are not all the way back */
      if (seconds[k2] <= index) {
        k1 = k2;
        }
      else 
        k1 = k2 - 1;
      }
      
    if (seconds[k1]==seconds[k2]) /* dont divide by zero,  just pick one -- usually ka==k2 as well */
      {
      x->timepoint[j] = pt[k1];    
      }
    else {
      if  ((index -seconds[k1])> -1. && (index-seconds[k1] < 1.)) {
        x->timepoint[j] = pt[k1];
	}
      else 
        x->timepoint[j] =  pt[k1]  + (pt[k2]-pt[k1]) *
            (index-seconds[k1]) /(seconds[k2]-seconds[k1]);
      }
    /* point 1 + delta point / delta seconds * number of seconds beyond point 1 */

    } /* for each point */
  } /* for each company */
       
}
                     
       
       

		            
       
       
       
int compute_average_company(struct company *companies, int number_companies, struct company *a) 
{       
int i,j,k;

 strcpy(a->symbol,"average");
 a->point = (float *)malloc(sizeof(float)*NUMBER_LOG_POINTS); 
     /* HACK -- this actually has the number of samples read for the average */
 a->number_points = NUMBER_LOG_POINTS;
 a->logpoint = NULL;
 a->timelogpoint = (float *)malloc(sizeof(float)*NUMBER_LOG_POINTS);
 a->number_log_points = 0;
 a->timelogpoint_over_average = NULL;     
 
 a->average = 0.;
 a->standard_deviation = 0.;
 a->variance = 0.;
 a->r = NULL;
a->t = 0;                  
  
/* find the resolution in seconds as the greatest of the whole model */      
a->resolution_seconds = 0.;
for (i=0;i<number_companies;i++) {
  struct company *x;
  x = companies+i;
  if (a->resolution_seconds < x->resolution_seconds) 
      a->resolution_seconds = x->resolution_seconds;
  }
  
  

for (i=0;i<NUMBER_LOG_POINTS;i++) {
  int number_points;
  float sum;
  number_points = 0;
  
  sum = 0.;
  for (j=0;j<number_companies;j++) {
    struct company *x;
    x = companies+j;
    if (x->number_log_points > i) {
      number_points ++;
      sum = sum + x->timelogpoint[i];
      }
    }
  a->point[i] = number_points; /* hack */ 
  if (number_points)  {
    a->timelogpoint[i] = sum / ((float)(number_points));
    a->number_log_points = i+1;
    }
  else {
    a->timelogpoint[i] = 0.;
    }
  

  }    /* for each log point */                 
}         
             
 
 
 
 
 
int compute_over_average(struct company *x,struct company *a) {
int i;                               
x->timelogpoint_over_average = (float *)malloc(sizeof(float) * x->number_log_points);

for (i=0;i<x->number_log_points;i++) {                        
  if (a->timelogpoint[i] != 0.) {
    x->timelogpoint_over_average[i] = x->timelogpoint[i]/* / a->timelogpoint[i]*/;
    }
  else
    x->timelogpoint_over_average[i] = -1; /* hack */
  x->timelogpoint_over_average[i] = x->timelogpoint[i];
  }
}




int compute_simple_state (struct company *x) {
int i;                               
float average;
float variance;
/* average */
for (i=0;i<x->number_log_points;i++) {                        
  average += x->timelogpoint_over_average[i];
  }
average = average / ((float)x->number_log_points);
x->average = average;

variance = 0.;
for (i=0;i<x->number_log_points;i++) {                        
  variance += (x->timelogpoint_over_average[i]-average) *  
              (x->timelogpoint_over_average[i]-average);
  }
variance = variance / ((float)(x->number_log_points-1));
x->variance = variance;

x->standard_deviation = sqrt(variance);
}

 
 
 
int dump_company(struct company *x) {
int i,j,k;
fprintf(stderr,"Company %s\n",x->symbol);
fprintf(stderr,"Original points (%d): ",x->number_points);
for (i=0;i<x->number_points;i++) {
  fprintf(stderr,"%f	",x->point[i]);
  }
fprintf(stderr,"\n");

if (x->logpoint) {
  fprintf(stderr,"log points (%d): ",x->number_points);
  for (i=0;i<x->number_points;i++) {
    fprintf(stderr,"%f	",x->logpoint[i]);
    }
  }
fprintf(stderr,"\n");

if (x->timelogpoint) {
  fprintf(stderr,"time log points (%d):",x->number_log_points);
  for (i=0;i<x->number_log_points;i++) {
    fprintf(stderr,"%f	",x->timelogpoint[i]);
    }
  }  
fprintf(stderr,"\n");

if (x->timelogpoint_over_average) {
  fprintf(stderr,"time log points over average (%d):",x->number_log_points);
  for (i=0;i<x->number_log_points;i++) {
    fprintf(stderr,"%f	",x->timelogpoint_over_average[i]);
    }
  }  

fprintf(stderr,"\nAverage %f standard deviation %f variance %f\n",
x->average,x->standard_deviation,   x->variance);
}




/* offsets do every half hour for all days */






float general_average(float *x,int n) {
float average;
float weight;
int i;
average = 0.;
weight = 0.;
for (i=0;i<n;i++) {
  average += x[i];
  }
weight = (float) n;
if (weight != 0.) {
  average = average / weight;
  }
else
  average = 0.;
return average;  
}



float  general_stddev(float *x,int n,float average) 
{
float standard_deviation;
int i;
float variance = 0.;
for (i=0;i<n;i++) {                        
  variance += (x[i]-average) *  
              (x[i]-average);
  }
variance = variance / ((float)(n-1));
standard_deviation = sqrt(variance);
return(standard_deviation);
}
	                    
			    
float general_correlation(float *x,float *y,int nx,int ny) {
int i,j,k,l;
int flag;
int gofar;
float sum_xi_minus_avg_over_yi_minus_avg;
float xaverage,yaverage;
float xstddev,ystddev;
float correlation_index;
gofar = leastest(nx,ny);
    
xaverage = general_average(x,gofar);
yaverage = general_average(y,gofar);
xstddev = general_stddev(x,gofar,xaverage);
ystddev = general_stddev(y,gofar,yaverage);
    
sum_xi_minus_avg_over_yi_minus_avg = 0.;
for (k=0;k<gofar;k++) {

  sum_xi_minus_avg_over_yi_minus_avg += (x[k]
                                           -xaverage) *   
                                            (y[k]
					    -yaverage);
      
  } /* for each data point that is in common between x and y */
    if (gofar > 4) {
        if ((xstddev == 0.0)||(ystddev == 0.0))
	   correlation_index=0.0; /* forget it.  straight lines are not interesting */
	else
          correlation_index = sum_xi_minus_avg_over_yi_minus_avg / 
    	     (((float)(gofar-1)) *
	      xstddev * ystddev);

    	}
    else {
    	correlation_index = 0.;
    	}
return(correlation_index);
}










int determine_causation(struct company *companies,int c1,int c2,float r,int debug) {
struct company *x;
struct company *y;
int offset;
int gofar;
int i;
int bestpos;
float bestnum;
float normalcorr;


x = companies + c1;
y = companies + c2;



/* first we check every x to y to see if there is a correlation 
  then we check every y to x to see if there is a correlation */
bestpos=0;
normalcorr = bestnum = general_correlation(x->timepoint,y->timepoint,
  x->number_timepoints,y->number_timepoints);
  
     
for (offset=1;offset< (y->number_timepoints/2);offset++) {
  float corr;
  corr = general_correlation(x->timepoint,
                                     y->timepoint+offset,
				     x->number_timepoints,
				     y->number_timepoints-offset);
  if (absf(corr) > absf(bestnum)) {
    bestnum = corr;
    bestpos = -offset;
    }     
  /*hueristic -- if we went 24 hour with nothing better,  quit */
  if ((offset>120)&&(bestpos==0)) break;
  }

for (offset=1;offset< (x->number_timepoints/2);offset++) {
  float corr;
  corr = general_correlation(x->timepoint+offset,
                                     y->timepoint,
				     x->number_timepoints-offset,
				     y->number_timepoints);
  if (absf(corr) > absf(bestnum)) {
    bestnum = corr;
    bestpos = offset;
    }     
  if ((offset>120)&&(bestpos==0)) break;
  }

if ((bestpos != 0)&&(absf(bestnum) >= 0.96)
   && (absf(bestnum)-absf(normalcorr) >0.07)
		  ) {
  if (bestpos >0) {
    fprintf(stdout,"%s leads %s by %d minutes %f %f\n",x->symbol,y->symbol,
             bestpos*30,bestnum,normalcorr);
    }
  else if (bestpos < 0) {
    fprintf(stdout,"%s leads %s by %d minutes %f %f\n",y->symbol,x->symbol,
             -(bestpos*30),bestnum,normalcorr);
    }
  }
/*else {
  fprintf(stdout,"%s correlates %s %f (%f)\n",x->symbol,y->symbol,normalcorr,
            r);
  }*/
}


main() {
int i,j,k,l;
int flag;
fprintf(stderr,"read...\n");
read_data(&companies,&number_companies);
fprintf(stderr,"analyze...\n");



current_time_seconds = derive_current_time_in_seconds(companies,number_companies);

fprintf(stderr,"  logpoints...\n");
compute_logpoint(companies,number_companies,current_time_seconds);

fprintf(stderr,"  evenly spaced time points...\n");
compute_timepoint(companies,number_companies,current_time_seconds);
fprintf(stderr,"  logrithmically  spaced time points...\n");

fprintf(stderr,"  average company...\n");
/* find average of entire stock market */
compute_average_company(companies,number_companies,&average);  


fprintf(stderr,"  compare all companies to average...\n");
for (i=0;i<number_companies;i++) {
  compute_over_average(companies+i,&average);
  compute_simple_state(companies+i);
  }


fprintf(stderr,"  correlate each to its other...\n");
/* set the correlation matrix up */
for (i=0;i<number_companies;i++) {
  companies[i].r = (float *)malloc(sizeof(float)*number_companies);
  }


/* this is the guts - compute the correlation on every stock to every other stock */  
for (i=0;i<number_companies;i++) {
  struct company *x;
  x = companies+i;
  fprintf(stderr,"%d ",i);
  x->r[i] = 0.; /* ignore self */
  for (j=i+1;j<number_companies;j++) {
    int gofar;
    struct company *y;
    float sum_xi_minus_avg_over_yi_minus_avg;
    float correlation_index;
    y = companies+j;
    
/*  flag is used to sinle out a specific computation for 
    debugging...
        flag = (i==hash_find("ITXC"))&&(j==hash_find("SFNCA"));
    if (!flag) { x->r[j]=0;y->r[i]=0;continue;}
    */
    gofar = leastest(x->number_log_points,y->number_log_points);
    
    
    sum_xi_minus_avg_over_yi_minus_avg = 0.;
    for (k=0;k<gofar;k++) {

      sum_xi_minus_avg_over_yi_minus_avg += (x->timelogpoint_over_average[k]
                                           -x->average) *   
                                            (y->timelogpoint_over_average[k]
					    -y->average);
      
      } /* for each data point that is in common between a and b */
    if (gofar > 4) {
        if ((x->standard_deviation == 0.0)||(y->standard_deviation == 0.0))
	   correlation_index=0.0; /* forget it.  straight lines are not interesting */
	else
          correlation_index = sum_xi_minus_avg_over_yi_minus_avg / 
    	     (((float)(gofar-1)) *
	      x->standard_deviation * y->standard_deviation);

    	}
    else {
    	correlation_index = 0.;
    	}
    x->r[j]= correlation_index;
    y->r[i]= correlation_index;

    } /* for each company a and b */ 
  } /* for each company */





/* print out interesting results */   
for (i=0;i<number_companies;i++) {
  struct company *x;
  int flag;
  x = companies+i;            
  fprintf(stderr,"%d ",i);
  flag=1;
/*  dump_company(x);*/
  for (j=i+1;j<number_companies;j++) {
	struct company *y;
	float correlation_index;
  	y = companies+j;       
  	correlation_index = x->r[j];                                   
  	if ((correlation_index > .94)||(correlation_index < -.92)) {
	  determine_causation(companies,i,j,correlation_index,0);
	flag=0;
	}
      
    }                                                                 
  }
}
