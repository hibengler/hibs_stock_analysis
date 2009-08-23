#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This used to split up the correlation output into groups
and run the groups
Now it just makes roster.dat which is used to decide what to do
*/
int main () {
char line[10000];
char lastcompany[10000];
char list[10000];
lastcompany[0]='\0';
list[0]='\0';
while (fgets(line,9999,stdin)) {
  char *curline;
  char *buddy;
  if (curline=strtok(line,"\t")) {
    if (buddy=strtok(NULL,"\t")) {
      if (strcmp(curline,lastcompany)==0) {
//        printf("%s\n",buddy);
	strcat(list,buddy);
	strcat(list," ");
	}
      else {
        if (lastcompany[0]) {
/*          printf ("%c  >%s/%s_stocks.dat\n cd %s;step3_launch %s\necho %c%s %s%c >>../roster.dat;cd ..\n\n",
	       '"',lastcompany,lastcompany,
	       lastcompany,lastcompany,'"',lastcompany,list,'"');*/
          printf ("%s\n",
	       list);
	     
	  
	  }
//        printf("rm -rf 2>&1 %s;mkdir %s;echo %c%s\n%s\n",
//        curline,curline,'"',curline,buddy);
        strcpy(lastcompany,curline);
	sprintf(list,"%s %s ",curline,buddy);
	} /* if we are the start of a new segment */
      } /* if we got the values we were looking for */
    } /* if we got one value */
  } /* while here are lines */
    
if (lastcompany[0]) {
//  printf ("%c  >%s/%s_stocks.dat\n cd %s;step3_launch %s\necho %c%s%c >>../roster.dat;cd ..\n\n",
//	       '"',lastcompany,lastcompany,
//	       lastcompany,lastcompany,'"',list,'"');
          printf ("%s\n",
	       list);

	  
  }
exit(0);
}    
	  
	  
	  
	  
