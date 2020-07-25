/*         MAIN module File #3            */

#include <stdio.h>
#include <cpm.h>
#include <time.h>
#include "zmp.h"

static int bin2bcd(bin)
int bin;
{
   return ((bin / 10) << 4) | (bin % 10);
}

static int bcd2bin(bcd)
uchar bcd;
{
   return 10*(bcd>>4)+(bcd&0x0F);
}

void setmodtime()
{
   struct fcb fc;
   short      luid;
   struct tm  *tp;
   
   if (ZsDos && FileModTime && !setfcb(&fc, Pathname)) {
      tp = localtime(&FileModTime);
      Buf[0] = Buf[5] = Buf[10] = bin2bcd(tp->tm_year % 100);
      Buf[1] = Buf[6] = Buf[11] = bin2bcd(tp->tm_mon+1);
      Buf[2] = Buf[7] = Buf[12] = bin2bcd(tp->tm_mday);
      Buf[3] = Buf[8] = Buf[13] = bin2bcd(tp->tm_hour);
      Buf[4] = Buf[9] = Buf[14] = bin2bcd(tp->tm_min);
      bdos(CPMSDMA,Buf);
      luid = getuid();
      setuid(fc.uid);
      bdos(CPMSSTP, &fc);
      setuid(luid);
   }
}

static time_t getmodtime(fcbp)
struct fcb *fcbp;
{
   int olduser;
   unsigned char *p;
   struct tm ftm;
   time_t tt = 0;
   
   if (ZsDos) {   
      bdos(CPMSDMA,CPMBUF);        /* set dma address */
      olduser = getuid();
      setuid(fcbp->uid);
      bdos(CPMGSTP, fcbp);
      p = (unsigned char *)(CPMBUF+10);
      ftm.tm_year = bcd2bin(*p);
      if (ftm.tm_year < 78) {
         ftm.tm_year += 100;
      }
      ++p;
      ftm.tm_mon  = bcd2bin(*p) - 1;
      ++p;
      ftm.tm_mday = bcd2bin(*p);
      ++p;
      ftm.tm_hour = bcd2bin(*p);
      ++p;
      ftm.tm_min  = bcd2bin(*p);
      ftm.tm_sec  = 0;
      tt = mktime(&ftm);
      setuid(olduser);
   }
   return tt;
}

static unsigned filelength(fcbp)
struct fcb *fcbp;
{
   int olduser;

   bdos(CPMSDMA,CPMBUF);        /* set dma address */
   olduser = getuid();          /* save this user number */
   setuid(fcbp->uid);           /* go to file's user no. */
   bdos(CPMCFS,fcbp);           /* compute file size */
   setuid(olduser);             /* restore original */
   return (fcbp->ranrec[1]<<8)|fcbp->ranrec[0];
}

void fstat(fname,status)
char *fname;
struct stat *status;
{
   setfcb(&Thefcb,fname);
   status->modtime = getmodtime(&Thefcb);
   status->records = filelength(&Thefcb);
   getfirst(fname);
   setfcb(&Thefcb,"????????.???");
}

/*          End of MAIN module             */
