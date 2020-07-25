/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void report(int,char *);
extern int opabort();
extern void mcharout(char);
extern int wcgetsec(char *,int);
extern int putsec(int,int);
extern void sreport(int,long);
extern void zperr(char *,int);
extern int closeit();
extern void setmodtime();

extern int Firstsec;
extern int Crcflag;
extern char *Rxptr;
extern int Wcsmask;

/*
 * Adapted from CMODEM13.C, written by
 * Jack M. Wierda and Roderick W. Hart
 */

int wcrx()
{
   static int sectnum, sectcurr;
   static char sendchar;
   static int cblklen;         /* bytes to dump this block */
   long charsgot;

   Firstsec=TRUE;
   sectnum=0; 
   charsgot = 0L;
   sendchar=Crcflag?WANTCRC:NAK;
   report(BLKCHECK,Crcflag?"CRC":"Checksum");

   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      mcharout(sendchar);               /* send it now, we're ready! */
      sectcurr = wcgetsec(Rxptr,Firstsec||(sectnum&0177)?50:130);
      if (sectcurr==(sectnum+1 &Wcsmask)) {
         charsgot += Blklen;
         sreport(++sectnum,charsgot);
         cblklen = Blklen;
         if (putsec(cblklen,FALSE)==NERROR) {
            return NERROR;
         }
         sendchar=ACK;
      } else if (sectcurr==(sectnum&Wcsmask)) {
         zperr("Duplicate Sector",TRUE);
         sendchar=ACK;
      } else if (sectcurr==WCEOT) {
         if (closeit()) {
            return NERROR;
         } else {
            setmodtime();
         }
         mcharout(ACK);
         return OK;
      } else if (sectcurr==NERROR) {
         return NERROR;
      } else {
         zperr( "Sync Error",TRUE);
         return NERROR;
      }
   }
}

/************************** END OF MODULE 8 *********************************/
