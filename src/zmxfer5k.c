/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"

extern void zperr(char *,int);

extern int Fd;
extern char *Rxptr;
extern char *Cpmbuf;
extern unsigned Cpindex;
extern unsigned Cpbufsize;

/* Add a block to the main buffer pointer and write to disk if full */
/* or if flag set */
int putsec(count,flag)
int count, flag;
{
   short status;
   unsigned size;

   status = 0;
   Rxptr += count;
   Cpindex += count;
   if ((Cpindex >= Cpbufsize) || flag)
   {
      size = (Cpindex > Cpbufsize) ? Cpbufsize : Cpindex;

      status = write(Fd,Cpmbuf,size);
      if (status != size)
      {
         zperr("Disk write error",TRUE);
         status = NERROR;
      }
      Cpindex = 0;
      Rxptr = Cpmbuf;
   }
   return status;
}

/************************** END OF MODULE 5 *********************************/
