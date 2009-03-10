#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash[40001];
char hasht[40001][12];


void hash_init() {
int i;
for (i=0;i<40001;i++) {
  hash[i]=-1;
  hasht[i][0]='\0';
  }

}


int hash_find(char *x) { /* returns -1 if not found */
int p;
int i;
int ch;
p=0;
for (i=0;ch=x[i];i++) {
  p=(p*256+ch) % 40001;
  }
i=(p+40000)% 40001 ; /* previous one */
while (p != i) {
  if (hasht[p][0]==0) return(-1);
  if (strcmp(hasht[p],x)==0) {
    return hash[p];
    }
  p = (p+1) % 40001;
  }
return(-1);
}



int hash_set(char *x,int d) { /* returns -1 if not found */
int p;
int i;
int ch;
p=0;
for (i=0;ch=x[i];i++) {
  p=(p*256+ch) % 40001;
  }
i=(p+40000)% 40001 ; /* previous one */
while (p != i) {
  if (hasht[p][0]==0) {
      hash[p] = d;
      strcpy(hasht[p],x);
      return(0);
      }
  if (strcmp(hasht[p],x)==0) {
     hash[p] = d;
     return(0);
     }
  p = (p+1) % 40001;
  }
return(-1);
}



