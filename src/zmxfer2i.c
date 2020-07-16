/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern int newload(char *,int);

/* fill buf with count chars padding with ^Z for CPM */

int filbuf(buf, count)
char *buf;
int count;
{
   static int c, m;

   c = m = newload(buf, count);
   if (m <= 0) {
      return 0;
   }
   while (m < count) {
      buf[m++] = CTRLZ;
   }
   return c;
}

/************************** END OF MODULE 7 *********************************/
