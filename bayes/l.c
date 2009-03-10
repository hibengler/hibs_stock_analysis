
/* This gets data from bihourly
step 1 :
ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash > simple.csv
pf = fopen("simple.csv","r");

*/

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "gen.h"

struct company
{
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

double last_date_in_seconds = 0.f;

double continual_add_data_line(struct company *companies, int *pnumber_companies, char *line2, int add_new_companies)

{
  int i;
  FILE *pf;
  char *line;
  size_t n;
  struct dataline dl;

                                 /* if we are not a header line */
  if (strncmp(line2,"name,symbol",11) != 0)
  {
    int position;
    struct company *c;
    char *x;
    line = line2;
    /* convert the line to the big structure of data */
    if (x=strtok(line,",")) strcpy(dl.name,x); else return(last_date_in_seconds);
    if (x=strtok(NULL,",")) strcpy(dl.symbol,x); else return(last_date_in_seconds);
    if (x=strtok(NULL,",")) strcpy(dl.quotedate,x); else return(last_date_in_seconds);
    if (x=strtok(NULL,",")) dl.currentPrice = atof(x); else return(last_date_in_seconds);
    dl.dateInSeconds = str_to_date(dl.quotedate);

    /*    fprintf(stderr,"\\%s,%s,%s,%s	%f %lf\n",
        dl.name,dl.symbol,dl.quotedate,x,dl.currentPrice,dl.dateInSeconds);*/

    /* ok now lets convert this to a point */

                                 /* if we cant find the ticker */
    if (((position=hash_find(dl.symbol))==-1)&& (add_new_companies))
    {

      hash_set(dl.symbol,*pnumber_companies);
      position= *pnumber_companies;

      c = companies + position;
      strcpy(c->symbol,dl.symbol);

      c->number_points = 5000;   /* guess */
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

      (*pnumber_companies)++;

    }                            /* new ticker found */
    else
    {
      c = companies + position;
    }

    c->point[c->number_points] = dl.currentPrice;
    c->seconds[c->number_points] = dl.dateInSeconds;
    c->number_points++;
    last_date_in_seconds = dl.dateInSeconds;
    return dl.dateInSeconds;
  }                              /* if we are a point line and not a title line */
  return last_date_in_seconds;
  line=NULL;
  n=0;
}


int read_data(struct company **pcompanies, int *pnumber_companies)

{
  struct company *companies;     /* stores the companies -- before we assign it to *pcompanies */
  int number_companies;          /* holds the number of companies - before we assign it to *pnumber_Companies */
  int i;
  FILE *pf;
  char line2[20000];
  char *line;
  size_t n;
  struct dataline dl;

  hash_init();
  /* set the environment to new york time */
  putenv("TZ=EST5EDT");
  /*pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
       ,"r"); */
  pf = fopen("simple.csv","r");
  if (!pf) return(-1);
  number_companies = 4000;       /* guess */
  companies = (struct company *)malloc(sizeof(struct company)*number_companies);
  i=0;
  number_companies = 0;
  n=0;
  while (fgets(line2,19999,pf) != NULL)
  {
    continual_add_data_line(companies,&number_companies,line2,1);
  }

  *pcompanies = companies;
  *pnumber_companies = number_companies;
}


float price_at_time(struct company *x,
double time)
{
  /* similart to compute logpoint,  except the intervals are bihourly
  and linear */
  int i;
  float ioff;
  float *pt;
  double *seconds;
  int k1,k2;

  pt = x->point;                 /* could be logpoint as well */
  seconds = x->seconds;

  k1=x->number_points-1;         /* k1 and k2 points are compared to determine a linear point in between them */
  k2=x->number_points-1;

  /* k1 is the index of the prior k.  K2 is the index of the next K.
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */

  while ((k2>0)&&(seconds[k2-1] > time)) k2--;

  if (k2==0)                     /* we are far back */
  {
    k1 = k2;
    if (seconds[k2] > time)
    {
      return(-1.);
    }
  }
  else                           /* we are not all the way back */
  {
    if (seconds[k2] <= time)
    {
      k1 = k2;
    }
    else
      k1 = k2 - 1;
  }

  if (seconds[k1]==seconds[k2])  /* dont divide by zero,  just pick one -- usually ka==k2 as well */
  {
    return(pt[k1]);
  }
  else
  {
    if  ((time -seconds[k1])> -1. && (time-seconds[k1] < 1.))
    {
      return(pt[k1]);
    }
    else
      return(  pt[k1]  + (pt[k2]-pt[k1]) *
        (time-seconds[k1]) /(seconds[k2]-seconds[k1]));
  }
}


int compute_logpoint(struct company *x, int number_companies)
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

  /* compute current_time_seconds */

  fprintf(stderr,"   current time seconds %d ...\n",i);
  current_time_seconds= 0.;
  for (i=0;i<number_companies;i++)
  {
    float currenttime;
    struct company *x;
    x = companies+i;
    if (x->number_points)
    {
      float xtime;
      currenttime = x->seconds[x->number_points-1];
      if (current_time_seconds < currenttime)
        current_time_seconds =currenttime;
    }

  }

  fprintf(stderr,"   find data points...\n");
  /* find data points logarithmized */
  for (i=0;i<number_companies;i++)
  {
    struct company *x;
    int j;
    float minpoint;

    x = companies+i;

    x->logpoint = (float *)malloc(sizeof(float) * x->number_points);

    minpoint = x->point[0];
    for (j=1;j<x->number_points;j++)
    {
      if (x->point[j] < minpoint) minpoint = x->point[j]-1.;
    }

    if (minpoint<1.)
    {
      for (j=0;j<x->number_points;j++)
      {
        x->logpoint[j] = logf(x->point[j]+minpoint+1.);
      }
    }                            /* if we need to fudge the logpoints */
    else
    {
      for (j=0;j<x->number_points;j++)
      {
        x->logpoint[j] = logf(x->point[j]);
      }                          /* for each point */
    }                            /* if out minpoint is not out of the ordinary */
  }                              /* for each company */

  fprintf(stderr,"   find adjusted log point...\n");
  /* ok.  now find the time adjusted log of the points
    this could be of points or logpoints -- right now,  I will do points so that we can see easily */
  for (i=0;i<number_companies;i++)
  {
    struct company *x;
    int j;int k1,k2;
    float *pt;
    double *seconds;

    /* setup */
    x = companies+i;
    pt = x->point;               /* could be logpoint as well */
    seconds = x->seconds;
    x->timelogpoint = (float *)malloc(sizeof(x) * NUMBER_LOG_POINTS);
    k1=x->number_points-1;       /* k1 and k2 points are compared to determine a linear point in between them */
    k2=x->number_points-1;
    number_of_log_points = 0;    /* goes up as we have data */

    for (j=0;j<NUMBER_LOG_POINTS;j++)
    {
      double index = current_time_seconds - secondsforlogpoint[j];
      /* k1 is the index of the prior k.  K2 is the index of the next K.
         if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
         if the time is less than our earliest sample time,  we are done.
         One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
         going back in time as far as you want to go.
         And we clip it if the stock hasent been around for as long
         */

      while ((k2>0)&&(seconds[k2-1] > index)) k2--;

      if (k2==0)                 /* we are far back */
      {
        k1 = k2;
        if (seconds[k2] > index)
        {
          break;
        }
      }
      else                       /* we are not all the way back */
      {
        if (seconds[k2] <= index)
        {
          k1 = k2;
        }
        else
          k1 = k2 - 1;
      }

                                 /* dont divide by zero,  just pick one -- usually ka==k2 as well */
      if (seconds[k1]==seconds[k2])
      {
        x->timelogpoint[j] = pt[k1];
      }
      else
      {
        x->timelogpoint[j] =  pt[k1]  + (pt[k2]-pt[k1]) *
          (index-seconds[k1]) /(seconds[k2]-seconds[k1]);

        /* point 1 + delta point / delta seconds * number of seconds beyond point 1 */
      }
      /* set the number of log points up -- we have a record! */
      number_of_log_points = j+1;
    }                            /* for each point */
    x->number_log_points = number_of_log_points;
  }                              /* for each company */

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
  for (i=0;i<number_companies;i++)
  {
    struct company *x;
    x = companies+i;
    if (a->resolution_seconds < x->resolution_seconds)
      a->resolution_seconds = x->resolution_seconds;
  }

  for (i=0;i<NUMBER_LOG_POINTS;i++)
  {
    int number_points;
    float sum;
    number_points = 0;

    sum = 0.;
    for (j=0;j<number_companies;j++)
    {
      struct company *x;
      x = companies+j;
      if (x->number_log_points > i)
      {
        number_points ++;
        sum = sum + x->timelogpoint[i];
      }
    }
    a->point[i] = number_points; /* hack */
    if (number_points)
    {
      a->timelogpoint[i] = sum / ((float)(number_points));
      a->number_log_points = i+1;
    }
    else
    {
      a->timelogpoint[i] = 0.;
    }

  }                              /* for each log point */

}


int compute_over_average(struct company *x,struct company *a)
{
  int i;
  x->timelogpoint_over_average = (float *)malloc(sizeof(float) * x->number_log_points);

  for (i=0;i<x->number_log_points;i++)
  {
    if (a->timelogpoint[i] != 0.)
    {
      x->timelogpoint_over_average[i] = x->timelogpoint[i]/* / a->timelogpoint[i]*/;
    }
    else
                                 /* hack */
      x->timelogpoint_over_average[i] = -1;
    x->timelogpoint_over_average[i] = x->timelogpoint[i];
  }
}


int compute_simple_state (struct company *x)
{
  int i;
  float average;
  float variance;
  average=0.;
  /* average */
  for (i=0;i<x->number_log_points;i++)
  {
    average += x->timelogpoint_over_average[i];
  }
  average = average / ((float)x->number_log_points);
  x->average = average;

  variance = 0.;
  for (i=0;i<x->number_log_points;i++)
  {
    float val = x->timelogpoint_over_average[i]-average;
    val = val*val;
    variance += val;
  }
  variance = variance / ((float)(x->number_log_points-1));
  x->variance = variance;

  x->standard_deviation = sqrt(variance);
}


int dump_company(struct company *x)
{
  int i,j,k;
  fprintf(stderr,"Company %s\n",x->symbol);
  fprintf(stderr,"Original points (%d): ",x->number_points);
  for (i=0;i<x->number_points;i++)
  {
    fprintf(stderr,"%f	",x->point[i]);
  }
  fprintf(stderr,"\n");

  if (x->logpoint)
  {
    fprintf(stderr,"log points (%d): ",x->number_points);
    for (i=0;i<x->number_points;i++)
    {
      fprintf(stderr,"%f	",x->logpoint[i]);
    }
  }
  fprintf(stderr,"\n");

  if (x->timelogpoint)
  {
    fprintf(stderr,"time log points (%d):",x->number_log_points);
    for (i=0;i<x->number_log_points;i++)
    {
      fprintf(stderr,"%f	",x->timelogpoint[i]);
    }
  }
  fprintf(stderr,"\n");

  if (x->timelogpoint_over_average)
  {
    fprintf(stderr,"time log points over average (%d):",x->number_log_points);
    for (i=0;i<x->number_log_points;i++)
    {
      fprintf(stderr,"%f	",x->timelogpoint_over_average[i]);
    }
  }

  fprintf(stderr,"\nAverage %f standard deviation %f variance %f\n",
    x->average,x->standard_deviation,   x->variance);
}


int compute_baysean_stats(struct company *x, int number_of_companies)
{
  /* companies * positions * points  input */
  /* companies * positions output */
  int positions;
  int points;
  int inputsize;
  int outputsize;
  int output_band_size;
  int sample_size;
  int i,j,k,l,m;
  int count;
  short *sum_inputs;
  short *sum_outputs;
  short *input;
  short *input2;
  short *output;
  short *sum_all;
  double base_time;
  double end_time;
  double point_offsets[10] = {1.,2.,4.,7.,12.,20.,33.,54.,88.,143.};
  int fileno;
  positions=9;
  points=3;
  inputsize = number_of_companies * points * positions;
  output_band_size =  1600000000 / (inputsize*2 * positions);
  outputsize = output_band_size * positions;
  fprintf(stderr,"inputsize %d output band size %d outputsize %d\n",inputsize,output_band_size,outputsize);
  count = 0;
  sum_inputs = calloc(sizeof(short),inputsize);
  input = calloc(sizeof(short),inputsize);
  output = calloc(sizeof(short),outputsize);
  sum_outputs = calloc(sizeof(short),outputsize);

  sum_all = (short *)calloc(sizeof(short),inputsize*outputsize);

  if (!sum_all) {fprintf(stderr,"No memory!\n"); exit(-1);}
  fprintf(stderr,"lets begin... all %x  in*out*2 %d\n",sum_all,inputsize*outputsize*2);

  base_time = x[0].seconds[0];
  fprintf(stderr,"base time %lf   to %lf\n",base_time,base_time + 900.*(2.+point_offsets[points-1]));
  base_time = base_time + 900.*(2.+point_offsets[points-1]);
  if (x[0].number_points)
  {
    end_time = x[0].seconds[x[0].number_points-1];
  }
  else
  {
    fprintf(stderr,"nothing loaded in\n");
  }
  fprintf(stderr,"end time %lf  est cycles %f\n",end_time, (end_time-base_time)/900.);
  sample_size =  (end_time-base_time)/900. -1.;

  fileno=0;
                                 /* each band one by one */
  for (i=0;i<number_of_companies;i+=output_band_size)
  {
    for (j=0;j<sample_size;j++)  /* each time point */
    {
      double time;
      time = base_time + (900.*((double)j));
      if (j % 100 == 0) fprintf (stderr,"Band %d time %d  -- %lf\n",i,j,time);
      for (k=0;k<number_of_companies;k++)
      {
        struct company *y;

        float res;
        int outrespos;
        float in;
        float out;

        y=x+k;
        in = price_at_time(y,time);

        /*	fprintf(stderr,"in=1 %s %lf is %f\n",y->symbol,time,in);*/

        for (m=0;m<points;m++)   /* m is each of the points to look at  in the past */
        {
          float in2,res;
          int respos;

          in2 = price_at_time(y,time-900.*point_offsets[m]);

          res = (in-in2)/in2;    /* raw change from the past */
          res = 1.0/(1.0+expf(-res*20.f));
          respos =  ((int)(res * ((double)(positions))) );
          if (respos==positions) respos--;
          /*   	  fprintf(stderr,"   %d in %s %lf is %f    res %f  respos %d\n",m,y->symbol,time-900.*point_offsets[m],in2,res,respos);*/

          for (l=0;l<positions;l++)
          {
            int loc;
            loc=k * positions*points + m* positions + l;
            short a;
            if (l==respos)
            {
              input[loc]=1;
              sum_inputs[loc]++;
            }
            else
            {
              input[loc] = 0;
            }
          }                      /* for each position */
        }                        /* for each point */

        /* wanna test -- here is where to do it */

        if ((k>=i)&&(k<i+output_band_size))
        {
          out = price_at_time(y,time+900.);

          /*	  fprintf(stderr,"out %s %lf is %f\n",y->symbol,time+900.,out);*/
          /* hack - avoid division by zero */
          if (in==0.f) in=0.001f;
          if (out==0.f) out=0.001f;
          res = (out-in)/in;     /* change predicted tfor the future form the past */

          res = 1.0/(1.0+expf(-res*20.f));
          outrespos =  ((int)(res * ((double)(positions))) );
          if (outrespos==positions) outrespos--;
          if (k==1519) brk();

          for (l=0;l<positions;l++)
          {
            int loc;
            loc=(k-i) * positions+l;
            if (l==outrespos)
            {
              output[loc]=1;
              sum_outputs[loc]++;
            }
            else
            {
              output[loc]=0;
            }
          }                      /* for each position */
        }                        /* if we should do output */

      }                          /* for each company*/

      /* input and output are set up in theory */
      {
        short *sum_pos;
        sum_pos = sum_all;
        for (k=i;k<output_band_size+i;k++)
        {
          sum_pos +=  inputsize;
          if (output[k])
          {
            int f;
            for (f=0;f<inputsize;f++)
            {
              sum_pos[f]+=input[f];
            }
          }                      /* for each input band */
        }                        /* for each output band */
      }                          /* block to add detail */

    }                            /* each time point */

    fprintf(stderr,"Writing this guy out\n");
    {
      FILE *xf;
      char xfilename[1000];
      sprintf(xfilename,"gzip >output%3.3d.dat.gz",fileno);
      xf = popen(xfilename,"w");
      if (!xf)
      {
        fprintf(stderr,"could not write out a band\n");
        exit(-1);
      }
      fprintf(stderr,"right on\n");
      fwrite(&fileno,4,1,xf);    /* file number*/
      fwrite(&i,4,1,xf);         /* integer with the band number*/
                                 /* integer with the output band size */
      fwrite(&output_band_size,4,1,xf);
      fwrite(&outputsize,4,1,xf);/* integer with the output band size */
      fwrite(&inputsize,4,1,xf); /* integer with the input band size*/
                                 /* integer with the learning sample size*/
      fwrite(&sample_size,4,1,xf);

                                 /* integer with the input band size*/
      fwrite(sum_outputs,sizeof(short),outputsize,xf);
                                 /* integer with the input band size*/
      fwrite(sum_inputs,sizeof(short),inputsize,xf);

                                 /* sum of everything */
      fwrite(sum_all,sizeof(short),inputsize*outputsize,xf);

      pclose(xf);

      fprintf(stderr,"Written\n");
    }

    /* reset the memory */
    memset(sum_inputs,0,sizeof(short)*inputsize);
    memset(sum_outputs,0,sizeof(short)*outputsize);
    fprintf(stderr,"about to clear the big one at %x\n",sum_all);
    memset(sum_all,0,sizeof(short)*inputsize*outputsize);

    fprintf(stderr,"memory cleaned up\n");
    fileno++;
  }                              /* for each band offset */

}


int predict_bayes(struct company *x, int number_of_companies)
{
  /* companies * positions * points  input */
  /* companies * positions output */
  int positions;
  int points;
  int inputsize;
  int outputsize;
  int output_band_size;
  int sample_size;
  int i,j,k,l,m;
  int count;
  short *sum_inputs;
  short *sum_outputs;
  short *input;
  short *input2;
  short *output;
  double *predict;
  short *sum_all;
  double base_time;
  double end_time;
  double last_time;
  double point_offsets[10] = {1.,2.,4.,7.,12.,20.,33.,54.,88.,143.};
  int fileno;
  positions=9;
  points=3;
  fileno=0;
  FILE *xf;
  char xfilename[1000];
  int number_of_bands;

  fprintf(stderr,"Reading first file\n");
  sprintf(xfilename,"gunzip <output%3.3d.dat.gz",fileno);

  xf = popen(xfilename,"r");
  if (!xf)
  {
    fprintf(stderr,"could not open first band\n");
    exit(-1);
  }
  fprintf(stderr,"groovy\n");
  fread(&fileno,4,1,xf);         /* file number*/
  fread(&i,4,1,xf);              /* integer with the band number*/
                                 /* integer with the output band size */
  fread(&output_band_size,4,1,xf);
  fread(&outputsize,4,1,xf);     /* integer with the output band size */
  fread(&inputsize,4,1,xf);      /* integer with the input band size*/
  fread(&sample_size,4,1,xf);    /* integer with the input band size*/

  number_of_bands = (number_of_companies + (output_band_size-1) ) / (output_band_size);

  sum_inputs = calloc(sizeof(short),inputsize);
  input = calloc(sizeof(short),inputsize);
  output = calloc(sizeof(short),outputsize*number_of_bands);
  predict = calloc(sizeof(double),outputsize*number_of_bands);
  sum_outputs = calloc(sizeof(short),outputsize*number_of_bands);

                                 /* integer with the input band size*/
  fread(sum_outputs,sizeof(short),outputsize,xf);
                                 /* integer with the input band size*/
  fread(sum_inputs,sizeof(short),inputsize,xf);

                                 /* only one line at a time in this version */
  sum_all = (short *)calloc(sizeof(short),inputsize);

  if (!sum_all) {fprintf(stderr,"No memory!\n"); exit(-1);}
  pclose(xf);                    /* close this out,  we will read it again */

  {
    int i;
    FILE *pf;
    char line2[20000];

    fprintf(stderr,"opening the current feed (simple2.csv)\n");
    putenv("TZ=EST5EDT");
    /*pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
         ,"r"); */
    pf = fopen("simple2.csv","r");
    if (!pf) return(-1);
    i=0;
    number_companies = 0;
    while (fgets(line2,19999,pf) != NULL)
    {
      double time;
      if (line2[0]=='*')
      {
        if (line2[1]=='p')       /* predict */
        {
          time = str_to_date(line2+2);

          base_time = time;

          fprintf(stderr,"base time %lf   to %lf\n",time,time + 900.*(2.+point_offsets[points-1]));
          base_time = base_time + 900.*(2.+point_offsets[points-1]);
          if (x[0].number_points)
          {
            end_time = x[0].seconds[x[0].number_points-1];
          }
          else
          {
            fprintf(stderr,"nothing loaded in\n");
          }

          {

            for (k=0;k<number_of_companies;k++)
            {
              struct company *y;

              float res;
              int outrespos;
              float in;
              float out;

              y=x+k;
              in = price_at_time(y,time);

              /*	fprintf(stderr,"in=1 %s %lf is %f\n",y->symbol,time,in);*/

                                 /* m is each of the points to look at  in the past */
              for (m=0;m<points;m++)
              {
                float in2,res;
                int respos;

                in2 = price_at_time(y,time-900.*point_offsets[m]);

                                 /* raw change from the past */
                res = (in-in2)/in2;
                res = 1.0/(1.0+expf(-res*20.f));
                respos =  ((int)(res * ((double)(positions))) );
                if (respos==positions) respos--;
                /*   	  fprintf(stderr,"   %d in %s %lf is %f    res %f  respos %d\n",m,y->symbol,time-900.*point_offsets[m],in2,res,respos);*/

                for (l=0;l<positions;l++)
                {
                  int loc;
                  loc=k * positions*points + m* positions + l;
                  short a;
                  if (l==respos)
                  {
                    input[loc]=1;
                    /*	      sum_inputs[loc]++;*/
                  }
                  else
                  {
                    input[loc] = 0;
                  }
                }                /* for each position */
              }                  /* for each point */

              /* wanna test -- here is where to do it */
            }                    /* for each company*/

            /* input is set up  - remember - the file is open - so we keep it open */

                                 /* each band one by one */

            fileno=0;
            for (i=0;i<number_of_companies;i+=output_band_size)
            {

                                 /* if we have another band to read */
              {
                int junk;
                sprintf(xfilename,"gunzip <output%3.3d.dat.gz",fileno);
                xf = popen(xfilename,"r");
                if (!xf)
                {
                  fprintf(stderr,"could not open band %d\n",fileno);
                  exit(-1);
                }
                fprintf(stderr,"groovin %d\n",fileno);
                                 /* file number*/
                fread(&junk,4,1,xf);
                                 /* integer with the band number*/
                fread(&junk,4,1,xf);
                                 /* integer with the output band size */
                fread(&junk,4,1,xf);
                                 /* integer with the output band size */
                fread(&junk,4,1,xf);
                                 /* integer with the input band size*/
                fread(&junk,4,1,xf);
                                 /* integer with the input band size*/
                fread(&junk,4,1,xf);

                                 /* integer with the input band size*/
                fread(sum_outputs + i*positions ,sizeof(short),outputsize,xf);
                                 /* integer with the input band size*/
                fread(sum_inputs,sizeof(short),inputsize,xf);
                /* ready to continue reading the output probability bands */
              }                  /* if we had to open the next band */

              /* input is set up in theory we have output set to what it will be.  Lets compute predict */
              {
                short *sum_pos;
                sum_pos = sum_all;

                for (k=i;(k-i<output_band_size)&&(k<number_of_companies);k++)
                {
                  int kt=k*positions;
                  if (k==1519) brk();
                  for (l=0;l<positions;l++)
                  {
                    int kpl = kt+l;

                    /* prediction is p(words|cat) * p(cat) / p (words)
                     p(w1 | cat) * p(w2 | cat) * p(w3 | cat)    * p(cat)
                    ---------------------------------------------------
                    p(w1) * p(w2) * p(w3) * p(w4)

                    = nw1/ncat * nw2/ncat * nw3/ncat * nw4/ncat     * ncat/ntotal
                    -------------------------------------------------------------
                    nw1/ntotal * nw2/ntotal * nw3/ntotal * nw4/ntotal

                    Or log(nw1)-log(ncat) + log(nw2) - log(ncat) + nog(ncat) - log(ntotal)
                    -----------------------------------------------------------------------
                    nw1/ntotal * nw2/total ...

                    */

                    /*        sum_pos +=  inputsize;*/
                                 /* same as ncat/ntotal above */
                    predict[kpl] = (double)(sum_outputs[kpl])/(double)(sample_size);
                                 /* this one line */
                    {
                      int y;
                      /*        fprintf(stdout,"%d\n",k);*/
                      y=fread(sum_all,sizeof(short),inputsize,xf);
                      if (y<inputsize)
                      {
                        fprintf(stdout,"Error band %d line %d y %d\n",i,k,y);
                      }
                    }            /* for each output line in a band */

                                 /* if we have work to do */
                    if (predict[kpl] != 0.)
                    {
                      int f;
                      double logsample_size = log(sample_size);
                                 /* convert to logarithms */
                      predict[kpl] = log(predict[kpl]);

                      for (f=0;f<inputsize;f++)
                      {
                                 /* if this is in our input */
                        if (input[f])
                        {
                          if (sum_inputs[f])
                          {
                            if (sum_all[f])
                            {
                              /*                    fprintf(stderr,"predict %d to %d: %lf + log  ( %d / %d)    = %lf\n",
                                                    k,f,predict[k],sum_all[f],sum_inputs[f],  predict[k] + log((double)(sum_all[f]) / ((double)(sum_inputs[f]))) );
                              */
                              predict[kpl] += log((double)(sum_all[f])) - log(((double)(sum_inputs[f])));
                            }

                          }
                          else
                          {
                            /* estimate this based on other actions */
                            int z;
                            z = (f % positions);
                            while (1)
                            {
                              if (z < positions/2) z++;
                              if ( z> positions/2) z--;
                              if (sum_inputs[f]) break;
                              if (z == positions/2)
                              {
                                fprintf(stderr,"input sum not there wtf?\n");
                                break;
                              }
                            }    /* while */
                            if (sum_inputs[f])
                            {
                              if (sum_all[f])
                                predict[kpl] += log((double)(sum_all[f])) - log(((double)(sum_inputs[f])));
                            }

                          }
                        }        /* if we have an input */
                      }          /* for each input */
                    }            /* if we have any chance of doing this record */
                    else
                    {
                                 /* big negative number */
                      predict[kpl] = -17000000.f;
                    }
                  }
                                 /* we need to adjust the last position outputs based on the position */
                  {
                    int i;
                    int count;
                    double largest;
                    double sum;
                    largest = 0.f;
                    count=0;
                    /* We want the chances to add up to 100%.  But we did not factor everything right.  so we will use the
                    log positions as a way to convert this.
                    for everything to add up to 100%,  this means that we need to scale things.  We will find the largest chance,  and make that
                      1  ( by suntracting the largest from each,  it scales it back - so log(largest) = 0 , therefore largest = 1.
                      Then we exp everything,  and then scale it */

                    /* find the largest logarithm */
                    for (i=k*positions; i<k*positions+positions; i++)
                    {
                                 /*should no tbe ignored */
                      if (predict[i] !=  -17000000.f)
                      {
                        if (!count) largest=predict[i];
                        else
                        {
                          if (largest < predict[i]) largest=predict[i];
                        }
                        count++;
                      }          /* if we have a prediction */
                    }            /* for each item */

                    if (count==0)
                    {
                      /* assume no movoment - if nothing is predicted */
                      predict[k*positions+(positions/2)]=1.f;
                      largest=1.f;
                      fprintf(stderr,"Output has no possible outcomes - wtf?\n");
                    }
                    /* subtract and antilog everything , also figure out the scale needed */
                    sum = 0.f;
                    for (i=k*positions; i<k*positions+positions; i++)
                    {
                                 /*should no tbe ignored */
                      if (predict[i] !=  -17000000.f)
                      {
                        predict[i] = exp(predict[i]-largest);
                        sum += predict[i];
                      }
                      else
                      {
                        predict[i] = 0.f;
                      }
                    }

                    sum = 1/sum;
                    /* normalize so that everythign adds up to 100% */
                    for (i=k*positions; i<k*positions+positions; i++)
                    {
                      predict[i] *= sum;
                      if (((int)(predict[i]*100.f)))
                        fprintf(stderr,"%d %d %d\n",k,i-(k*positions),((int)(predict[i]*100.f)));
                    }            /* for each item */

                  }              /* bandpass computation */

                }                /* for each output line in a band */
                pclose(xf);
                fileno++;
              }                  /* band block */
            }                    /* for each output band */

            /* we have out full prediction */
          }                      /* for each line */

        }
        else if (line2[1]=='v')  /*verify prediction */
        {
          time = str_to_date(line2+2);
          fprintf(stderr,"would verify here\n");
        }

      }                          /* if special command */
      else
      {
                                 /* ignore new companies */
        time =continual_add_data_line(companies,&number_companies,line2,0);
      }
    }

  }
}


main(int argc, char *argv)
{

  if (argc<2)
  {
    int i,j,k,l;
    int flag;
    fprintf(stderr,"read...\n");
    read_data(&companies,&number_companies);
    fprintf(stderr,"analyze...\n");

    fprintf(stderr,"  compare all companies to average...\n");
    for (i=0;i<number_companies;i++)
    {
      compute_over_average(companies+i,&average);
      compute_simple_state(companies+i);
    }

    compute_baysean_stats(companies,number_companies);
  }

  else
  {

    int i,j,k,l;
    int flag;
    fprintf(stderr,"test read...\n");
    read_data(&companies,&number_companies);
    fprintf(stderr,"analyze...%d companies\n",number_companies);

    fprintf(stderr,"  compare all companies to average...\n");
    for (i=0;i<number_companies;i++)
    {
      compute_over_average(companies+i,&average);
      compute_simple_state(companies+i);
    }

    fprintf(stderr,"test...\n");
    predict_bayes(companies,number_companies);
  }

  exit(0);

}


int brk()
{
  fprintf(stderr,"break\n");
}
