/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int opabort();
extern int rzfile();
extern int tryz();

/*
 * Receive 1 or more files with ZMODEM protocol
 */

int rzmfile()
{
   static int c;

   for (;;) {
      if (opabort()) {
         return NERROR;
      }
      c = rzfile();
      closeit();
      switch (c) {
         case ZEOF:
         case ZSKIP:
            switch (tryz()) {
               case ZCOMPL:
                  return OK;
               default:
                  return NERROR;
               case ZFILE:
                  break;
            }
            continue;
         default:
            return c;
         case NERROR:
            return NERROR;
      }
   }
}

/************************** END OF MODULE 5 *********************************/
