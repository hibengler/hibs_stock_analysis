/* 
Input:
	companies.dat
	roster.dat   - list of correlation sets to look at
	argv(1) - learning

output:
	trained.net
	
argv(1) is normalized all companies
we do the following:
skip first line - absolutes

second line - record every key:
AAPL
AAPL_012345678 
Record every combination
AAPL_1_INTL_4

Second line - do the same as the first line but also look at the past combinations
AAPL_1_INTL_4|AAPL_2
Old    old    new
For each one.



Then when done - read from standard input - discard first one.
For each stock
  for each number 0-8
    For each combination possible
      find probablility if it exists and apply:
        take p(a|b) = p (b|a) p(a) / p(b)
           p(AAPL_2|AAPL_1_INTL_4) = APPL_1_INTL_4|AAPL_2 / AAPL_2 * AAPL_2 / POINTS
	                            / APPL_1_INTL_4 / POINTS 
                                   = APPL_1_INTL_4|AAPL_2 / AAPL_2 * APPL_2
				     / APPL_1_INTL_4
				   = AAPL_1_INTL_4|AAPL_2 / APPL_1_INTL_4
           we store p(a|b) multiplicitvely and 1-p(a|b)          
	   then we take pa*pb*pN / pa*pb*pN + (1-pa)(1-pb)(1-PN)
	   
	   
	   
	   


*/
#define HASHSIZE 50000001
      
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
  hash_set(line,number_companies);
  companies[number_companies++] = strdup(line);
  }
return(1);
}
  


float decode(int x) {
return (((float)x) - 4.0)*0.25; /* --4 to 4 into -1 to 1*/
}

int code(float x)
{
int c;
/* x is between -1 and 1 */
x = x*0.5 + 0.5;
/* now between 0 and 1 */
c = (x * 10.); /* 0 - 10 but 0 is only a boundary and so is 10 */
c--; /* -1 - 9 */
if (c==-1) c++; /* 0-9 */
else if (c==9) c--; /*0-8 with 4 the origin */
return(c);
}





struct roster {
char *name;
int id;
struct roster *next;
};

struct roster *rosters[10000];
int number_rosters = 0;




/* read the list of companies */
int read_roster(FILE *pf) {
char line[20000];

if (! fgets(line,19999,pf)) return(0);
{
  int position;
  struct roster *r,*p;
  char *head;
  char *cp;
  line[strlen(line)-1] ='\0'; /* clip the \n off  and leave two sentinetals */
  if (line[strlen(line)-1] ==' ') line[strlen(line)-1] ='\0'; /* clip the extra space off  and leave two sentinetals */
  if (!(*line)) return(read_roster(pf));
  p = NULL;
  head = line;
  while (1) {
    cp = head;
    while (1) {
      if (*cp == ' ') {
        *cp = '\0';
	}
      if (*cp == '\0') {
        r = malloc(sizeof(struct roster));
	r->name = strdup(head);
	r->id = hash_find(head);
	r->next = NULL;
	if (p) p->next = r; else rosters[number_rosters]=r;
	p=r;
        break;
	}
      cp++;
      } /* while finding characters */
    head = cp+1;
    if (*head == '\0') break; 
    }
  number_rosters++;
  }
return(1);
}
  


int hash2[HASHSIZE]; 
char hasht2[HASHSIZE][24];
int est=0;

void hash2_init() {
int i;
for (i=0;i<HASHSIZE;i++) { 
  hash2[i]=-1;
  hasht2[i][0]='\0';
  }

}


int hash2_find(char *x) { /* returns 0 if not found */
int p;
int i;
int ch;
p=0;
for (i=0;ch=x[i];i++) {
  p=(p*256+ch) % HASHSIZE; 
  if (p<0) p=p+HASHSIZE;
  }
i=(p+HASHSIZE-1)% HASHSIZE ; /* previous one */ 
while (p != i) {
  if (hasht2[p][0]==0) return(0);
  if (strcmp(hasht2[p],x)==0) {
    return hash2[p];
    }
  p = (p+1) % HASHSIZE; 
  }
return(0);
}



int hash2_set(char *x,int d) { 
int p;
int i;
int ch;
p=0;
for (i=0;ch=x[i];i++) {
  p=(p*256+ch) % HASHSIZE; 
  if (p<0) p=p+HASHSIZE;
  }
i=(p+HASHSIZE-1)% HASHSIZE ; /* previous one */ 
while (p != i) {
  if (hasht2[p][0]==0) {
      hash2[p] = d;
      strcpy(hasht2[p],x);
      est++;
      return(0);
      }
  if (strcmp(hasht2[p],x)==0) {
     hash2[p] = d;
     return(0);
     }
  p = (p+1) % HASHSIZE; 
  }
return(-1);
}




int hash2_increment(char *x) { /* returns -1 if not found */
int p;
int i;
int ch;
p=0;
for (i=0;ch=x[i];i++) {
  p=(p*256+ch) % HASHSIZE; 
  if (p<0) p=p+HASHSIZE;
  }
i=(p+HASHSIZE-1)% HASHSIZE ; /* previous one */ 
while (p != i) {
  if (hasht2[p][0]==0) {
      hash2[p] = 1;
      strcpy(hasht2[p],x);
      est++;
      return(0);
      }
  if (strcmp(hasht2[p],x)==0) {
     hash2[p] = hash2[p]+1;
     return(0);
     }
  p = (p+1) % HASHSIZE; 
  }
return(-1);
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
#define ERROR_COUNT 50
float errors[ERROR_COUNT];
int counter;
FILE *trainfile;

  
roster=fopen("companies.dat","r");
number_companies=0;
while (read_companies(roster));
fclose(roster);

/* gather the roster */
roster=fopen("roster.dat","r");
while (read_roster(roster));
fclose(roster);

if (argc!=2) {
  FILE *itfile;
  fprintf(stderr,"Reading file instead\n");
  itfile=fopen("it.bdf","r");
  fread((void *)hash2,sizeof(float),HASHSIZE,itfile);    
  fread((void *)hasht2,sizeof(char)*24,HASHSIZE,itfile);    
  fclose(itfile);
  }
else {
  FILE *itfile;

trainfile = fopen(argv[1],"r");
if (!trainfile) {
  fprintf(stderr,"cannot open training float normalized file %s\n",argv[1]);
  exit(-1);
  }
  
current = malloc(sizeof(float)*number_companies);
previous = malloc(sizeof(float)*number_companies);
fprintf(stderr,"train read...\n");
counter = 0;
fread((void *)current,sizeof(float),number_companies,trainfile); /* read initial values line and ignore it */
while (fread((void *)current,sizeof(float),number_companies,trainfile)) {
  int i;
  int j;
  if (counter %1000 ==0) {
    fprintf(stderr,"Writing...\n");
    itfile=fopen("it.bdf","w");
    fwrite((void *)hash2,sizeof(float),HASHSIZE,itfile);    
    fwrite((void *)hasht2,sizeof(char)*24,HASHSIZE,itfile);    
    fclose(itfile);
    }
  
  if (counter) {
    for (i=0;i<number_companies;i++) {
      char buf[200];
      hash2_increment(companies[i]);
      sprintf(buf,"%s_%d",companies[i],code(current[i]));
      hash2_increment(buf);
      }
    
    for (j=0;j<number_rosters;j++) {
      struct roster *r;
      r = rosters[j];
      while (r) {
        struct roster *s;
        s = r;
        while (s) {
          char buf[200];
  	  sprintf(buf,"%s_%d_%s_%d",r->name,code(previous[r->id]),
	                          s->name,code(previous[s->id]));
          hash2_increment(buf);			  
	  s = s->next;
	  }
        r = r->next;
        } /* while we are going through rosters */
      } /* for each set of rosters */
      
    /* now do the big ones */    
    for (j=0;j<number_rosters;j++) {
      struct roster *r;
      r = rosters[j];
      while (r) {
        struct roster *s;
        s = r;
        while (s) {
	  struct roster *t;
	  t = rosters[j];
	  while (t) {
            char buf[200];
  	    sprintf(buf,"%s_%d_%s_%d|%s_%d",r->name,code(previous[r->id]),
	                          s->name,code(previous[s->id]),t->name,code(current[t->id]));
            hash2_increment(buf);			  
	    t=t->next;
	    } /* for each current value that we care about */
	  s = s->next;
	  }
        r = r->next;
        } /* while we are going through rosters */
      } /* for each set of rosters */
        
    } /* if we are able to show appl_1_intel_4|AAPL_1 */
  
  
  
  {float *t;
  t=current;
  current=previous;
  previous=t;
  }
  counter++;
  fprintf(stderr,"%d %f full\n",counter,(float)((float)est / (float)(HASHSIZE)  * 100.));
  }

  fprintf(stderr,"Writing it out\n");
  itfile=fopen("it.bdf","w");
  fwrite((void *)hash2,sizeof(float),HASHSIZE,itfile);    
  fwrite((void *)hasht2,sizeof(char)*24,HASHSIZE,itfile);    
  fclose(itfile);
  }


/* now to read standard input */
fprintf(stderr,"reading standard input...");
counter = 0;
current = malloc(sizeof(float)*number_companies);
previous = malloc(sizeof(float)*number_companies);
fread((void *)current,sizeof(float),number_companies,stdin); /* read initial values line and ignore it */
while (fread((void *)current,sizeof(float),number_companies,stdin)) {
  int i;
  int j;
  if (counter) {
    /* now do the big ones */    
    for (j=0;j<number_rosters;j++) {
      struct roster *r;
      r = rosters[j];
      while (r) {
        fprintf(stderr,"%d %s	",counter,r->name);
	/* we are going to evaluate this record */
	for (i=0;i<=8;i++) {
	  char look[50];
	  struct roster *s;
	  double ratio_product;
	  double onemratio_product;
	  int flag;
	  
	  sprintf(look,"%s_%d",r->name,i);
	  if (!hash2_find(look)) {
	    continue; /* skip the anomaly */
	    }
	  flag=0;
	  
	  ratio_product = 1.;
	  onemratio_product = 1.;
	  
	  s = rosters[j];
          while (s) {
	    struct roster *t;
	    t = s;
	    while (t) {
              char buf[200];
	      int frequency_count;
	      int total_count;
  	      sprintf(buf,"%s_%d_%s_%d|%s_%d",s->name,code(previous[s->id]),
	                          t->name,code(previous[t->id]),r->name,i);
              frequency_count = hash2_find(buf);
	      if (frequency_count) {
  	        double ratio;
		double onemratio;
		sprintf(buf,"%s_%d_%s_%d",s->name,code(previous[s->id]),
	                          t->name,code(previous[t->id]));
	        total_count = hash2_find(buf);
		if (total_count < frequency_count) total_count = frequency_count; // sanity
		
		/* all this baysean crap converts to this */
		ratio = (double)(frequency_count) / (double)(total_count);
		onemratio = 1.0 - ratio;
		if (onemratio <=0.0) onemratio=0.001;
		
		ratio_product = ratio_product * (ratio);
		if (isnan(log(onemratio))) {
		  fprintf(stderr,"isnan caught for %lf\n",onemratio);
		  }
		onemratio_product = onemratio_product * onemratio;
		if (isnan(onemratio_product)) {
		  fprintf(stderr,"onemratio_product screwed\n");
		  onemratio_product=1.;
		  }
		if (isnan(ratio_product)) {
		  fprintf(stderr,"mratio_product screwed\n");
		  ratio_product=1.;
		  }
		flag=1;
		} /* if we found a WORD that was used before */
	      t=t->next;
	      } /* while each s, t combo */
	    s = s->next;
	    } /* while we are going through every possible word */
	  
	  if (flag) {	  
	    fprintf(stderr,"%d %d%c	",i, (int)( 100. * (ratio_product) / ((ratio_product) + (onemratio_product)) ) ,'%');
	    }
	  else {
	    fprintf(stderr,"%d 0%c	",i, '%' );
	    }
	  
	  } /* for each possible value that a current word could be */
	fprintf(stderr, "-> %d\n",code(current[r->id]));
        r = r->next;
        } /* while we are going through rosters */
      } /* for each set of rosters */
        
    } /* if we are able to show appl_1_intel_4|AAPL_1 */
  counter = counter +1;
  } /* while reading from standard input */  
    
exit(0);
}



