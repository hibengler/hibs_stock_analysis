
      
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

      /* This simplifies the data coming in so it can be read quicker */      
                         


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




int convert_data() 

{
int i;
FILE *pf;
char line[100000];
size_t n;
struct dataline dl;

/* set the environment to new york time */
pf = popen("ls data/bi* | awk '{print \"unzip -p \" $1}'  | bash"
     ,"r");
if (!pf) return(-1);
i=0;
n=0;
while (fgets(line,99999,pf) != NULL) {

  if (strncmp(line,"name,symbol",11) != 0) { /* if we are not a header line */
    int position;
    struct company *c;
    char *x;
    dl.currentPrice=-1.;
    dl.volume=-1.;
    /* convert the line to the big structure of data */    
    if (x=strtok(line,",")) strcpy(dl.name,x); else continue;
    if (x=strtok(NULL,",")) strcpy(dl.symbol,x); else continue;
    if (x=strtok(NULL,",")) strcpy(dl.quotedate,x); else continue;
    if (x=strtok(NULL,",")) dl.currentPrice = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.changeValue = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.changePercent = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.previousClose = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.open = atof(x); else continue;
    if (x=strtok(NULL,",")) dl.volume = atof(x); else continue;
    
    

        
    /* ok now lets convert this to a point */
    fprintf(stdout,"x,%s,%s,%f,0,0,0,0,%f\n",dl.symbol,dl.quotedate,
    dl.currentPrice,dl.volume);
      
    
    } /* if we are a point line and not a title line */
  
  n=0;
  } /* while reading lines */


}
                            
                            
                            
			    
main() {
fprintf(stderr,"converting to smaller file...\n");
convert_data();
}
