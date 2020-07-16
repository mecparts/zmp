/*************************************************************************/
/*									 */
/*		ZMP - A ZMODEM Program for CP/M				 */
/*									 */
/*	Developed from H. Maney's Heath-specific HMODEM			 */
/*		by Ron Murray and Lindsay Allen				 */
/*									 */
/*************************************************************************/
/*									 */
/* See separate file zmp-hist.doc for details of modification history	 */
/*									 */
/*************************************************************************/
/*                                                                       */
/*  This source code may be distributed freely without restriction       */
/*  for the express purpose of promoting the use of the ZMODEM           */
/*  file transfer protocol.  Programmers are requested to include        */
/*  credit (in the source code) to the developers for any code           */
/*  incorporated from this program.                                      */
/*                                                                       */
/*  This program was inspired by lmodem.c written by David D. Clark      */
/*  in the November 83 issue of Byte.  Although it bears no resemblance  */
/*  to David's program, I can credit him with sparking my interest in    */
/*  modem programs written in C.                                         */
/*                                 - Hal Maney                           */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  The following files comprise ZMP:					 */
/*                                                                       */
/*    zmp.h	        the header file                                  */
/*    zmp.c             the main program Pt.1                            */
/*    zmp2.c            The main program Pt.2				 */
/*    zmterm.c		Terminal overlay				 */
/*    zmterm2.c			"					 */
/*    zmconfig.c        Configuration overlay				 */
/*    zminit.c		Initialisation overlay				 */
/*    zmxfer.c +>>	File transfer overlay (with the next 3)		 */
/*    zmxfer2.c		Chuck Forsberg's sz.c modified for cp/m          */
/*    zmxfer3.c				"				 */
/*    zmxfer4.c         Chuck Forsberg's rz.c modified for cp/m          */
/*    zmxfer5.c				"				 */
/*    zzm.c		Chuck Forsberg's zm.c modified for cp/m          */
/*    zzm2.c				"				 */
/*    zmovl.mac		Sample user-dependent overlay source		 */
/*    zmodem.h          zmodem header file                               */
/*    zmconfig.c        configuration overlay                            */
/*    zconfig.h         configuration overlay header                     */
/*                                                                       */
/*************************************************************************/

#define  MAIN

#include <stdio.h>
#include <cpm.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include "zmp.h"

char *alloc();
int allocerror(void *p);
int isin(char *, char *);
int linetolist(void);
int getline(char *, unsigned);
void addu(char *,int,int);

extern ovloader(char *, ...);
extern int readline(int n);
extern int minprdy(void);

main()
{
   static int termcmd;
   short *p, *getvars();

   signal(SIGINT,SIG_IGN);      /* Ignore control-c's */

   Invokdrive = bdos(CPMIDRV) + 'A';
   Invokuser = getuid();
   p = getvars();
   Overdrive = *p++;            /* get du: for overlays etc */
   Overuser = *p++;
   Userover = (char *) *p++;    /* user-defined overlay list */

   if (!Overdrive) {            /* use Invokdrive if zero */
      Overdrive = Invokdrive;
      Overuser = Invokuser;
   }
   strcpy(Pathname,Initovly);
   addu(Pathname,Overdrive,Overuser);
   ovloader(Pathname);          /* Do initialisation */

   /************** the main loop ************************/

   for(;;) {
      printf("\nWait..");
      strcpy(Pathname,Termovly);
      addu(Pathname,Overdrive,Overuser);
      termcmd = ovloader(Pathname);   /* Load the TERM overlay */
      printf("\nLoading overlay...\n");
      switch(termcmd) {

         case RECEIVE:
         case SEND:
            strcpy(Pathname,Xferovly);
            addu(Pathname,Overdrive,Overuser);
            ovloader(Pathname,termcmd);
            putchar('\007');   /* tell user it's finished */
            mswait(300);
            putchar('\007');
            break;

         case CONFIG:
            strcpy(Pathname,Configovly);
            addu(Pathname,Overdrive,Overuser);
            ovloader(Pathname);
            break;

         default:
            printf("Fatal error in %s.OVR\n",Termovly);
            exit(0);
            break;
      }                         /* end of switch  */
   }                            /* end of for(;;) */
}                               /* end of main    */


char *grabmem(sizep)            /* grab all available memory */
unsigned *sizep;                /* place to store how much we got */
{
   static char *p,*q;
   static unsigned size;

   q = alloc(BUFSIZ + 10);      /* Make sure we have enough for disk i/o */

   size = BUFSTART + 10;        /* allow some overrun */
   while ((p = alloc(size)) == (char *) MEMORY_FULL) {
      size -= 1024;
      if ((size - 10) < 2048) {
         size = 0;
         break;
      }
   }

#ifdef DEBUG
   printf("\ngrabmem = %x %d\n",p,size);
#endif

   *sizep = size - 10;          /* don't mention the overrun */
   free(q);                     /* Free disk i/o space */
   return p;
}

int getpathname(string)
char *string;
{
   static char *buffer;

   buffer = Pathname;
   sprintf(buffer,"\nPlease enter file name%s:  ",string);
   printf(buffer);
   if (!getline(Pathname,PATHLEN)) {
      return 0;
   }
   return linetolist();
}

int linetolist()                    /* expand and put Pathnames in Pathlist, return count */
{
   static char *p;
   static int count;
   static char **tempalloc;

#ifdef DEBUG
   static int i;
#endif

   tempalloc = Pathlist = (char **) alloc(510);
   if (allocerror(tempalloc)) {
      return 0;
   }

#ifdef   DEBUG
   printf("Pathlist = %x\n",Pathlist);
#endif

   count = 0;
   Pathlist[count++] = Pathname;
   for (p=Pathname; *p; p++) {         /* break up into substrings */
      if (*p == ' ') {
         *p = '\0';
         while (*++p == ' ');         /* dump extra spaces */
         Pathlist[count++] = p;
      }
   }

#ifdef   DEBUG
   printf("\nbefore command\n");
   for (i=0; i < count; i++) {
      printf("%d %s\n",i,Pathlist[i]);
   }
#endif

   command(&count,&Pathlist);

#ifdef   DEBUG
   printf("\nafter command\n");
   for (i=0; i < count; i++) {
      printf("%d %s\n",i,Pathlist[i]);
   }
#endif

   free(tempalloc);
   return count;
}

void freepath(n)
int n;
{
   if (n) {
      while (n) {
         free(Pathlist[--n]);
      }
      free(Pathlist);
   }
}

void reset(drive,user)
unsigned drive;
int user;
{
   drive = toupper(drive);
   if (isalpha(drive) && drive <= Maxdrive && user >= 0 && user <= 15) {
      Currdrive = drive;
      bdos(CPMRDS);
      bdos(CPMLGIN,(Currdrive-'A')&0xff);
      Curruser = user;
      setuid(user);
   }
}

void addu(filename,drive,user)
char *filename;
int drive,user;
{
   if (!isin(filename,":") && user >= 0 && user <= 15) {
      strcpy(Buf,filename);
      sprintf(filename,"%d:%c:%s",user,(char)drive,Buf);
   }
}

void deldrive(filename)
char *filename;
{
   char *i, *index();

   if ((i = index(filename,':')) != (char *) NULL) {
      strcpy(filename,i+1);
   }
}

int dio()                           /* direct console port inp when bdos is too slow */
{
   return bios(2 + 1);
}

int chrin()                         /* Direct console input which repeats character */
{
   return bdos(CPMRCON);
}

int getch()
{
   return bdos(CPMDCIO,0xFF);
}

void flush()
{
   while(bdos(CPMICON)) {       /*clear type-ahead buffer*/
      bdos(CPMRCON);
   }
   getch();                     /*and anything else*/
}

void purgeline()
{
   while (minprdy()) {          /*while there are characters...*/
      mcharinp();               /*gobble them*/
   }
}

int openerror(chan,fname,test)
int chan,test;
char *fname;
{
   int result;

   if (result = (chan == test)) {
      printf("\n\nERROR - Cannot open %s\n\n",fname);
      wait(3);
   }
   return result;
}

int fopenerror(fd,fname)
FILE *fd;
char *fname;
{
   int result;

   result = fd == NULL;
   if (result) {
      printf("\n\nERROR - Cannot open %s\n\n",fname);
      wait(3);
   }
   return result;
}

void wrerror(fname)
char *fname;
{
   printf("\n\nERROR - Cannot write to %s\n\n",fname);
   wait(3);
}

char *alloc(cnt)
int cnt;
{
   char *malloc();

   return malloc(cnt);
}

allocerror(p)
void *p;
{
   static int status;

   if (status = (p == (void *) MEMORY_FULL)) {
      perror("Memory allocation failure");
   }
   return status;   
}

perror(string)
char *string;
{
   printf("\007\nERROR - %s\n\n",string);
   wait(3);
}


kbwait(seconds)
unsigned seconds;
{
   static unsigned t;
   static int c;

   t = seconds * 10;
   while(!(c=getch()) && (t--)) {
      mswait(100);
   }
   return (c & 0xff) == ESC;
}

readstr(p,t)
char *p;
int t;
{
   static int c;

   t *= 10;                /* convert to tenths */
   flush();
   while (((c=readline(t)) != CR) && (c != TIMEOUT)) {
      if (c != LF) {
         *p++ = c;
      }
   }
   *p = 0;
   return c;
}

int isin(received,expected)
char *received, *expected;
{
   int i,j,lrec,lexp;
   j = 0;
   lrec = strlen(received);
   lexp = strlen(expected);
   for (i=0; i<lrec; ++i) {
     if (received[i] == expected[j]) {
       ++j;
       if (j >= lexp) {
         return TRUE;
       }
     } else {
       j = 0;
     }
   }
   return FALSE;
}

void report(row,string)
int row;
char *string;
{
   locate(row,RPTPOS);
   printf(string);
}

mstrout(string,echo)
char *string;
int echo;                       /* echo flag means send to screen also */
{
   static char c;

   while (c = *string++) {
      if ((c == RET) || (c == '\n')) {      /* RET is a ! */
         mcharout(CR);
         c = '\n';
      } else if (c == WAITASEC) {
         wait(1);
      } else {
         mcharout(c);
      }
      if (echo) {
         putchar(c);
      }
   }
   mswait(100);                 /* wait 100 ms */
   purgeline();
}

getline(buffer,maxc)
char *buffer;
unsigned maxc;
{
   unsigned count;
   char *p;
   char c;
   count = 0;
   p = buffer;
   *p = NUL;

   while( count <= maxc ) {
      c = getch();
      if (c!=0) {
         if ((c==BS || c==DEL) && count>0) {
            printf("\b \b");
            --count;
            --p;
            *p = NUL;
         } else if (c==CR) {
            return count;
         } else if (c>=' ' && c<='~' && count<maxc) {
            putchar(c);
            *p = c;
            ++count;
            ++p;
            *p = NUL;
         } else {
            putchar('\007');
         }
      }
   }
   return count;
}

/*         End of MAIN module File 1         */
