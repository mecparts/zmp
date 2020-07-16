/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern int readline(int);
extern unsigned updcrc(unsigned,unsigned);
extern void zperr(char *,int);

extern int Errors;
extern int Xmodem;
extern int Lastrx;
extern int Firstsec;

/*
 * Wcgetsec fetches a Ward Christensen type sector.
 * Returns sector number encountered or NERROR if valid sector not received,
 * or CAN CAN received
 * or WCEOT if eot sector
 * time is timeout for first char, set to 4 seconds thereafter
 ***************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
 *    (Caller must do that when he is good and ready to get next sector)
 */

int wcgetsec(rxbuf, maxtime)
char *rxbuf;
int maxtime;
{
   static int checksum, wcj, firstch;
   static unsigned oldcrc;
   static char *p;
   static int sectcurr;

   for (Lastrx=Errors=0; Errors < RETRYMAX; ) {  /* errors incr by zperr */
      if (opabort()) {
         return NERROR;
      }
      if ((firstch=readline(maxtime))==STX) {
         Blklen=KSIZE; 
         goto get2;
      }
      if (firstch==SOH) {
         Blklen=SECSIZ;
get2:
         sectcurr=readline(INTRATIME);
         if ((sectcurr+readline(INTRATIME))==0xFF) {
            oldcrc=checksum=0;
            for (p=rxbuf,wcj=Blklen; --wcj>=0; ) {
               if ((firstch=readline(INTRATIME)) < 0) {
                  goto bilge;
               }
               oldcrc=updcrc(firstch, oldcrc);
               checksum += (*p++ = firstch);
            }
            if ((firstch=readline(INTRATIME)) < 0) {
               goto bilge;
            }
            if (Crcflag) {
               oldcrc=updcrc(firstch, oldcrc);
               if ((firstch=readline(INTRATIME)) < 0) {
                  goto bilge;
               }
               oldcrc=updcrc(firstch, oldcrc);
               if (oldcrc & 0xFFFF) {
                  zperr( "CRC Error",TRUE);
               } else {
                  Firstsec=FALSE;
                  return sectcurr;
               }
            } else if (((checksum-firstch)&0xFF)==0) {
               Firstsec=FALSE;
               return sectcurr;
            } else {
               zperr("Checksum error",TRUE);
            }
         } else {
            zperr("Block nr garbled",TRUE);
         }
      } else if (firstch==EOT && readline(10)==TIMEOUT) {
         /* make sure eot really is eot and not just mixmash */
         return WCEOT;
      } else if (firstch==CAN) {
         if (Lastrx==CAN) {
            zperr( "Sender CANcelled",TRUE);
            return NERROR;
         } else {
            Lastrx=CAN;
            continue;
         }
      } else if (firstch==TIMEOUT) {
         if (Firstsec) {
            zperr( "TIMEOUT",TRUE);
            goto humbug;
         }
bilge:
         zperr( "TIMEOUT",TRUE);
      } else {
         zperr( "Bad header",TRUE);
      }

humbug:
      Lastrx=0;
      while(readline(50) != TIMEOUT)
         if (QuitFlag) {
            return NERROR;
         }
      if (Firstsec) {
         if (Xmodem && (Errors == RETRYMAX/2)) {
            Crcflag = !Crcflag;
         }
         report(BLKCHECK,Crcflag?"CRC":"Checksum");
         mcharout(Crcflag?WANTCRC:NAK);
      } else {
         maxtime=40; 
         mcharout(NAK);
      }
   }
   /* try to stop the bubble machine. */
   canit();
   return NERROR;
}

/************************** END OF MODULE 8 *********************************/
