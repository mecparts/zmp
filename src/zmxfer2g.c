/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int readock(int,int);
extern int opabort();
extern int filbuf(char *,int);
extern int wcputsec(char *,int,int);

extern int Firstsec;
extern int Rxtimeout;
extern char *Txbuf;

int wctx(flen)
long flen;
{
   static int thisblklen, i;
   static unsigned sectnum, attempts, firstch;
   static long charssent;

   charssent = 0L;  
   Firstsec = TRUE;  
   thisblklen = Blklen;
   i = 0;

   while ((firstch=readock(1,2)) != NAK 
      && firstch != WANTCRC
      && firstch != CAN
      && !opabort()
      && ++i < Rxtimeout) {
      ;
   }
   if (QuitFlag) {
      return NERROR;
   }
   if (firstch==CAN) {
      zperr("Rcvr CANcelled",TRUE);
      return NERROR;
   }
   if (firstch==WANTCRC) {
      Crcflag=TRUE;
   }
   report(BLKCHECK,Crcflag?"CRC":"Checksum");
   sectnum=0;
   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      if (flen <= (charssent + 896L)) {
         Blklen = thisblklen = 128;
      }
      if ( !filbuf(Txbuf, thisblklen)) {
         break;
      }
      purgeline();   /* ignore anything got while loading */
      if (wcputsec(Txbuf, ++sectnum, thisblklen)==NERROR) {
         return NERROR;
      }
      charssent += thisblklen;
      sreport(sectnum,charssent);
   }
   attempts=0;
   do {
      dreport(ERRORS,attempts);
      purgeline();
      mcharout(EOT);
      ++attempts;
   } while ((firstch=(readock(Rxtimeout, 1)) != ACK) 
      && attempts < RETRYMAX
      && !opabort() );
   if (attempts == RETRYMAX) {
      zperr("No ACK on EOT",TRUE);
      return NERROR;
   } else if (QuitFlag) {  /* from opabort */
      return NERROR;
   } else {
      return OK;
   }
}

/************************** END OF MODULE 7 *********************************/
