#include <stdio.h>

int main() {
FILE *xf;
 char xfilename[1000];
int fileno=0;
     sprintf(xfilename,"gzip >output%3.3d.dat.gz",fileno);
     
     
xf = popen(xfilename,"w");
if (!xf) exit(-1);
fprintf(xf,"hello\n");
pclose(xf);
}

