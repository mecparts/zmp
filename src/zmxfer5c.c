/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>

extern void report(int,char *);
extern char *ltoa(long,char *);

void lreport(row,value)
int row;
long value;
{
   static char buf[20];

   report(row,ltoa(value,buf));
}

/************************** END OF MODULE 5 *********************************/
