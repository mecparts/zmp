/********************** START OF XFER MODULE 4 ******************************/

/* rz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>
#include <stdlib.h>
#include <unixio.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int openerror(int,char *,int);
extern void clrreports();
extern void report(int,char *);
extern char *ttime(long);
extern void checkpath(char *);
extern void testexist(char *);
extern void zperr(char *,int);

extern int Fd;

/*
 * Process incoming file information header
 */
int procheader(name)
char *name;
{
   static char *p, *ap, c;
   
   /*
    *  Process YMODEM,ZMODEM remote file management requests
    */

   clrreports();
   p = name + 1 + strlen(name);
   FileModTime = 0;
   if (*p) {   /* file coming from Unix or DOS system */
      ap = p;
      while ((c = *p) && (c != ' ')) /* find first space or null */
         ++p;
      if (c)
         *p = '\0';
      /* ap now points to a long integer in ascii */
      report(FILESIZE,ap);
      report(SENDTIME,ttime(atol(ap)));
      while (*ap && *ap != ' ') { /* skip over filesize */
         ++ap;
      }
      ++ap; /* if *ap is non zero, ap is pointing at the file mod timestamp */
      if( *ap ) {
         while (*ap >= '0' && *ap <= '7') {
            FileModTime <<= 3;
            FileModTime += *ap -'0';
            ++ap;
         }
      }
   }
   
   strcpy(Pathname, name);
   checkpath(Pathname);

   testexist(Pathname);
   Fd = creat(Pathname, 0);
   if (openerror(Fd,Pathname,UBIOT))
      return NERROR;
   return OK;
}

/************************** END OF MODULE 8 *********************************/
