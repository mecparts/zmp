/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void stohdr(long);
extern void zshhdr(int,char *);
extern void mcharout(char);

extern char Txhdr[4];
extern char Rxhdr[4];

/* Say "bibi" to the receiver, try to do it cleanly */

void saybibi()
{
   for (;;) {
      stohdr(0L);      /* CAF Was zsbhdr - minor change */
      zshhdr(ZFIN, Txhdr);   /*  to make debugging easier */
      switch (zgethdr(Rxhdr, 0)) {
         case ZFIN:
            mcharout('O'); 
            mcharout('O'); 
         case ZCAN:
         case TIMEOUT:
            return;
      }
   }
}

/************************** END OF MODULE 7A *********************************/
