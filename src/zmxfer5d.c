/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern void dreport(int,int);
extern void lreport(int,long);

void sreport(sct,bytes)
int sct;
long bytes;
{  
   dreport(BLOCKS,sct);
   lreport(KBYTES,bytes);
}

/************************** END OF MODULE 5 *********************************/
