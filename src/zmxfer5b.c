/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>

extern void report(int,char *);
extern char *itoa(int,char *);

void dreport(row,value)
int row, value;
{
   static char buf[7];

   report(row,itoa(value,buf));
}

/************************** END OF MODULE 5 *********************************/
