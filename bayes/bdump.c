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

int number_companies;

int dump_bayes(int number_of_companies)
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
  double point_offsets[10] = {1.,2.,4.,7.,12.,20.,33.,54.,88.,143.};
  int fileno;
  positions=9;
  points=3;
  fileno=0;
  FILE *xf;
  char xfilename[1000];
  int bandpos;

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

  fprintf(stdout,"output band size %d\n",output_band_size);
  fprintf(stdout,"output size %d\n",outputsize);
  fprintf(stdout,"input size %d\n",inputsize);
  fprintf(stdout,"Sample size %d\n",sample_size);

  sum_inputs = calloc(sizeof(short),inputsize);
  input = calloc(sizeof(short),inputsize);
  output = calloc(sizeof(short),outputsize);
  predict = calloc(sizeof(double),outputsize);
  sum_outputs = calloc(sizeof(short),outputsize);

                                 /* integer with the input band size*/
  fread(sum_outputs,sizeof(short),outputsize,xf);
                                 /* integer with the input band size*/
  fread(sum_inputs,sizeof(short),inputsize,xf);


                                 /* only one line at a time in this version */
  sum_all = (short *)calloc(sizeof(short),inputsize);

  if (!sum_all) {fprintf(stderr,"No memory!\n"); exit(-1);}
  pclose(xf);                    /* close this out,  we will read it again */

  for (i=0;i<number_of_companies;i+=output_band_size)
  {
                                 /* if we have another band to read */
    {
      int junk;
      fprintf(stdout,"band %d\n",fileno);
      sprintf(xfilename,"gunzip <output%3.3d.dat.gz",fileno);
      xf = popen(xfilename,"r");
      if (!xf)
      {
        fprintf(stderr,"could not open band %d\n",fileno);
        exit(-1);
      }
      fprintf(stderr,"groovin %d %s\n",fileno,xfilename);
      fread(&junk,4,1,xf);       /* file number*/
      fread(&junk,4,1,xf);       /* integer with the band number*/
      fread(&junk,4,1,xf);       /* integer with the output band size */
      fread(&junk,4,1,xf);       /* integer with the output band size */
      fread(&junk,4,1,xf);       /* integer with the input band size*/
      fread(&junk,4,1,xf);       /* integer with the input band size*/

                                 /* integer with the input band size*/
      fread(sum_outputs,sizeof(short),outputsize,xf);
                                 /* integer with the input band size*/
      fread(sum_inputs,sizeof(short),inputsize,xf);
      /* ready to continue reading the output probability bands */
    }                            /* if we had to open the next band */


  {int i;
  for (i=0;i<inputsize;i++)
  {
    fprintf(stderr,"i%d  %d\n",
    i,
    sum_inputs[i]);
  }
  }

  {int i;
  for (i=0;i<outputsize;i++)
  {
    fprintf(stderr,"%d %d %d\n",
    fileno*output_band_size+(i/positions),i%positions,
    sum_outputs[i]);
  }
  }
  
    for (k=i;(k-i<output_band_size)&&(k<number_of_companies);k++)
    {
      for (l=0;l<positions;l++)
      {
        int y;
        /*	  fprintf(stdout,"%d\n",k);*/
        y=fread(sum_all,sizeof(short),inputsize,xf);
        if (y<inputsize)
        {
          fprintf(stdout,"Error band %d line %d y %d\n",i,k,y);
        }
      }                          /* for each output line in a band */
    }
    pclose(xf);
    fileno++;
  }                              /* band block */
}


main(int argc, char *argv)
{

  number_companies=3152;
  dump_bayes(number_companies);

  exit(0);

}
