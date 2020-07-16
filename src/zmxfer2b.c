/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void zperr(char *,int);
extern void wait(int);
extern void fstat(char *,struct stat *);
extern int wctxpn(char *);
extern void canit();
extern int wctx(long);

extern int Fd;
extern int Noeofseen;
extern long Lastread;
extern int Lastn;
extern int Dontread;
extern int Filcnt;
extern struct stat Fs;

int wcs(oname)
char *oname;
{
   unsigned length;
   long flen;

   if ((Fd=open(oname,0))==UBIOT) {
      zperr("Can't open file",TRUE);
      wait(2);
      return OK;                /* pass over it, there may be others */
   }
   ++Noeofseen;  
   Lastread = 0L;  
   Lastn = -1; 
   Dontread = FALSE;
   ++Filcnt;
   fstat(oname,&Fs);
   switch (wctxpn(oname)) {     /* transmit path name */
      case NERROR:
         if (Zmodem) {
            canit();            /* Send CAN */
         }
         return NERROR;

      case ZSKIP:
         return OK;
   }
   length = Fs.records;
   flen = (long)length * 128;
   if (!Zmodem && wctx(flen)==NERROR)
      return NERROR;
   return 0;
}

/************************** END OF MODULE 7 *********************************/
