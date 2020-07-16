/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern int opabort();
extern void wait(int);
extern void sendbrk();
extern void mcharout(char);

/*
 * Send a string to the modem, processing for \336 (sleep 1 sec)
 *   and \335 (break signal)
 */
int zmputs(s)
char *s;
{
   static int c;

   while (*s) {
      if (opabort())
         return NERROR;
      switch (c = *s++) {
      case '\336':
         wait(1); 
         continue;
      case '\335':
         sendbrk(); 
         continue;
      default:
         mcharout(c);
      }
   }
}

/************************** END OF MODULE 5 *********************************/
