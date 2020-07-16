/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void slabel();
extern char *alloc(int);
extern int allocerror(void *);
extern char *grabmem(unsigned *);
extern void savecurs();
extern void hidecurs();
extern void box();
extern void report(int,char *);
extern void stohdr(long);
extern void zshhdr(int,char *);
extern int getzrxinit();
extern void clrreports();
extern int opabort();
extern int wcs(char *);
extern void tfclose();
extern void canit();
extern void zperr(char *,int);
extern void saybibi();
extern void wctxpn(char *);
extern void showcurs();
extern void restcurs();

char *Rxptr;
int Tryzhdrtype;
int Beenhereb4;
unsigned Rxbuflen = 16384;      /* Receiver's max buffer length */
int Noeofseen = 0;
int Totsecs;                    /* total number of sectors this file */
char *Txbuf;
int Filcnt;                     /* count of number of files opened */
long Bytcnt;
long Lastread;                  /* Beginning offset of last buffer read */
int Lastn;                      /* Count of last buffer read or -1 */
int Dontread;                   /* Don't read the buffer, it's still there */
int Incnt;                      /* count for chars not read from the Cpmbuf */
struct stat Fs;

long Lrxpos;		/* Receiver's last reported offset */
int Errors;
int Lastrx;
int Firstsec;
int Eofseen;		/* indicates cpm eof (^Z) has been received */
int Fd;
char *Cpmbuf;             /* buffer bytes for writing to disk */
unsigned Cpbufsize;       /* size of Cpmbuf */
unsigned Cpindex = 0;          /* index for Cpmbuf */
char *Secbuf;       /* sector buffer receiving */
int Zctlesc;		   /* Encode control characters */
int Rxtimeout = 100;		   /* Tenths of seconds to wait for something */
int Rxtype;		/* Type of header received */
int Rxcount;		/* Count of data bytes received */
char Rxhdr[4];		/* Received header */
char Txhdr[4];		/* Transmitted header */
long Rxpos;		/* Received file position */
long Txpos;		/* Transmitted file position */
int Txfcs32;		/* TRUE means send binary frames with 32 bit FCS */
int Crc32t;		/* Display flag indicating 32 bit CRC being sent */
int Crc32;		/* Display flag indicating 32 bit CRC being received */
char Attn[ZATTNLEN+1];	/* Attention string rx sends to tx on err */
int Baudrate;

wcsend(argc, argp)
int argc;                       /* nr of files to send */
char *argp[];                   /* list of file names */
{
   int n, status;

   slabel();
   Zctlesc = 0;
   Incnt = 0;
   Baudrate = Baudtable[Current.cbaudindex];
   Filcnt = Errors = 0;

   Fd = -1;

   Txbuf = alloc(KSIZE);
   if (allocerror(Txbuf)) {
      return NERROR;
   }
   Cpmbuf = grabmem(&Cpbufsize);
   if (allocerror(Cpmbuf)) {
      return NERROR;
   }
   Cpindex = 0;                 /* just in case */
   Crcflag  = FALSE;
   Firstsec = TRUE;
   Bytcnt = -1;
   Rxtimeout = 600;   
   savecurs();
   hidecurs();
   box();
   status = NERROR;
   report(PROTOCOL,Xmodem?"XMODEM Send":Zmodem?"ZMODEM Send":"YMODEM Send");
   if (Zmodem) {
      stohdr(0L);
      zshhdr(ZRQINIT, Txhdr);
   if (getzrxinit()==NERROR)
      goto badreturn;
   }
   for (n=0; n<argc; ++n) {
      clrreports();
      Totsecs = 0;
      if (opabort() || wcs(argp[n]) == NERROR) {
         goto badreturn;
      }
      tfclose();
   }
   Totsecs = 0;
   if (Filcnt==0) {             /* we couldn't open ANY files */
      canit();
      goto badreturn;
   }
   zperr("Complete",FALSE);
   if (Zmodem) {
      saybibi();
   } else if (!Xmodem) {
      wctxpn("");
   }
   status = OK;

badreturn:
   free(Cpmbuf);
   free(Txbuf);
   showcurs();
   restcurs();
   if (status == NERROR) {
      tfclose();
   }
   return status;
}

/************************** END OF MODULE 7 *********************************/
