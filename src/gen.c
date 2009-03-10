#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifdef __APPLE__

size_t getline(char **lineptr, size_t *n, FILE *stream) 
{
char *l;
int count;
int ch;
count=0;
l = malloc(1024);
if (!l) return(-1);
while ((ch=fgetc(stream)) != EOF) {
  if (count & 1023 == 1022) {
    char *y;
    y=malloc(count+2+1024);
    if (!y) return ((size_t)-1);
    strncpy(y,l,count);
    free(l);
    l = y;
    }
  l[count++] = ch;
  if (ch=='\n') break;
  }

l[count]='\0';
if (count==0) return((size_t)-1);
*lineptr=l;
return ((size_t)count);
}
#endif




                         
double  str_to_date(char *x) {
/* number of seconds since jan 1, 1970 */
struct tm datime;
time_t time2;
if (!strptime(x,"%Y-%m-%d %H:%M:%S",&datime)) {
  fprintf(stderr,"error stripping time\n");
  }
time2 = mktime(&datime);

return((double)(time2));
}


int greatest(a,b) {
  if (a>b) return a;
  return b;
  }

int leastest(a,b) {
  if (a<b) return a;
  return b;
  }
  
                         
double absd(double a) {
if (a>=0.0) return (a);
return(-a);
}


float absf(float a) {
if (a>=0.0) return (a);
return(-a);
}


