/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>

extern void clrline(int);

clrreports()
{
   static int i;

   for (i=4; i<13; i++)
      clrline(i);
}
/************************** END OF MODULE 8 *********************************/
