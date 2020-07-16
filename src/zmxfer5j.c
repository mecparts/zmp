/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern void report(int,char *);

extern int Firstsec;

/* Report CRC mode in use, but only if first sector */
void crcrept(flag)
int flag;
{
   if (Firstsec)
      report(BLKCHECK, flag ? "CRC-32" : "CRC-16");
   Firstsec = FALSE;   /* clear the flag */
}

/************************** END OF MODULE 5 *********************************/
