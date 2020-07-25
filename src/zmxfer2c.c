/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void report(int,char *);
extern void lreport(int,long);
extern void dreport(int,int);
extern int getnak();
extern void deldrive(char *);
extern int zsendfile(char *,int);
extern int wcputsec(char *,int,int);
char *ttime(long);
extern void zperr(char *,int);

extern char *Txbuf;
extern struct stat Fs;

/*
 * generate and transmit pathname block consisting of
 *  pathname (null terminated),
 *  file length, modification time and file mode
 *  in octal.
 *  N.B.: modifies the passed name, may extend it!
 */
int wctxpn(name)
char *name;
{
   static char *p;
   static unsigned length;
   static long nrbytes;

   memset(Txbuf,'\0',KSIZE);
   p = Txbuf;
   if (*name) {
      length = Fs.records;
      nrbytes = (long)length * 128;
      report(PATHNAME,name);
      lreport(FILESIZE,nrbytes);
      dreport(FBLOCKS,length);
      report(SENDTIME,ttime(nrbytes));   
      if (Xmodem) {                 /* xmodem, don't send path name */
         return OK;
      }
      if (!Zmodem) {
         Blklen = KSIZE;
         if (getnak()) {
            return NERROR;
         }
      }
      strcpy(Txbuf,name);
      deldrive(Txbuf);      /* remove drive ind if any */
      p = Txbuf + strlen(Txbuf);
      ++p;
      sprintf(p,"%lu %lo 100644",nrbytes,Fs.modtime);
   }
   if (Zmodem) {
      return zsendfile(Txbuf, 1+strlen(p)+(p-Txbuf));
   }
   if (wcputsec(Txbuf, 0, SECSIZ)==NERROR) {
      return NERROR;
   }
   return OK;
}

/************************** END OF MODULE 7 *********************************/
