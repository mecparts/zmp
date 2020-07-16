/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <unixio.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void stohdr(long);
extern void purgeline();
extern zshhdr(int,char *);
extern int readline(int);

extern char Txhdr[4];
/*
 * Ack a ZFIN packet, let byegones be byegones
 */

void ackbibi()
{
   static int n;

   stohdr(0L);
   for (n=3; --n>=0; ) {
      purgeline();
      zshhdr(ZFIN, Txhdr);
      switch (readline(100)) {
      case 'O':
         readline(INTRATIME);   /* Discard 2nd 'O' */
         return;
      case RCDO:
         return;
      case TIMEOUT:
      default:
         break;
      }
   }
}
   
/************************** END OF MODULE 5 *********************************/
