/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <unixio.h>

extern int Fd;
extern char *Cpmbuf;
extern unsigned Cpbufsize;
extern unsigned Cpindex;
extern int Incnt;

int newload(buf, count)
int count;
char *buf;
{
   static int j;

   j = 0;
   while (count--) {
      if (Incnt <= 0) {
         Incnt = read( Fd, Cpmbuf, Cpbufsize );
         Cpindex = 0;      
         if (Incnt <= 0) {
            break;
         }
      }
      buf[j++] = Cpmbuf[Cpindex++];
      --Incnt;
   }
   return (j ? j : -1);
}

/************************** END OF MODULE 7 *********************************/
