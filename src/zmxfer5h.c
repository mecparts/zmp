/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void zperr(char *,int);
extern int procheader(char *);
extern int opabort();
extern void stohdr(long);
extern void zshhdr(int,char *);
extern int zgethdr(char *,int);
extern int zrdata(char *,int);
extern long rclhdr(char *);
extern int closeit();
extern void crcrept(int);
extern int zmputs(char *);
extern void statrep(long);
extern int putsec(int,int);
extern void mcharout(char);
extern void setmodtime();

extern int Tryzhdrtype;
extern int Firstsec;
extern char *Secbuf;
extern int Errors;
extern int Crc32;
extern unsigned Cpindex;
extern unsigned Cpbufsize;
extern char *Rxptr;
extern char Attn[ZATTNLEN+1];
extern int Rxcount;
extern char Rxhdr[4];
extern char Txhdr[4];

#define MAX_ZERRORS 10
/*
 * Receive a file with ZMODEM protocol
 *  Assumes file name frame is in Secbuf
 */
int rzfile()
{
   static int c, n;
   static unsigned bufleft;
   static long rxbytes;

   if (procheader(Secbuf) == NERROR) {
      return (Tryzhdrtype = ZSKIP);
   }

   n = MAX_ZERRORS; 
   rxbytes = 0L;
   Firstsec = TRUE;

   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      stohdr(rxbytes);
      zshhdr(ZRPOS, Txhdr);
nxthdr:
      if (opabort()) {
         return NERROR;
      }
      switch (c = zgethdr(Rxhdr, 0)) {

         default:
            return NERROR;

         case ZNAK:
         case TIMEOUT:
            if ( --n < 0) {
               return NERROR;
            }

         case ZFILE:
            zrdata(Secbuf, KSIZE);
            continue;

         case ZEOF:
            if (rclhdr(Rxhdr) != rxbytes) {
              /*
               * Ignore eof if it's at wrong place - force
               *  a timeout because the eof might have gone
               *  out before we sent our zrpos.
               */
               Errors = 0;  
               goto nxthdr;
            }
            if (closeit()) {
               Tryzhdrtype = ZFERR;
               return NERROR;
            } else {
               setmodtime();
            }
            lreport(KBYTES,rxbytes);
            crcrept(Crc32);
            return c;

         case NERROR:   /* Too much garbage in header search error */
            if ( --n < 0) {
               return NERROR;
            }
            zmputs(Attn);
            continue;

         case ZDATA:
            if (rclhdr(Rxhdr) != rxbytes) {
               if ( --n < 0) {
                  return NERROR;
               }
               zmputs(Attn);  
               continue;
            }
moredata:
            if (opabort()) {
               return NERROR;
            }
            bufleft = Cpbufsize - Cpindex;
            c = zrdata(Rxptr, (bufleft > KSIZE) ? KSIZE : bufleft);
            switch (c) {

               case ZCAN:
                  return NERROR;

               case NERROR:   /* CRC error */
                  statrep(rxbytes);
                  if ( --n < 0) {
                     return NERROR;
                  }
                  zmputs(Attn);
                  continue;

               case TIMEOUT:
                  statrep(rxbytes);
                  if ( --n < 0) {
                     return NERROR;
                  }
                  continue;

               case GOTCRCW:
                  n = MAX_ZERRORS;
                  if (putsec(Rxcount,TRUE) == NERROR) {
                     return NERROR; /* Write to disk! */
                  }
                  rxbytes += Rxcount;
                  stohdr(rxbytes);
                  statrep(rxbytes);
                  zshhdr(ZACK, Txhdr);
                  mcharout(XON);
                  goto nxthdr;

               case GOTCRCQ:
                  n = MAX_ZERRORS;
                  if (putsec(Rxcount,TRUE) == NERROR) {
                     return NERROR; /* Write to disk! */
                  }
                  rxbytes += Rxcount;
                  stohdr(rxbytes);
                  statrep(rxbytes);
                  zshhdr(ZACK, Txhdr);
                  goto moredata;

               case GOTCRCG:
                  n = MAX_ZERRORS;
                  if (putsec(Rxcount,FALSE) == NERROR) {
                     return NERROR; /* Don't write to disk */
                  }
                  rxbytes += Rxcount;
                  goto moredata;

               case GOTCRCE:
                  n = MAX_ZERRORS;
                  if (putsec(Rxcount,FALSE) == NERROR) {
                     return NERROR; /* Don't write to disk */
                  }
                  rxbytes += Rxcount;
                  goto nxthdr;
            }
      }
   }
}

/************************** END OF MODULE 5 *********************************/
