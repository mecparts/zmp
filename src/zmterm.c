/*         TERM module File #1            */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <cpm.h>
#include <time.h>
#include "zmp.h"

#define autolen 6      /* length of ZRQINIT required for auto rx */

extern char *alloc(int);
extern int allocerror(void *);
extern int getch();
extern int help();
extern int minprdy();
extern int mcharinp();
extern int dio();
extern int dial();
extern int screenpr();

static void prtchr(char);
static void tobuffer(int);
static void prompt(short);
static void toprinter(int);
static void toggleprt();
static int getprtbuf();
static void doexit();
static int prtservice();
static int pready();
static void adjustprthead();

ovmain()
{
   short mdmdata, dolabel, keycount, keypoint, keywait;
   short lastkey = '\0', fkey, autopos, escwait;
   char keybuf[25];
   static char autoseq[] = { '*','*',CTRLX,'B','0','0' };
   char kbdata;
   char literal;

   if (FirsTerm) {
      locate(0,0);              /* print header if first time*/
      prompt(FALSE);            /* but don't clear screen */
      locate(15,0);             /* then put cursor near bottom */
      printf("Ready.    \n");   /* overwrite 'wait' */
      FirsTerm = FALSE;         /* don't come here again */
   } else {
      prompt(TRUE);
   }
   startcapture();
   autopos = keycount = 0;      /* no remote xfer, no keys in buffer */
   keywait = 0;
   Zmodem = FALSE;              /* ensure we don't auto zmodem */
   purgeline();                 /* get rid of any junk */
   literal = FALSE;

   /* Main loop: */
   for (;;) {
      kbdata = '\0';
      if (keywait) {
         --keywait;
         mswait(1);
      } else if (keycount) {           /* get any buffered keys */
         kbdata = keybuf[keypoint++];
         keycount--;
         keywait = 25;
         if (kbdata == '\\') {
            if (literal) {
               literal = FALSE;
            } else {
               kbdata = '\0';
               literal = TRUE;
            }
         } else if (kbdata == RET ) {   /* Translate ! to CR */
            if (!literal) {
               kbdata = CR;
            }
            literal = FALSE;
         } else if (kbdata == WAITASEC) {
            if (!literal) {
               keywait = 1000;     /* handle pause */
               kbdata = '\0';      /* that's it for this loop */
            }
            literal = FALSE;
         }
      }                         /* end of function keys */  
      if (!kbdata) {
         kbdata = getch();      /* if none, any at keyboard      */
         if (kbdata == ESC) {   /* if another key follows an ESC */
            escwait = 5;        /* within 5ms, it's a keyboard   */
            do {                /* cursor or function key - just */
               kbdata = getch();/* send it                       */
               if (!kbdata) {
                 mswait(1);
               }
            } while (!kbdata && --escwait > 0);       
            if (kbdata) {       /* keyboard cursor/function key */
               mcharout(ESC);   /* send ESC right now and let   */
               if (!FDx) {      /* ZMP see next char in sequence*/
                  prtchr(ESC);
               }
            } else {
               kbdata = ESC;    /* if no next char, normal ZMP  */
            }                   /* ESC processing               */
         }
      }
      if (kbdata)  {
         if (lastkey == ESC) {
            fkey = toupper(kbdata);
            /* allow ESC ESC without complications */
            lastkey = (kbdata == ESC) ? '\0' : kbdata;
            dolabel = TRUE;
            if (fkey == HELP) {
               keybuf[0] = help();
               if (keybuf[0]) {
                  lastkey = ESC;
                  keycount = 1;
                  keypoint = 0;
                  dolabel = FALSE;
               }
            } else if (fkey==RECEIVE || fkey==SEND || fkey==CONFIG) {
               keep(Lastlog,FALSE);
               return fkey;
            } else if (fkey==CAPTURE) {
               capturetog(Logfile);
               dolabel = FALSE;
            } else if (fkey==DIR) {
               killlabel();
               keep(Lastlog,FALSE);
               directory();
               startcapture();
               dolabel = FALSE;
            } else if (fkey==PRTSCRN) {
               screenpr();
               dolabel = FALSE;
            } else if (fkey==HANGUP) {
               hangup();
               dolabel = FALSE;
            } else if (fkey==COMMAND) {
               docmd();
            } else if (fkey==DIAL) {
               keep(Lastlog,FALSE);
               dial();
               dolabel = FALSE;
               purgeline();
               startcapture();
            } else if (fkey==TOGPRT) {
               toggleprt();
               dolabel = FALSE;
            } else if (fkey==DISK) { 
               diskstuff();
            } else if (fkey==QUIT) {
               doexit();
            } else if (fkey==CLRSCR) {
               cls();
            } else if (fkey==BRK) {
               sendbrk();
               printf("\nBreak sent.\n");
               dolabel = FALSE;
            } else if (fkey==DISPKEYS) {
               keydisp();
               dolabel = FALSE;
            } else {
               dolabel = FALSE;
               if (fkey>='0' && fkey<='9') {
                  strcpy(keybuf,KbMacro[fkey-'0']);
                  keycount = strlen(keybuf);
                  keypoint = 0;
               } else {
                  mcharout(kbdata); /* send it if not anything else */
               }
            }

            if (dolabel) {
               prompt(TRUE);   /* print header */
            }
            /* end of if lastkey == ESC */
         } else if ((lastkey = kbdata) != ESC) {
            mcharout(kbdata);   /* Not a function key */
ilfloop:
            if (!FDx) {
               prtchr(kbdata);
               if (kbdata == CR) {
                  kbdata = LF;
                  if (RemEcho) {
                     mcharout(kbdata);
                  }
                  goto ilfloop;
               }
            }
         }
      }                     /*  end of if char at kbd  */

      if (minprdy()) {
         mdmdata = mcharinp();   /* Character at modem */
         if (mdmdata == autoseq[autopos++]) {   /* ZRQINIT? */
            if (autopos == autolen) {
               printf("\nZmodem receive.\n");
               Zmodem = TRUE;   /* yes, do auto.. */
               return RECEIVE;   /* ..zmodem receive */
            }
         } else {
            /* no, reset ZRQINIT sequence test */
            autopos = (mdmdata == '*') ? 1 : 0;
         }
         if (ParityMask) {      /* if flag on, */
            mdmdata &= 0x7f;    /* remove parity */
         }
         if (Filter && (mdmdata > '\r') && (mdmdata < ' ')) {
            goto endloop;   /* filter control chars */
         }
olfloop:
         prtchr(mdmdata);   /* print the character */
         tobuffer(mdmdata);
         toprinter(mdmdata);
         if (RemEcho) {
            mcharout(mdmdata);
            if (mdmdata == CR) {
               mdmdata = LF;
               goto olfloop;
            }
         }
      }
endloop:
      prtservice();             /* service printer at the end of each loop */
   }                            /* end of while */
}                               /* end of main */


/* print character, handling tabs (can't use bdos 2 as it reacts to ctl-s) */
static void prtchr(c)
char c;
{
   bios(BIOS_CONOUT,c);
}

static void tobuffer(c)
int c;
{
   if (BFlag && c) {
      MainBuffer[TxtPtr++] = (char)c;
      if (TxtPtr > Buftop) {
         keep(Lastlog,TRUE);   /* must be true since remote */
         startcapture();      /* is probably still going */
      }
   }
}

/* Print message at top of page. Clear screen first if clear set */
static void prompt(clear)
short clear;
{
   time_t tt;
   struct tm *ptm;
   char ampm;
   int hh;
   
   if (clear) {
      cls();
   }
   printf("\rTerminal Mode: ESC H for help.\t\t");
   printf(
      "Drive %c%d:   %u0 baud",
      Currdrive,Curruser,
      Baudtable[Current.cbaudindex]);
   if( ZsDos ) {
      tt = time(NULL);
      ptm = gmtime(&tt);
      hh = ptm->tm_hour;
      if (hh < 12) {
        ampm = 'A';
      } else {
        ampm = 'P';
        hh -= 12;
      }
      if (!hh) {
         hh = 12;
      }
      printf("\t\t%2d:%02d%cM",hh,ptm->tm_min,ampm);
   }
   putchar('\n');
}

static void toprinter(i)
int i;
{
   char c;

   c = (char) i;
   if (PFlag && (c != '\f')) {   /* don't print form feeds */
      *Prthead++ = c;
      adjustprthead();
   }
}

static void toggleprt()
{
   PFlag = !PFlag;
   if (PFlag) {
      if (getprtbuf() != OK) {
         PFlag = FALSE;
      } else {
         printf("\nPrinter ON\n");
      }
   } else {
      while (prtservice())
         ;   /* Empty the buffer */
      bdos(CPMWLST,'\r');   /* do final cr/lf */
      bdos(CPMWLST,'\n');
      free(Prtbuf);
      printf("\nPrinter OFF\n");
   }
}

static int getprtbuf()
{
   keep(Lastlog,TRUE);   /* need to steal some of the buffer */
   Prtbuf = alloc(Pbufsiz);
   if (allocerror(Prtbuf)) {
      return NERROR;
   }
   Prthead = Prttail = Prtbottom = Prtbuf;
   Prttop = Prtbuf + Pbufsiz - 1;
   startcapture();

#ifdef DEBUG
   printf("\nPrtbuf = %x\n",Prtbuf);
#endif

   return OK;
}

/* Quit. */
static void doexit()
{
   char c;

   killlabel();
   putlabel("Are you SURE you want to exit ZMP? (Y/N) <Y>");
   c = dio();
   if (toupper(c) == 'N') {
      return;
   }
   keep(Lastlog,FALSE);
   reset(Invokdrive,Invokuser);
   deinitve();      /*restore interrupt vector*/
   cls();
   userout();   /* user-defined exit routine */
   exit(0);      /* and quit */

}

static int prtservice()      /*printer service routine*/
{
   if (PFlag) {
      if (pready()) {
         if (Prthead != Prttail) {
            bdos(CPMWLST,*Prttail++);         /* write list byte */
            if (Prttail > Prttop) {
               Prttail = Prtbottom;
            }
         }
         return (Prthead != Prttail);   /* Return true if buffer full */
      }
   }
   return FALSE;
}

static int pready()   /*get printer status using bios call*/
{
   return (bios(14 + 1));
}

static void adjustprthead()
{
   if (Prthead > Prttop) {
      Prthead = Prtbottom;
   }
}

/*         End of TERM module File 1         */
