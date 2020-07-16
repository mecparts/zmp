/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern void clrline(int);
extern void report(int,char *);
extern void dreport(int,int);

extern int Errors;

void zperr(string,incrflag)
char *string;
int incrflag;
{
   clrline(MESSAGE);
   report(MESSAGE,string);
   if (incrflag) {
      dreport(ERRORS,++Errors);
   }
}

/************************** END OF MODULE 5 *********************************/
