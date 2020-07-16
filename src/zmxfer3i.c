/************************* START OF XFER MODULE 3 ***************************/

#include <stdio.h>

extern void putlabel(char *);

extern char Buf[128];

void slabel() /*print send mode labels on the 25th line*/
{
   sprintf(Buf,"SEND FILE Mode:  Press ESC to Abort...");
   putlabel(Buf);
}

/************************** END OF MODULE 7A *********************************/
