/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern int readock(int,int);
extern int getzrxinit();
extern void report(int,char *);

extern int Lastrx;

int getnak()
{
   static int firstch;

   Lastrx = 0;
   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      firstch = readock(800,1);
      if (firstch==ZPAD) {
         if (getzrxinit()) {
            return NERROR;
         }
         break;
      } else if (firstch==TIMEOUT) {
         zperr("Timeout on PName",TRUE);
         return TRUE;
      } else if (firstch==WANTCRC) {
         Crcflag = TRUE;
         break;
      } else if (firstch==NAK) {
         break;
      } else if (firstch==CAN) {
         if ((firstch = readock(20,1)) == CAN && Lastrx == CAN) {
            return TRUE;
         }
      }
      Lastrx = firstch;
   }
   report(BLKCHECK,Crcflag?"CRC":"Checksum");
   return FALSE;
}

/************************** END OF MODULE 7 *********************************/

;	      /* Spacing between zcrcq requests */
unsigned Txwcnt;	      /*