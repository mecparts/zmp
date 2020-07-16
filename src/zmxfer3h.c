/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>
#include <unixio.h>

extern int Fd;

void tfclose()          /* close file if still open */
{
   if (Fd >= 0) {
      close(Fd);
   }
   Fd = -1;
}

/************************** END OF MODULE 7A *********************************/
