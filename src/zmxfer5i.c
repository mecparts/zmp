/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"

extern void lreport(int,long);
extern void crcrept(int);

extern int Crc32;

/* Status report: don't do unless after error or ZCRCW since characters */
/*   will be lost unless rx has interrupt-driven I/O         */
void statrep(rxbytes)
long rxbytes;
{
   lreport(KBYTES,rxbytes);
   crcrept(Crc32);
}

/************************** END OF MODULE 5 *********************************/
