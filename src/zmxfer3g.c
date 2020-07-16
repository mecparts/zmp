/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern int Zmodem;
extern int Baudrate;

char *ttime(fsize)
long fsize;
{
   static unsigned efficiency, cps, seconds;
   static char buffer[10];

   efficiency = Zmodem ? 9 : 8;
   cps = (Baudrate/10) * efficiency;   
   seconds = (unsigned)(fsize/cps);     
   sprintf(buffer,"%u:%02u",seconds/60,seconds%60);
   return buffer;
}

/************************** END OF MODULE 7A *********************************/
