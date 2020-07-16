/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int dreport(int,int);
extern int opabort();
extern void mcharout(char);
extern unsigned updcrc(unsigned,unsigned);
extern int readock(int,int);
extern void zperr(char *,int);

extern int Lastrx;
extern int Rxtimeout;
extern int Firstsec;
extern int Crcflag;
extern int Totsecs;
extern int Noeofseen;

int wcputsec(buf, sectnum, cseclen)
char *buf;
int sectnum;
int cseclen;   /* data length of this sector to send */
{
   static unsigned checksum;
   static char *cp;
   static unsigned oldcrc;
   static int wcj;
   static int firstch;
   static int attempts;

   firstch=0;   /* part of logic to detect CAN CAN */

   dreport(ERRORS,0);
   for (attempts=0; attempts <= RETRYMAX; attempts++) {
      if (opabort()) {
         return NERROR;
      }
      if (attempts) {
         dreport(ERRORS,attempts);
      }
      Lastrx= firstch;
      mcharout(cseclen==KSIZE?STX:SOH);
      mcharout(sectnum);
      mcharout(0xFF & ~sectnum);
      oldcrc=checksum=0;
      for (wcj=cseclen,cp=buf; --wcj>=0; ) {
         mcharout(*cp);
         oldcrc=updcrc((0377& *cp), oldcrc);
         checksum += *cp++;
      }
      if (Crcflag) {
         oldcrc=updcrc(0,updcrc(0,oldcrc));
         mcharout((int)oldcrc>>8);
         mcharout((int)oldcrc);
      } else {
         mcharout(checksum);
      }

      firstch = readock(Rxtimeout, (Noeofseen&&sectnum) ? 2:1);
gotnak:
      switch (firstch) {
      case CAN:
         if(Lastrx == CAN) {
cancan:
            zperr("Rcvr CANcelled",TRUE);  
            return NERROR;
         }
         break;
      case TIMEOUT:
         zperr("Timeout on ACK",TRUE); 
         continue;
      case WANTCRC:
         if (Firstsec) {
            Crcflag = TRUE;
         }
         report(BLKCHECK,Crcflag?"CRC":"Checksum");
      case NAK:
         zperr("NAK on sector",TRUE); 
         continue;
      case ACK: 
         Firstsec=FALSE;
         Totsecs += (cseclen>>7);
         return OK;
      case NERROR:
         zperr("Got burst",TRUE); 
         break;
      default:
         zperr("Bad sector ACK",TRUE);
         break;
      }
      for (;;) {
         if (opabort()) {
            return NERROR;
         }
         Lastrx = firstch;
         if ((firstch = readock(Rxtimeout, 2)) == TIMEOUT) {
            break;
         }
         if (firstch == NAK || firstch == WANTCRC) {
            goto gotnak;
         }
         if (firstch == CAN && Lastrx == CAN) {
            goto cancan;
         }
      }
   }
   zperr("Retry Exceeded",TRUE);
   return NERROR;
}

/************************** END OF MODULE 7 *********************************/
