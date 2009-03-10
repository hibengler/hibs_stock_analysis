/* NNL
This tries an NNL on the stocks and tries to predict what will come next.
*/
      
#include "gen.h"
#include "lwneuralnet.h"      
#include "ai_constant.h"	    

char *companies[10000];
int number_companies;
float *input;
float *output;
float *target;




int make_nnl(network_t **resultnet)
{
int number_spots;
network_t *net;
int number_input;
int number_output;
int number_base;

number_input = (number_companies*run_offset_length*2);
number_output = number_companies*train_offset_length*3;

number_base = (number_input+number_output);
number_spots = (number_input+number_output);

/* create the neural network; */
net = net_allocate (3,number_input,number_base,
 number_output);

/*net_use_bias(net,0);*/
*resultnet = net;
return(0);
}


int read_companies(char *file) {
FILE *pf;
char line[20000];
number_companies=0;

pf = fopen(file,"r");
if (!pf) {
  fprintf(stderr,"error cant find file %s\n");
  exit(-1);
  }
while (fgets(line,19999,pf) != NULL) {
  int position;
  struct company *c;
  line[strlen(line)-1] ='\0'; /* clip the \n off */
  if (!(*line)) continue;
  companies[number_companies++] = strdup(line);
  }
return(0);
}
  


int make_buffers()
{
int number_spots;
int number_input;
int number_output;

number_input = (2*number_companies*run_offset_length)
 + (number_companies*iterations_forward*2);
number_output = 3*number_companies*train_offset_length;
 + (number_companies*iterations_forward*3);
/* input and output are bigger so we can keep feeding the loop */

input = malloc(sizeof(float)*number_input);
target = malloc(sizeof(float)*number_input);
output =malloc(sizeof(float)*number_output);

return(0);
}



                            
                            
                            
                            
                            
                     

			    
							                            
                            
int grow_nnl(network_t **resultnet)
{
network_t *oldnet;
network_t *newnet;
oldnet = *resultnet;

/*newnet = net_spawn(oldnet,0.25);*/
newnet = net_copy(oldnet);
net_jolt(newnet,.1,.1);
/*net_add_neurons(newnet,1,-1,100);
*/
net_free(oldnet);

*resultnet = newnet;
}
                            
                            

			    
							
										    
														
																	    
double train_total;
int train_count;
																								    
int process_nnl(network_t **resultnet,int output_flag,int mode)
{
network_t *net;
net = *resultnet;
FILE *pf;
char line[100000];

int number_input;
train_total=0.;
train_count=0;
pf=stdin;
while (fgets(line,99999,pf) != NULL) {
  process_nnl_line(net,&output_flag,line,mode);
  }  /* while procesing a line */
if ((train_count)&&(output_flag)) {
  printf("average_acc %lf",(train_total/((double)train_count)));
  }
return(0);
}

  
      

int process_nnl_line(network_t *net,int *poutput_flag,char *line,int mode) 
{

char *rest;
char *x;
if (!(x=strtok_r(line," ",&rest))) {
  fprintf(stderr,"nnl line is empty\n");
  return(-1);
  }
if (strcmp(x,"run")==0) {
  return(0);
  if(run_me(net,rest)) return(-1);
  extend_me(net);
  if (*poutput_flag) {print_guess(net,rest,"guess");}
  }
else if (strcmp(x,"train") ==0) {
  x=strtok_r(NULL,"\t",&rest);
  if (run_me(net,x)) return(-1);
  net_compute(net, input, output);
  if (train_me(net,rest,mode)) {
    return(-1);
    }
  if (*poutput_flag) {
  print_guess(net,rest,"report");
  printf("rptorg %s\n",x);
  print_target(net);
  print_diff();
    }    
  } /* if we are training*/
else if (strcmp(x,"output") ==0) {
  x=rest;
  *poutput_flag=atoi(x);
  }
else if (strcmp(x,"learning_rate") ==0) {
  x=rest;
  net_set_learning_rate(net,atof(x));
  fprintf(stderr,"lr is now %f\n",atof(x));
  }
else if (strcmp(x,"momentum") ==0) {
  x=rest;
  net_set_momentum(net,atof(x));
  }
  
return(0);
}




int run_me(network_t *net,char *line) {
char *rest;
char *x;
char line_copy[100000];
int i;
int inputpos;
int cnum;
float day;
float time_of_day;

inputpos=0;
cnum=0;
strcpy(line_copy,line);

if (!(x=strtok_r(line_copy,",",&rest))) {
  fprintf(stderr,"run missing time in seconds\n");
  return(-1);
  }
/* time in seconds. ignore */
  
for (i=0;i<number_companies;i++) {
  int j;
  if (!(x=strtok_r(NULL,",",&rest))) {
    fprintf(stderr,"run missing company name for %s",companies[cnum]);
    return(-1);
    }
  /* company name */ 
  if (strcmp(x,companies[cnum]) != 0) {
    fprintf(stderr,"run error parsing: %s not equal to %s\n",x,companies[cnum]);
    exit(-1);
    }
  for (j=0;j<run_offset_length;j++) {
   /* we make it like this
      c1p1 c1v1 c2p1 c2v1 c3p1 c3v1 */
    if (!(x=strtok_r(NULL,",",&rest))) {
      fprintf(stderr,"run missing price parameter %d of company %s\n",j+1,
         companies[cnum]);
      return(-1);
      }
    /*data point of past values for company */
    input[inputpos*number_companies*2+cnum+cnum] = atof(x) ;
    
    if (!(x=strtok_r(NULL,",",&rest))) {
      fprintf(stderr,"run missing volume parameter %d of company %s\n",j+1,
         companies[cnum]);
      return(-1);
      }
    /*data point of past values for company */
    input[inputpos*number_companies*2+cnum+cnum+1] = atof(x) ;
    inputpos++;
    }
  cnum++;
  }  

return(0);
}


/* This is a cute one
If we add the output to the input we can 
extrapolate further into the future.
It could extrapolate all the way so that there is no more
real input! wow!
*/
int extend_me(network_t *net) {
int cnt;
int i;
float *in,*out;
in=input;
out=output;
for (cnt=0;cnt<200;cnt++) {
  net_compute(net, in, out);
  for (i=0;i<number_companies;i++) {
    in[number_companies*run_offset_length*2+i+i] = out[i];
    in[number_companies*run_offset_length*2+i+i+1] = out[i+1];
    in[number_companies*run_offset_length*2+i+i+2] = out[i+2];
    }
  in= in + (number_companies*2);
  out = out + (number_companies*3);
  }
return(0);
}


int print_guess (network_t *net,char *line,char *command) 
{
/* print out the results */    
int outputpos;
int i;
printf("%s %s",command,line);
printf("guess_ ");
outputpos=0;


for (i=0;i<number_companies;i++) {
  int j;
  printf("%s,",companies[i]);
  for (j=0;j<train_offset_length;j++) {
    printf("%5.2f,",output[outputpos++] ); /* output point */
    printf("%8.0f,",output[outputpos++] ); /* output point */
    printf("%5.2f,",output[outputpos++]*100. ); /* output point */
    }
  }
printf("\t %5.2lf\n",output[outputpos]);

}


int print_target (network_t *net) 
{
/* print out the results */    
int targetpos;
int i;
printf("tuess_ ");
targetpos=0;


for (i=0;i<number_companies;i++) {
  int j;
  printf("%s,",companies[i]);
  for (j=0;j<train_offset_length;j++) {
    printf("%5.2f,",target[targetpos++]); /* output point */
    printf("%8.0f,",target[targetpos++]); /* output point */
    printf("%5.2f,",target[targetpos++]*100.); /* output point */
    }
  }
printf("\n");

}


double diff_value();






int train_me(network_t *net,char *line,int mode) {
char line_copy[100000];
int i;
int trainpos;
float day;
float time_of_day;
char *x;
char *rest;
int cnum;
char *start;
  double companyact;
  double companytot;

trainpos=0;
cnum=0;
strcpy(line_copy,line);

companyact=0.;
companytot=0.;

  
start=line_copy;  
for (i=0;i<number_companies;i++) {
  int j;
  if (!(x=strtok_r(start,",",&rest))) {
    fprintf(stderr,"train missing company name for %s",companies[cnum]);
    return(-1);
    }
  start=NULL; /* for strtok */
  /* company name */ 
  if (strcmp(x,companies[cnum]) != 0) {
    fprintf(stderr,"train error parsing: %s not equal to %s\n",x,companies[cnum]);
    return(-1);
    }
    
  for (j=0;j<train_offset_length;j++) {
    if (!(x=strtok_r(NULL,",",&rest))) {
      fprintf(stderr,"train missing parameter %d of company %s\n",j+1,
         companies[cnum]);
      return(-1);
      }
    /*data point of past values for company */
    target[trainpos++] = atof(x)  ;
    
    if (!(x=strtok_r(NULL,",",&rest))) {
      fprintf(stderr,"train missing parameter %d of company %s\n",j+1,
         companies[cnum]);
      return(-1);
      }
    /*data point of past values for company */
    target[trainpos++] = atof(x)  ;
    
    companyact += target[trainpos] = diff_value(trainpos-2);
    companytot += 1.;
    trainpos++;
    }
  cnum++;
  }  

if (mode !=3) {
  net_compute_output_error(net, target);
  net_train(net);
  }
return(0);
}


 
  
double diff_value(int index) {
double diff;
double divisor;
diff=fabs(target[index]-output[index]);
if (diff==0.) return(0.);
divisor = fabs(target[index]);

if (divisor==0.) return(0.);
diff = diff/divisor;
if (diff>1.) diff=1.;
diff=(1.0-diff);

return((double)(diff));
}
    
    
    
double print_diff_value(int index) {
double diff;
diff = diff_value(index);
printf("%d ",
  (int)(diff*100.));
  

return(diff);
}
    

int print_diff()
{
int pos;
double relact;
double totact;
int i;
relact=0.;
totact=0.;
/* print out the results */    
pos=0;

printf("\naccuracy ");
for (i=0;i<number_companies;i++) {
  double companyact;
  double companytot;
  double x;
  int j;
  x=50.;
  companyact=0.;
  companytot=0.;
  printf("%s,",companies[i]);
  for (j=0;j<train_offset_length;j++) {
    double y;
    y=print_diff_value(pos++);
    companyact += y * x;
    companytot += x;
    x = x * 0.95;
    pos++; /* skip volume */
    printf("<%d>,",(int)(target[pos++]*100.));
    }
  printf("acc,%d,",(int)(companyact*100./companytot));
  relact += companyact;
  totact += companytot;
  }
printf("accall\t,%5.2lf,",relact*100./totact);

train_total += relact*100./totact;
train_count++;
printf("\n");
}


    


																															    																												                            
main(int argc,char *argv[]) {
/* argument 1 - file name for the list of companies
argument 2 - mode
             1 = new
	     2 = grow
	     3 = run only
	     4 = run/train
	     
argument 3 - file name for nnl to write to
argument 4 - file name for nnl to read

*/
int i,j,k,l;
int flag;
network_t *net;
int mode;

fprintf(stderr,"read...\n");
read_companies(argv[1]);
fprintf(stderr,"done reading...\n");


mode = atoi(argv[2]);
if ((mode==2)||(mode==3)||(mode==4)) {
  fprintf(stderr,"reading  %s nnl...\n",argv[4]);
  net = net_load(argv[4]);
  }

if ((mode==2)) {
  grow_nnl(&net);
  }
else if (mode==1) {
  make_nnl(&net);
  }
make_buffers();
process_nnl(&net,1,mode);

if (mode !=3) net_save(argv[3],net);


exit(0);
}
