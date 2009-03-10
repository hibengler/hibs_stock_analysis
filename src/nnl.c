/* NNL
This tries an NNL on the stocks and tries to predict what will come next.
*/
      
#include "gen.h"


#include "lwneuralnet.h"      
	    
struct asset {
  struct causation *cause;
  struct company *company;
  float number_of_shares;
  float purchase_price;
  double purchase_time;
  float projected_sell_price;
  double projected_sell_time;
  };
  

	    
struct causation {
  struct company *leadcompany;
  struct company *followcompany;
  double correlation_factor;
  double nolead_correlation_factor;
  double timeinminutes;
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
	 int number_leaders;
	 int number_followers;
	 struct causation ** leaders;
         struct causation ** followers;
	 };
         

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


       
struct asset assets[1000];
int number_assets;
	      
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
						  
									                            
                         






int read_data_limit(struct company **pcompanies, int *pnumber_companies) 

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

number_companies = 4000; /* guess */
companies = (struct company *)malloc(sizeof(struct company)*number_companies);

/* open the nnl_test.txt  to get the list of companies that we will look at */
pf = fopen("nnltest.txt","r");
if (!pf) exit(-1);
number_companies = 0;
line=NULL;
while (getline(&line,&n,pf) != -1) {
  int position;
  struct company *c;
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) continue;
  if ((position=hash_find(line))==-1) { /* if we cant find the ticker */
      hash_set(line,number_companies);
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
      c->number_leaders = 0;
      c->number_followers=0;
      c->leaders = NULL;
      c->followers = NULL;
      number_companies++;
    } /* if we are not duplicated*/
  }
fclose(pf);
  

/*pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
     ,"r");*/
pf = fopen("nnltest.csv","r");
if (!pf) return(-1);
i=0;
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
      
    if ((position=hash_find(dl.symbol))==-1) {
      /* if we cant find the ticker */
      continue; /* then ignore it */
      }
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

pt = x->point; /* could be logpoint as well */
seconds = x->seconds;
  
k1=x->number_points-1; /* k1 and k2 points are compared to determine a linear point in between them */
k2=x->number_points-1;    
  
  /* k1 is the index of the prior k.  K2 is the index of the next K.  
        if the time we are selecting is greater than our last sample time,  we give the latest and greatest sample.
        if the time is less than our earliest sample time,  we are done.
        One ntoe -- the logarithmic data is in reverse order,  with current being index poition 0,  and all other index positions
        going back in time as far as you want to go.
        And we clip it if the stock hasent been around for as long
        */
	
    
while ((k2>0)&&(seconds[k2-1] > time)) k2--;
    
    
if (k2==0) { /* we are far back */
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
    k1 = k2 - 1;
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
                     
		     
		     



struct asset possibilities[10000];
int number_possibilities=0;



int decide(struct company *comp) {
/* look for any following records.  and find the following records
where the rate of return is maximum */
int k;
struct causation *c;
for (k=0;k<comp->number_followers;k++) {
  struct asset *a;
  c = comp->followers[k];
  /*
  make a list of fake assets -  and then we will look at the 
  rate of return and amount of return
  rate of return is (projected_sell_price-purchase_price)/projected_sell_time
  so we look at the fake assets -- and group assets int long and short term
  based on the median amount of time.
  For each one,  find the biggest rate of return -- based on the amount of money
  to be had.  And see if that rate - 10% - $20.00 comission is >0
  if so,  invest,  otherwise wait.
  Then the next thing is to look at projected assets and see how they are doing.
  for right now,   we will hold until the projected rate or the progected sell time
  is met.  and then sell.
  For money investment -- invest 1/3,  1/3,  and keep 1/3 in cash.
  So the first time,  we will invest 3000, 3000,  and nothing
  the second time,  we might invest 1000,1000, and nothing -- if the deal is really good.
  
  */
  a=possibilities+number_possibilities;
  a->cause = c;
  a->company=c->followcompany;
  a->number_of_shares=1;
  if (c->correlation_factor >0) { /* positive correlation */
    a->purchase_price = c->followcompany->point[c->followcompany->number_points-1];
    a->projected_sell_price = 
                          a->purchase_price *
			  c->leadcompany->point[c->leadcompany->number_points-1]
			  /
			  price_at_time(c->leadcompany,
			 c->leadcompany->seconds[
			 c->leadcompany->number_points-1] - c->timeinminutes*60.);
   }
 else { /* negative correlation */
    a->purchase_price = c->followcompany->point[c->followcompany->number_points-1];
    a->projected_sell_price = 
                          a->purchase_price *
			  price_at_time(c->leadcompany,
			 c->leadcompany->seconds[
			 c->leadcompany->number_points-1] 
			 - c->timeinminutes*60.)
			  /
			  c->leadcompany->point[c->leadcompany->number_points-1]
			  ;
  }  
  
a->purchase_time = c->leadcompany->seconds
                             [c->leadcompany->number_points-1];
  a->projected_sell_time = c->timeinminutes*60.+a->purchase_time;
  if (a->projected_sell_price >0) 
    number_possibilities++;
  }
			 
  
}








int dump_cause(struct causation *c) 
{
fprintf(stdout," cause %s(%f->%f) leads %s by %lf hours %lf\n",
  c->leadcompany->symbol,
  price_at_time(c->leadcompany,
	        c->leadcompany->seconds[c->leadcompany->number_points-1] 
		- c->timeinminutes*60.),
  c->leadcompany->point[c->leadcompany->number_points-1],
  c->followcompany->symbol,
  c->timeinminutes/60.,c->correlation_factor);
}





int sell(int index) {
struct asset *a;
struct company *com;
float final_price;
a=assets+index;
com=a->company;
final_price = a->number_of_shares * com->point[com->number_points-1];
if (a->purchase_price < a->projected_sell_price) {
  fprintf(stdout,"sell %s %f profit %f expected profit %f\t",
     com->symbol,final_price,final_price - 
                   (a->number_of_shares * a->purchase_price) - 20.,
                   (a->number_of_shares * (a->projected_sell_price -
		                            a->purchase_price)) - 20.);
  a->company=NULL; /* blank it out */
  assets[0].number_of_shares += final_price - 10.;
  }
else { /* buyshort */
  fprintf(stdout,"buyshort %s %f profit %f expected profit %f\t",
     com->symbol,final_price,final_price -
                   (a->number_of_shares * a->purchase_price) - 20., 
                   (a->number_of_shares * (a->projected_sell_price -
		                            a->purchase_price)) - 20.);
  a->company=NULL; /* blank it out */
  assets[1].number_of_shares += a->number_of_shares * a->purchase_price *2.;  
  assets[0].number_of_shares -= a->number_of_shares * a->purchase_price * 2.;
  assets[0].number_of_shares += final_price 
                                - 10.;
  }
dump_cause(a->cause);
}

		   


int buy(struct asset *asset) {
int i;
for (i=2;i<=number_assets;i++) { /* 1 because 0 is cash and 1 is reserved cash */
  struct asset *a;
  a = assets+i;
  if ((a->company == NULL)||(i==number_assets)) { /* find a blank one or go to the end */
    a->company = asset->company;
    a->purchase_price = asset->purchase_price;
    a->purchase_time = asset->purchase_time;
    a->projected_sell_price = asset->projected_sell_price;
    a->projected_sell_time = asset->projected_sell_time;
    a->cause = asset->cause;
    if (a->purchase_price < a->projected_sell_price) {
      a->number_of_shares = (float)((int)(((assets[0].number_of_shares* 0.333333333) - 10.) 
                                / a->purchase_price));
      assets[0].number_of_shares -= (a->number_of_shares * a->purchase_price 
                                      + 10.);
      fprintf(stdout,"buy %s %f shares at %f (to %f) %f total\n",
          a->company->symbol,a->number_of_shares,a->purchase_price,
	     a->projected_sell_price,
	     (a->number_of_shares * a->purchase_price
	                                           + 10.
						   ));
      }
    else {
      a->number_of_shares = -(float)((int)(((assets[0].number_of_shares* 0.333333333) - 10.) 
                                / a->purchase_price));
      assets[0].number_of_shares -= 10. - 
         (a->number_of_shares * a->purchase_price);
      assets[1].number_of_shares -= (a->number_of_shares * a->purchase_price)
                                    ;
      assets[1].number_of_shares -= (a->number_of_shares * a->purchase_price);
      /* once for the just in case amount,  once for the amount we bought */
      fprintf(stdout,"sell_short %s %f shares at %f %f total\n",
          a->company->symbol,a->number_of_shares,a->purchase_price,
	     (a->number_of_shares * a->purchase_price
	                                           + 10.));
     }
    if (i==number_assets) number_assets++;
    dump_cause(a->cause);
    break;
    } /* if we found an empty asset slot */
    
  }    
}



int dump_assets(double currenttime) {
struct asset *a;
int i;
float sum;
a=assets;

fprintf(stdout,"%lf money %f ",currenttime,a->number_of_shares);
sum = a->number_of_shares;
a++;
if (a->number_of_shares != 0. ) {
  fprintf(stdout," res %f ", a->number_of_shares);
  sum += a->number_of_shares;
  }
for (i=2;i<number_assets;i++) {
  float amount;
  a=assets+i;
  if (a->company) {
    amount = a->company->point[a->company->number_points-1]*a->number_of_shares;
    sum += amount;
    fprintf(stdout," %s %f",a->company->symbol,amount);
    }
  }
fprintf(stdout,"  = %f\n",sum);
}





int trade(double currenttime) {
struct asset *money;
struct asset *a;
float best_score;
int best_possibility;
int i;
int h;

  
for (i=2;i<number_assets;i++) { /* 1 because asset 0 is money and 1 is reserved moneyh*/
  struct asset *a;
  a=assets+i;
  if (a->company) {
    if (a->projected_sell_price> a->purchase_price) {
      if ((currenttime>= a->projected_sell_time)||(a->company->point
                                       [a->company->number_points-1] > 
                a->projected_sell_price*1.03))
        /* if time is up,  or if we got a better price than what we bargained for */
        { 
        sell(i); /* GET OUT!!!! */
        }
      }
    else { /* short position */
      if ((currenttime>= a->projected_sell_time)||(a->company->point
                                       [a->company->number_points-1] < 
                a->projected_sell_price*0.97))
        /* if time is up,  or if we got a better price than what we bargained for */
        { 
        sell(i); /* GET OUT!!!! */
        }      
      }
    }
  }
  

money = assets;
for (h=0;h<2;h++) {
  best_possibility=-1;
  best_score=0.; /* might up this later */
  for (i=0;i<number_possibilities;i++) {
    float score;
    int j;
    a=possibilities+i;
    for (j=2;j<number_assets;j++) {/* see if this asset is owned */
      if (assets[j].company == a->company)
        break;
      }
    if (j!= number_assets) continue; /* skip to the next one */
    
    if (a->projected_sell_price > a->purchase_price) {
      score = (float)((int)((money->number_of_shares/3.0)/a->purchase_price))
               * a->projected_sell_price * 0.90
	     -
             (float)((int)((money->number_of_shares/3.0)/a->purchase_price))
               * a->purchase_price
	       - 20.;
      /* score is 1/3 of the money - the 20 dollars comission * 90 percent of the expected 
       return.  devided by the change in time
       so a high return -- over a year is less advantageous than
        a middle return in a couple of days. */
      }
    else {
      score = - (float)((int)((money->number_of_shares/3.0)/a->purchase_price))
               * a->projected_sell_price * 1.10
	     +
             (float)((int)((money->number_of_shares/3.0)/a->purchase_price))
               * a->purchase_price
	       - 20.;
      }    
    if (score > best_score) {
      best_score = score;
      best_possibility = i; 
      }
    } /* for each possibility */
  if ((best_possibility!= -1) &&(best_score>=0.)) {
    buy(possibilities+best_possibility);
    for (i=best_possibility+1;i<number_possibilities;i++) {
      possibilities[i-1] = possibilities[i];
      }
    number_possibilities--;
    } /* if we found a buy */
  } /* for each of our two buys */
number_possibilities=0;
dump_assets(currenttime);

}     
	   
	   
	   
	   
int make_money(struct company *pcompanies,int *pnumber_companies,
  struct asset *assets, int *pnumber_assets) 

{
int i;
FILE *pf;
char *line;
size_t n;
struct dataline dl;
double priordateinseconds;

/* set the environment to new york time */
putenv("TZ=EST5EDT");
pf = popen("ls data2/bi* | awk '{print \"unzip -p \" $1}'  | bash"
     ,"r"); 
/*pf = fopen("simple.csv","r");*/
if (!pf) return(-1);
i=0;
line = NULL;
n=0;
priordateinseconds=-1.;
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
    
	
	    
    /* see if we should buy and sell */
    if (priordateinseconds!= dl.dateInSeconds) {
      if (priordateinseconds != -1.) {
        trade(priordateinseconds);
	}    
      priordateinseconds = dl.dateInSeconds;
      }
    /* ok now lets convert this to a point */
      
    if ((position=hash_find(dl.symbol))==-1) { /* if we cant find the ticker */
/*      fprintf(stderr,"ignore new company %s\n",dl.symbol);*/
      continue;
      } /* new ticker found */
    else {
      c = companies + position;
      }

    c->point[c->number_points] = dl.currentPrice;
    c->seconds[c->number_points] = dl.dateInSeconds;
    c->number_points++;
    decide(c);
    } /* if we are a point line and not a title line */
  
  if (line) free(line);
  line=NULL;
  n=0;
  } /* while reading lines */

if (line) free(line);

}

			    
							                            
                            
int make_nnl(struct company *pcompanies,int *pnumber_companies,network_t **resultnet)
{
int i;
FILE *pf;
char *line;
size_t n;
struct dataline dl;
double priordateinseconds;
int number_spots;
float input[5000];
float target[5000];
float output[5000];
network_t *net;
int ii;
int k;
int redo;

int number_input;
struct company *base_company;

number_spots = 500/(number_companies+2);
number_input = (number_companies+2)*number_spots;

/* create the neural network; */
net = net_allocate (3,number_input,5,number_companies+2);

/*net_use_bias(net,0);*/

/* we will use the first company to find the points */
base_company = companies;
while (!(base_company->point)) base_company++;

for (redo=0;redo<10;redo++) {


for (ii = 1; ii < base_company->number_points; ii++)
{
  int inputpos;
  /*Set input to i-th input element of training set;*/
  inputpos=0;  
  for (k=ii-1;k>=0;k--) {
    double seconds = base_company->seconds[k];

    if (inputpos>= number_input) break;
    
    input[inputpos++] = ((float)((int)(seconds / 86400.)))/17000.; /* day number */
    input[inputpos++] = ((float)((int)(seconds - 86400*
                                ((int)(seconds / 86400.)))))/86400.; /* second number */
    for (i=0;i<number_companies;i++) {
      struct company *x;
      x = companies+i;
      input[inputpos++] =x->point[k]* 0.001;
      } /* for each company we are comparing */
    } /* for each data point in the past we know */
  if (inputpos < number_input) {/* not yet full */
    continue;
    }
  
  /*Set target to intended output for this input;*/
  {  
    double seconds = base_company->seconds[ii];
    int targetpos=0;

    target[targetpos++] = ((float)((int)(seconds / 86400.)))/ 17000.; /* day number */
    target[targetpos++] = ((float)((int)(seconds - 86400*
                                ((int)(seconds / 86400.)))))/ 86400.; /* second number */
    for (i=0;i<number_companies;i++) {
      struct company *x;
      x = companies+i;
      target[targetpos++] =x->point[ii]* 0.001;
      } /* for each company we are comparing */
    } /* block to set the intended output */
    
  net_compute(net, input, output);
  net_compute_output_error(net, target);
  if (((redo*1900+ii)% 500) ==0) {
  printf("%d..........%d\n",redo,ii);
  for (i=0;i<number_companies+2;i++) {
    printf("%f\t",target[i]*1000.);
    }
  printf("\t -->\n");

  for (i=0;i<number_companies+2;i++) {
    printf("%f\t",output[i]*1000.);
    }
  printf("\n");
  for (i=0;i<number_companies+2;i++) {
    printf("%d%c\t",(int)(fabs(target[i]-output[i])/output[i]*100),'%');
    }
  printf("\n");
}
 net_train(net);
  } /* for each training point */
} /* for each redo */
/* might save the net */
*resultnet = net;
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





				      
								      		      	      	      
       
       


 
 
 













int init_assets(struct asset *assets,int *pnumber_assets) {
struct asset *a;
a=assets; /* a will be the first asset - money */
a->company=NULL; /* no company for money */
a->number_of_shares = 100000.; /* 1000 shares */
a->purchase_price = 1.; /* 1 dollar */
a->projected_sell_price = 3000000; /* price of when to sell the stock */
a->projected_sell_time = 700000000000.; /* future */
a++; /* held assets for selling short and such */
a->company=NULL; /* no company for money */
a->number_of_shares = 0; /* no shares */
a->purchase_price = 1.; /* 1 dollar */
a->projected_sell_price = 3000000; /* price of when to sell the stock */
a->projected_sell_time = 700000000000.; /* future */
*pnumber_assets=2;
}








main(int argc,char *argv[]) {
int i,j,k,l;
int flag;
network_t *net;

fprintf(stderr,"read...\n");
read_data_limit(&companies,&number_companies);
fprintf(stderr,"done reading...\n");


current_time_seconds = derive_current_time_in_seconds(companies,number_companies);

init_assets(assets,&number_assets);

fprintf(stderr,"making nnl...\n");
make_nnl(companies,&number_companies,&net);

net_save(argv[1],net);


/*make_money(companies,&number_companies,assets,&number_assets);*/
exit(0);
}
