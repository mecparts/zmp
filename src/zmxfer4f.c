/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern void mstrout(char *,int);
extern void purgeline();

/* send cancel string to get the other end to shut up */
void canit()
{
   static char canistr[] = {
      24,24,24,24,24,24,24,24,24,24,8,8,8,8,8,8,8,8,8,8,0
   };

   mstrout(canistr,FALSE);
   purgeline();
}

/************************** END OF MODULE 8 *********************************/
