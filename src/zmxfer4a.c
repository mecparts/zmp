/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <unixio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int wcrx();
extern int procheader(char *);
extern int wcrxpn(char *);
extern int opabort();
extern int rzmfile();
extern char *grabmem(unsigned *);
extern char *alloc(int);
extern int allocerror(void *);
extern void rlabel();
extern void hidecurs();
extern void savecurs();
extern void showcurs();
extern void restcurs();
extern int openerror(int,char *,int);
extern int tryz();

extern int Fd;
extern int Errors;
extern int Zctlesc;
extern int Baudrate;
extern int Tryzhdrtype;
extern char *Secbuf;
extern char *Cpmbuf;
extern unsigned Cpbufsize;
extern unsigned Cpindex;
extern char *Rxptr;
extern int Rxtimeout;

int Wcsmask = 0377;

int wcreceive(filename)
char *filename;
{
   static int c;

   rlabel();
   QuitFlag = FALSE;
   Zctlesc = 0;
   Baudrate = Baudtable[Current.cbaudindex];
   Tryzhdrtype = ZRINIT;
   Secbuf = alloc(KSIZE + 1);
   if (allocerror(Secbuf)) {
      return NERROR;
   }
   Cpmbuf = grabmem(&Cpbufsize);
   if (allocerror(Cpmbuf)) {
      return NERROR;
   }
   Cpindex = 0;                 /* just in case */
   Rxptr = Cpmbuf;              /* ditto */
   Rxtimeout = 100;             /* 10 seconds */
   Errors = 0;

#ifdef   DEBUG
   printf("\nbuffer size = %u\n",Cpbufsize);
   wait(5);
#endif

   savecurs();
   hidecurs();
   box();
   if (filename == (char *) 0) {/* batch transfer */
      Crcflag=(Wcsmask==0377);
      if (c=tryz()) {           /* zmodem transfer */
         report(PROTOCOL,"ZMODEM Receive");
         if (c == ZCOMPL) {
            goto good;
         }
         if (c == NERROR) {
            goto fubar;
         }
         c = rzmfile();
         if (c) {
            goto fubar;
         }
      } else {                  /* ymodem batch transfer */
         report(PROTOCOL,"YMODEM Receive");
         report(BLKCHECK,Crcflag?"CRC":"Checksum");
         for (;;) {
            if (opabort()) {
               goto fubar;
            }
            if (wcrxpn(Secbuf)== NERROR) {
               goto fubar;
            }
            if (Secbuf[0]==0) {
               goto good;
            }
            if (procheader(Secbuf) == NERROR) {
               goto fubar;
            }
            if (wcrx()==NERROR) {
               goto fubar;
            }
         }
      }
   } else {
      report(PROTOCOL,"XMODEM Receive");
      strcpy(Pathname,filename);
      checkpath(Pathname);

      testexist(Pathname);
      Fd = creat(Pathname, 0);
      if (openerror(Fd,Pathname,UBIOT)) {
         goto fubar1;
      }
      if (wcrx()==NERROR) {     /* xmodem */
         goto fubar;
      }
   }
good:
   free(Cpmbuf);
   free(Secbuf);
   showcurs();
   restcurs();
   return OK;

fubar:
   canit();
   if (Fd >= 0) {
      unlink(Pathname);         /* File incomplete: erase it */
   }
fubar1:
   free(Cpmbuf);
   free(Secbuf);
   showcurs();
   restcurs();
   return NERROR;
}
/************************** END OF MODULE 8 *********************************/
