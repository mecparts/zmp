/*         MAIN module File #2            */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"

extern char *alloc(int);
extern int mirdy();
extern int moutrdy();
extern int mchin();
extern int mrd();
extern int getch();

int mread(char *buffer, int count, int timeout);
unsigned mcharinp();
static void clrbox();

void fstat(fname,status)
char *fname;
struct stat *status;
{
   unsigned filelength();

   setfcb(&Thefcb,fname);
   status->records = filelength(&Thefcb);
   getfirst(fname);
   setfcb(&Thefcb,"????????.???");
}

unsigned filelength(fcbp)
struct fcb *fcbp;
{
   int olduser;

   bdos(CPMSDMA,CPMBUF);        /* set dma address */
   olduser = getuid();          /* save this user number */
   setuid(fcbp->uid);           /* go to file's user no. */
   bdos(CPMCFS,fcbp);           /* compute file size */
   setuid(olduser);             /* restore original */
   return (fcbp->ranrec[1]<<8)|fcbp->ranrec[0];
}

int roundup(dividend,divisor)
int dividend, divisor;
{
   return (dividend/divisor + ((dividend%divisor) ? 1 : 0));
}

int getfirst(aname)      /* ambiguous file name */
char *aname;
{
   bdos(CPMSDMA,CPMBUF);         /* set dma address */
   setfcb(&Thefcb,aname);
   return bdos(CPMFFST,&Thefcb) & 0xff;
}

int getnext()
{
   bdos(CPMSDMA,CPMBUF);         /* set dma address */
   return bdos(CPMFNXT,NULL) & 0xff;
}

/* command: expand wild cards in the command line.  (7/25/83)
 * usage: command(&argc, &argv) modifies argc and argv as necessary
 * uses sbrk to create the new arg list
 * NOTE: requires fcbinit() and bdos() from file stdlib.c.  When used
 *   with a linker and stdlib.rel, remove the #include stdlib.c.
 *
 * Written by Dr. Jim Gillogly; Modified for CP/M by Walt Bilofsky.
 * Modified by HM to just get ambiguous fn for zmodem, ymodem.
 */

int COMnf,COMc;
char **COMfn;
char **COMv;
char *COMarg,*COMs;
static expand();

void command(argcp, argvp)
int *argcp;
char ***argvp;
{
   char *p, c;
   char *f_alloc[MAXFILES];

   COMfn = f_alloc;
   COMc = *argcp;
   COMv = *argvp;
   COMnf = 0;
   for (COMarg = *COMv; COMc--; COMarg = *++COMv) {

#ifdef   DEBUG
      printf("\nDoing %s",COMarg);
#endif

      for (COMs = COMarg; *COMs; COMs++) {
         if (*COMs == '?' || *COMs == '*') {   
            if (!expand()) {   /* Too many */
               *argcp = 0;
               return;
            }
            goto contn;  /* expand each name at most once */
         }
      }
      COMfn[COMnf] = alloc(FNSIZE);
      p = COMarg;
      while (c = *p) {       /* Convert to lower case */
         *p++ = tolower(c);
      }
      strcpy(COMfn[COMnf++],COMarg);     /* no expansion */
contn:;
   }
   *argcp = COMnf;
   COMfn[COMnf++] = NULL;
   COMv = *argvp = (char **)alloc(2 * COMnf);
   while (COMnf--) {
      COMv[COMnf] = COMfn[COMnf];
   }
}

int static expand()      /* Returns FALSE if error */
{
   struct fcb searchfcb;
   static char *p,*q,*r,c;
   static int i,flg,olduser;

#ifdef   DEBUG
   printf("\nExpanding %s",COMarg);
#endif
   olduser = getuid();         /* save original user area */
   setfcb(&searchfcb,COMarg);
   setuid(searchfcb.uid);              /* go to specified user area */
   flg = CPMFFST;
   bdos(CPMSDMA,0x80);           /* Make sure DMA address OK */
   while (((i = bdos(flg,&searchfcb)) & 0xff) != 0xff) {
      COMfn[COMnf++] = q = alloc(FNSIZE);
      if (COMnf >= MAXFILES-1) {
         for (p = "Too many file names.\n"; putchar(*p++); ); {
            setuid(olduser);
            return FALSE;
         }
      }
      p = (char *)(0x81 + i * 32);   /* Where to find dir. record */
      /* transfer du: first */
      if (strchr(COMarg,':') && COMarg[0] != '?') {
         r = COMarg;
         do {
            *q++ = c = *r++;
         } while (c != ':');
      }
      /* Now transfer filename */
      for (i = 12; --i; ) {
         if (i == 3) {
            *q++ = '.';
         }
         *q = *p++ & 0x7F;
         *q = tolower(*q);
         if (*q != ' ') {
            ++q; 
         }
      }
      *q = 0;
      flg = CPMFNXT;
   }
   setuid(olduser);
   return TRUE;
}

int ctr(p)
char *p;
{
   if( strlen(p) >= 80 ) {
      return 0;
   }
   return (80 - strlen(p))/2;
}

int opabort()
{
   Lastkey = getch() & 0xff;
   if (Lastkey == ESC) {
      flush();
      if (!Inhost && !Dialing) {
         report(MESSAGE,"Operator abort");
      }
      QuitFlag = TRUE;
   }
   return QuitFlag;
}

/*
 * readock(timeout, count) reads character(s) from modem
 *  (1 <= count <= 3)
 * it attempts to read count characters. If it gets more than one,
 * it is an error unless all are CAN
 * (otherwise, only normal response is ACK, CAN, or C)
 *
 * timeout is in tenths of seconds
 */

int readock(timeout, count)
int timeout, count;
{
   int c;
   char byt[5];

   c = mread(byt,count,timeout);
   if (c < 1) {
      return TIMEOUT;
   }
   if (c == 1) {
      return byt[0] & 0xFF;
   } else {
      while (c) {
         if (byt[--c] != CAN) {
            return NERROR;
         }
      }
      return CAN;
   }
}

/*
 * readline(n): reads 1 character from modem in n/10 seconds
 */
int readline(n)
int n;
{
   return readock(n,1);
}

void putlabel(string)
char string[];
{
   cls();
   locate(0,ctr(string) - 1);   /* Centre on top line */   
   stndout();                   /* Inverse video */
   printf(" %s \n\n",string);   /* Print the string */
   stndend();                   /* Inverse off */
}

void killlabel() /*disable 25th line*/
{
   cls();                       /* just clear screen */
}

int mgetchar(seconds)               /* allows input from modem or operator */
int seconds;
{
   static int c, tenths;

   Lastkey = 0;
   tenths = seconds * 10;
   if ((c=readline(tenths)) != TIMEOUT) {
      return c & 0xff;
   } else if (Lastkey) {
      return Lastkey;
   }
   return TIMEOUT;
}

void box()          /* put box on screen for file transfer */
{
   register int i;
   static char *headings[] = {
      "",
      "Protocol:",
      "File Name:",
      "File Size:",
      "Block Check:",
      "Transfer Time:",
      "Bytes Transferred:",
      "Blocks Transferred:",
      "Sectors in File:",
      "Error Count:",
      "Last Message:  NONE"
   };
   static int start[] = {
      0, 
      13+LC, 
      12+LC, 
      12+LC, 
      10+LC, 
      8+LC, 
      4+LC, 
      3+LC, 
      6+LC,
      10+LC, 
      9+LC
   };

   locate(TR,LC);
   putchar(UL);
   for (i = 1; i < WD-1; i++) {
      putchar(HORIZ);
   }
   putchar(UR);
   locate(BR,LC);
   putchar(LL);
   for (i = 1; i < WD-1; i++) {
      putchar(HORIZ);
   }
   putchar(LR);
   for (i = 1; i < HT-1; i++) {
      locate(TR+i,LC);
      putchar(VERT);
      locate(TR+i,RC);
      putchar(VERT);
   }
   clrbox();
   for (i=1; i<11; i++) {
      locate(TR+i,start[i]);
      printf(headings[i]);
   }   
}

static void clrbox()
{
   register int i;

   for (i=TR+1; i < BR; i++) {
      locate(i,LC+1);
      printf("                                       ");
   }
}

int mread(buffer, count, timeout)   /* time in tenths of secs */
char *buffer;
int count, timeout;
{
   int i, c;

   i = 0;
   while (!(c = mrd()) && (timeout--) && !opabort());
   if (c) {
      buffer[i++] = mcharinp();
   }
   return i;
}

unsigned mcharinp()
{
   unsigned c;

   c = mchin();
   if (Stopped) {
      mchout(CTRLQ);
      Stopped = FALSE;
   }
   return c;
}

void mcharout(c)
char c;
{
   while (!moutrdy()) {
      opabort();                /* Test for operator abort while we wait */
   }
   mchout(c);                   /* Then send it */
}

int minprdy()
{
   return mirdy() || Stopped;
}

/*          End of MAIN module             */
