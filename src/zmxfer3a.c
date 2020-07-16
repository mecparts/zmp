/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern int zgethdr(char *,int);
extern void stohdr(long);
extern void zshhdr(int,char *);
extern int sendzsinit();

extern unsigned Rxbuflen;
extern long Rxpos;
extern char Rxhdr[4];
extern char Txhdr[4];
extern int Txfcs32;
extern int Zctlesc;

/*
 * Get the receiver's init parameters
 */

int getzrxinit()
{
   int n,hdr;
   int Rxflags;

   for (n=10; --n>=0; ) {
      if (opabort()) {
         return NERROR;
      }
      hdr = zgethdr(Rxhdr, 1);
      if (hdr==ZCHALLENGE) {   /* Echo receiver's challenge numbr */
         stohdr(Rxpos);
         zshhdr(ZACK, Txhdr);
         continue;
      } else if (hdr==ZCOMMAND) {      /* They didn't see out ZRQINIT */
         stohdr(0L);
         zshhdr(ZRQINIT, Txhdr);
         continue;
      } else if (hdr==ZRINIT) {
         Rxflags = 0377 & Rxhdr[ZF0];
         Txfcs32 = (Wantfcs32 && (Rxflags & CANFC32));
         Zctlesc |= Rxflags & TESCCTL;
         Rxbuflen = (0377 & Rxhdr[ZP0])+((0377 & Rxhdr[ZP1])<<8);
         return (sendzsinit());
      } else if (hdr==ZCAN || hdr==TIMEOUT) {
         return NERROR;
      } else if (hdr==ZRQINIT) {
         if (Rxhdr[ZF0] == ZCOMMAND) {
            continue;
         }
      } else {
         zshhdr(ZNAK, Txhdr);
         continue;
      }
   }
   return NERROR;
}
/************************** END OF MODULE 7A *********************************/
