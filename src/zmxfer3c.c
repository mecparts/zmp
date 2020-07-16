/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern void zsbhdr(int,char *);
extern int readline(int);
extern void clrline(int);
extern int zsndfdata();
extern int zgethdr(char *,int);

extern char Rxhdr[4];
extern char Txhdr[4];
extern long Lastsync;
extern long Bytcnt;
extern long Rxpos;
extern long Txpos;
extern int Incnt;
extern int Dontread;
extern int Sending;
extern int Fd;

long Lastsync;

/* Send file name and related info */

int zsendfile(buf, blen)
char *buf;
int blen;
{
   static int c;

   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      Txhdr[ZF0] = 0;   /* file conversion request */
      Txhdr[ZF1] = 0;   /* file management request */
      Txhdr[ZF2] = 0;   /* file transport request */
      Txhdr[ZF3] = 0;
      zsbhdr(ZFILE, Txhdr);
      zsdata(buf, blen, ZCRCW);
again:
      c = zgethdr(Rxhdr, 1);
      if (c==ZRINIT) {
         while ((c = readline(INTRATIME)) > 0) {
            if (c == ZPAD) {
               goto again;
            }
         }
      } else if (c==ZCAN || c==TIMEOUT || c==ZABORT || c==ZFIN) {
         return NERROR;
      } else if (c==ZSKIP) {
         return c;
      } else if (c==ZRPOS) {
        /*
         * Suppress zcrcw request otherwise triggered by
         * lastyunc==Bytcnt
         */
         Lastsync = (Bytcnt = Txpos = Rxpos) -1L;

         lseek(Fd, Rxpos, 0);   /* absolute offset */

         clrline(KBYTES);
         Incnt = 0;
         Dontread = FALSE;
         c = zsndfdata();
         Sending = FALSE;
         return c;
      }
   }
}

/************************** END OF MODULE 7A *********************************/
