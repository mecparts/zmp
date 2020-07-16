/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern int zgethdr(char *,int);
extern void zperr(char *,int);
extern void clrline(int);

extern char Rxhdr[4];
extern char Txhdr[4];
extern int Beenhereb4;
extern long Rxpos;
extern long Txpos;
extern long Lrxpos;
extern long Bytcnt;
extern long Lastsync;
extern long Lastread;
extern int Incnt;
extern int Fd;
extern int Dontread;
extern int Lastn;
extern char *frametypes[];

/*
 * Respond to receiver's complaint, get back in sync with receiver
 */
int getinsync(flag)      /* flag means that there was an error */
int flag;
{
   static int c;
   unsigned u;

   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      c = zgethdr(Rxhdr, 0);
      c = c < FRTYPES ? c : FRTYPES-1;
      sprintf(Buf,"Got %s", frametypes[c+FTOFFSET]);
      zperr(Buf,flag);
      switch (c) {

         case ZCAN:
         case ZABORT:
         case ZFIN:
         case TIMEOUT:
            return NERROR;

         case ZRPOS:
            /* ************************************* */
            /*  If sending to a modem buffer, you    */
            /*   might send a break at this point to */
            /*   dump the modem's buffer.            */
            /* ************************************* */
            if (Lastn >= 0 && Lastread == Rxpos) {
               Dontread = TRUE;
            } else {
               u = lseek(Fd, Rxpos, 0);   /* absolute offset */
               clrline(KBYTES);
               Incnt = 0;
            }
            Bytcnt = Lrxpos = Txpos = Rxpos;
            if (Lastsync == Rxpos) {
               if (++Beenhereb4 > 4) {
                  if (Blklen > 256) {
                     Blklen /= 2;
                  }
               }
            }
            Lastsync = Rxpos;
            return c;

         case ZACK:
            Lrxpos = Rxpos;
            if (flag || Txpos == Rxpos) {
               return ZACK;
            }
            continue;

         case ZRINIT:
         case ZSKIP:
            return c;

         case NERROR:
         default:
            zsbhdr(ZNAK, Txhdr);
            continue;
      }
   }
}

/************************** END OF MODULE 7A *********************************/
