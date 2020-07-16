/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void purgeline();
extern void mcharout(char);
extern int wcgetsec(char *,int);
extern int readline(int);

extern int Firstsec;
/*
 * Fetch a pathname from the other end as a C ctyle ASCIZ string.
 * Length is indeterminate as long as less than Blklen
 * A null string represents no more files (YMODEM)
 */

int wcrxpn(rpn)
char *rpn;   /* receive a pathname */
{
   static int c;

   purgeline();

et_tu:
   Firstsec=TRUE;  
   mcharout(Crcflag?WANTCRC:NAK);
   while ((c = wcgetsec(rpn, 100)) != 0) {
      if (QuitFlag)
         return NERROR;
      if (c == WCEOT) {
         mcharout(ACK);
         readline(INTRATIME);
         goto et_tu;
      }
      return NERROR;
   }
   mcharout(ACK);
   return OK;
}

/************************** END OF MODULE 8 *********************************/
