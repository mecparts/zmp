/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern void stohdr(long);
extern void zshhdr(int,char *);
extern int zgethdr(char *,int);
extern int zrdata(char *,int);
extern void purgeline();
extern void zperr(char *,int);
extern void ackbibi();

extern char Rxhdr[4];
extern char Txhdr[4];
extern char *Secbuf;
extern int Errors;
extern char Attn[ZATTNLEN+1];
extern int Tryzhdrtype;
extern unsigned Cpbufsize;
extern int Zctlesc;

/*
 * Initialize for Zmodem receive attempt, try to activate Zmodem sender
 *  Handles ZSINIT frame
 *  Return ZFILE if Zmodem filename received, -1 on error,
 *   ZCOMPL if transaction finished,  else 0
 */
int tryz()
{
   static int c, n, *ip;
   static int cmdzack1flg;

   if (Nozmodem) {              /* ymodem has been forced */
      return 0;
   }

   for (n=Zmodem?15:5; --n>=0; ) {
      if (opabort()) {
         return NERROR;
      }
      /* Set buffer length (0) and capability flags */
      stohdr(0L);
      Txhdr[ZF0] = (Wantfcs32 ? CANFC32 : 0) | CANFDX;
      if (Zctlesc) {
         Txhdr[ZF0] |= TESCCTL;
      }
      ip = (int *)&Txhdr[ZP0];
      *ip = Cpbufsize;
      zshhdr(Tryzhdrtype, Txhdr);
      if (Tryzhdrtype == ZSKIP) {  /* Don't skip too far */
         Tryzhdrtype = ZRINIT;     /* CAF 8-21-87 */
      }
again:
      switch (zgethdr(Rxhdr, 0)) {
         case ZRQINIT:
            continue;
         case ZEOF:
            continue;
         case TIMEOUT:
            continue;
         case ZFILE:
            Tryzhdrtype = ZRINIT;
            c = zrdata(Secbuf, KSIZE);
            if (c == GOTCRCW) {
               return ZFILE;
            }
            zshhdr(ZNAK, Txhdr);
            goto again;
         case ZSINIT:
            Zctlesc = TESCCTL & Rxhdr[ZF0];
            if (zrdata(Attn, ZATTNLEN) == GOTCRCW) {
               zshhdr(ZACK, Txhdr);
               goto again;
            }
            zshhdr(ZNAK, Txhdr);
            goto again;
         case ZFREECNT:
            stohdr(0L);
            zshhdr(ZACK, Txhdr);
            goto again;
         case ZCOMMAND:
            cmdzack1flg = Rxhdr[ZF0];
            if (zrdata(Secbuf, KSIZE) == GOTCRCW) {
               stohdr(0L);
               purgeline();   /* dump impatient questions */
               do {
                  zshhdr(ZCOMPL, Txhdr);
                  zperr("Waiting for ZFIN",FALSE);
                  if (opabort()) {
                     return NERROR;
                  }
               } while (++Errors<20 && zgethdr(Rxhdr,1) != ZFIN);
               ackbibi();
               return ZCOMPL;
            }
            zshhdr(ZNAK, Txhdr); 
            goto again;
         case ZCOMPL:
            goto again;
         default:
            continue;
         case ZFIN:
            ackbibi(); 
            return ZCOMPL;
         case ZCAN:
            return NERROR;
      }
   }
   return 0;
}

/************************** END OF MODULE 5 *********************************/
