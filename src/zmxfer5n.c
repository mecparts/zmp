/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"

extern void zperr(char *,int);
extern int roundup(int,int);

extern unsigned Cpindex;
extern int Fd;
extern char *Cpmbuf;
extern char *Rxptr;

/*
 * Close the receive dataset, return OK or NERROR
 */
int closeit()
{  
   static int status;
   int length, padding;

   status = OK;
   if (Fd != -1) {
      if (Cpindex) {
         length = 128*roundup(Cpindex,128);
         /* pad last sector with ^Z if necessary  */
         /* otherwise text files from Linux boxes */
         /* don't end properly                    */
         padding = length - Cpindex;
         while (padding > 0) {
            *Rxptr++ = CTRLZ;
            --padding;
         }

         status = ((write(Fd,Cpmbuf,length) == length) ? OK : NERROR);

         Cpindex = 0;
         Rxptr = Cpmbuf;
      }
      if (status == NERROR) {
         zperr("Disk write error",TRUE);
      }

      if (close(Fd)==NERROR) {
         Fd = -1;

         zperr("File close error",TRUE);
         return NERROR;
      }
   }
   return status;
}

/************************** END OF MODULE 5 *********************************/
