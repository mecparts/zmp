/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <setjmp.h>
#include <cpm.h>

#include "zmp.h"
#include "zmodmdef.h"

extern int getinsync(int);
extern int filbuf(char *,int);
extern int minprdy();

extern jmp_buf jb_stop;      /* buffer for longjmp */
extern int Blklen;
extern int Baudrate;
extern long Lrxpos;
extern int Beenhereb4;
extern unsigned Rxbuflen;
extern int Sending;
extern int Firstsec;
extern long Bytcnt;
extern int Crc32t;
extern long Lastsync;
extern long Lastread;
extern char *Txbuf;
extern long Txpos;
extern int Lastn;
extern int Dontread;
extern char Txhdr[4];

static int Txwindow;
static int Txwspac;
static int Txwcnt;

/* Send the data in the file */

int zsndfdata()
{
   static int c, e, n;
   static int newcnt;
   static long tcount;
   static int junkcount;      /* Counts garbage chars received by TX */
   static int nullv = 0;

   tcount = 0L;
   Blklen = 128;
   if (Baudrate > 30) {
      Blklen = 256;
   }
   if (Baudrate > 120) {
      Blklen = 512;
   }
   if (Baudrate > 240) {
      Blklen = KSIZE;
   }
   if (Rxbuflen && Blklen>Rxbuflen) {
      Blklen = Rxbuflen;
   }
   Lrxpos = 0L;
   junkcount = 0;
   Beenhereb4 = FALSE;
   Sending = Firstsec = TRUE;
somemore:
   if (nullv) {
waitack:
      junkcount = 0;
      c = getinsync(0);
      if (QuitFlag) {
         return NERROR;
      }
gotack:
      if (setjmp(jb_stop)) {   /* come here if rx stops us */
rxint:
         c = getinsync(1);
      }

      switch (c) {
         case ZSKIP:
            return c;
         case ZACK:
         case ZRPOS:
            break;
         case ZRINIT:
            return OK;
         case ZCAN:
         default:
            return NERROR;
     }
     /*
      * If the reverse channel can be tested for data,
      *  this logic may be used to detect error packets
      *  sent by the receiver, in place of setjmp/longjmp
      *  minprdy() returns non 0 if a character is available
      */
      while (minprdy()) {
         if (QuitFlag) {
            return NERROR;
         }
         switch (readline(1)) {
            case CTRLC:
            case CAN:
            case ZPAD:
               goto rxint;
            case XOFF:      /* Wait a while for an XON */
            case XOFF|0200:
               readline(100);
         }
      }
   }

   if (setjmp(jb_stop)) {   /* rx interrupt */
      c = getinsync(1);
      if (c == ZACK) {
         goto gotanother;
      }
      purgeline();
      /* zcrce - dinna wanna starta ping-pong game */
      zsdata(Txbuf, 0, ZCRCE);
      goto gotack;
   }

   newcnt = Rxbuflen;
   Txwcnt = 0;
   stohdr(Txpos);
   zsbhdr(ZDATA, Txhdr);
   do {
      if (QuitFlag) {
         return NERROR;
      }
      if (Dontread) {
         n = Lastn;
      } else {
         n = filbuf(Txbuf, Blklen);
         Lastread = Txpos;  
         Lastn = n;
      }
      Dontread = FALSE;
      if (n < Blklen) {
         e = ZCRCE;
      } else if (junkcount > 3) {
         e = ZCRCW;
      } else if (Bytcnt == Lastsync) {
         e = ZCRCW;
      } else if (Rxbuflen && (newcnt -= n) <= 0) {
         e = ZCRCW;
      } else if (Txwindow && (Txwcnt += n) >= Txwspac) {
         Txwcnt = 0;  
         e = ZCRCQ;
      } else {
         e = ZCRCG;
      }
      zsdata(Txbuf, n, e);
      Txpos += (long)n;
      Bytcnt = Txpos;
      crcrept(Crc32t);   /* praps report crc mode */
      lreport(KBYTES,Bytcnt);
      if (e == ZCRCW) {
         goto waitack;
      }

      /*
       * If the reverse channel can be tested for data,
       *  this logic may be used to detect error packets
       *  sent by the receiver, in place of setjmp/longjmp
       *  minprdy() returns non 0 if a character is available
       */

      while (minprdy()) {
         if (QuitFlag) {
            return NERROR;
         }
         switch (readline(1)) {
            case CAN:
            case CTRLC:
            case ZPAD:
               c = getinsync(1);
               if (c == ZACK) {
                  break;
               }
               purgeline();
      /* zcrce - dinna wanna starta ping-pong game */
               zsdata(Txbuf, 0, ZCRCE);
               goto gotack;
            case XOFF:    /* Wait a while for an XON */
            case XOFF|0200:
               readline(100);
            default:
               ++junkcount;

         }
gotanother:;
      }
      if (Txwindow) {
         while ((tcount = Txpos - Lrxpos) >= Txwindow) {
            if (QuitFlag) {
               return NERROR;
            }
            if (e != ZCRCQ) {
               zsdata(Txbuf, 0, e = ZCRCQ);
            }
            c = getinsync(1);
            if (c != ZACK) {
               purgeline();
               zsdata(Txbuf, 0, ZCRCE);
               goto gotack;
            }
         }
      }
   } while (n == Blklen);

   for (;;) {
      if (QuitFlag) {
         return NERROR;
      }
      stohdr(Txpos);
      zsbhdr(ZEOF, Txhdr);
      switch (getinsync(0)) {
         case ZACK:
            continue;
         case ZRPOS:
            goto somemore;
         case ZRINIT:
            return OK;
         case ZSKIP:
            return c;
         default:
            return NERROR;
      }
   }
}

/************************** END OF MODULE 7A *********************************/
