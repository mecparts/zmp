/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern void stohdr(long);
extern int opabort();
extern void zsbhdr(int,char *);
extern void zsdata(char *,int,int);
extern int zgethdr(char *,int);
extern void zperr(char *,int);

extern char *Txbuf;
extern char Rxhdr[4];
extern char Txhdr[4];

/*
 * Attention string to be executed by receiver to interrupt streaming data
 *  when an error is detected.  A pause (0336) may be needed before the
 *  ^C (03) or after it. 0337 causes a break to be sent.
 */
#define SLEEP 0336
static char Myattn[] = { CTRLC,SLEEP,0 };

/* Send send-init information */

int sendzsinit()
{
   int tries;

   stohdr(0L);      /* All flags are undefined */
   strcpy(Txbuf,Myattn);   /* Copy Attn string */
   for (tries = 0; tries < 20; tries++) {
      if (opabort()) {
         return NERROR;
      }
      zsbhdr(ZSINIT,Txhdr);   /* Send binary header */
      zsdata(Txbuf,strlen(Txbuf) + 1,ZCRCW);   /* Send string */
      if (zgethdr(Rxhdr,0) == ZACK) {
         return OK;
      }
      zperr("Bad ACK: ZSINIT",FALSE);
   }
   return NERROR;
}

/************************** END OF MODULE 7A *********************************/
