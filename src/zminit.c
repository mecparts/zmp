/******************** Initialisation Overlay for ZMP ***********************/

#define  INIT
#include <stdio.h>
#include <string.h>
#include <cpm.h>
#include <time.h>
#include "zmp.h"

extern void userin();
extern void initvect();
extern void initcurs();
extern void showcurs();
extern void cls();
extern void locate(int,int);
extern int ctr(char *);
extern int readline(int);

static void title();
static void getconfig();
static void initializemodem();
static void xfgets(char *,int,FILE *);
static void resetace();

ovmain()
{
   unsigned u;

   userin();   /* perform user-defined entry routine */
   title();
   getconfig();
   initvect();                 /* set up interrupt vector */
   u = (unsigned) *Mspeed;
   if ( !u || u > 13 ) {
      initializemodem();   /* initialise uart as well */
   } else {
      Current.cbaudindex = (int) (*Mspeed);
      Current.cparity = Line.parity;            /* Only update */
      Current.cdatabits = Line.databits;        /*  internal   */
      Current.cstopbits = Line.stopbits;        /*  variables  */
   }
   ZsDos = (bdoshl(CPMGZSD) & 0xFF00)>>8 == 'S';
   Currdrive = Invokdrive;
   Curruser = Invokuser;
   reset(Currdrive,Curruser);
   showcurs();
}


static void title()
{
   static char line1[] = "ZMP - A ZMODEM Program for CP/M";
   static char line3[] = "Developed from HMODEM II";
   static char line4[] = "by Ron Murray";
   static char line5[] = "Updated to Hi Tech C v3.09";
   static char line6[] = "by Wayne Hortensius";

   cls();
   locate(7,ctr(line1));
   printf(line1);
   locate(9,ctr(Version));
   printf(Version);
   locate(10,ctr(line3));
   printf(line3);
   locate(11,ctr(line4));
   printf(line4);
   locate(13,ctr(line5));
   printf(line5);
   locate(14,ctr(line6));
   printf(line6);
   locate(14,0);
   hidecurs();
   flush();
}

/* Initialise the modem */
static void initializemodem()
{
   resetace();
   mstrout("\n\n",FALSE);
   mstrout(Modem.init,FALSE);
   while(readline(10) != TIMEOUT);   /* gobble echoed characters */
}

/* Read the .CFG file into memory */
static void getconfig()
{
   int i;
   FILE *fd;

   strcpy(Pathname,Cfgfile);
   addu(Pathname,Overdrive,Overuser);
   fd = fopen(Pathname,"rb");
   if (fd) {
      fscanf(
         fd,"%d %d %d %d %d %d",&Crcflag,&Wantfcs32,&XonXoff,&Filter,&ParityMask,&time_zone);
      for (i = 0; i < 10; i++) {
         xfgets(KbMacro[i],22,fd);
      }
      xfgets(Modem.init,40,fd);
      xfgets(Modem.dialcmd,8,fd);
      xfgets(Modem.dialsuffix,8,fd);
      xfgets(Modem.connect,20,fd);
      xfgets(Modem.busy1,20,fd);
      xfgets(Modem.busy2,20,fd);
      xfgets(Modem.busy3,20,fd);
      xfgets(Modem.busy4,20,fd);
      xfgets(Modem.hangup,20,fd);
      fscanf(fd,"%d %d",&Modem.timeout,&Modem.pause);
      fscanf(fd,"%d %c %d %d",&Line.baudindex,&Line.parity,
         &Line.databits,&Line.stopbits);
      fscanf(fd,"%d %d %c %d %d",&Zrwindow,&Pbufsiz,&Maxdrive,
         &Chardelay,&Linedelay);
      fclose(fd);
   }
}

/* Read a string from a file and remove the newline characters */
static void xfgets(buf,max,fd)
char *buf;
int max;
FILE *fd;
{
   short noerror = 1;
   char *p, *index();
   char tbuf[81];

   tbuf[0] = '\0';
   while (!strlen(tbuf) && noerror) {
      noerror = (short) fgets(tbuf,80,fd);
      while ( p = index(tbuf,'\12')) {
         strcpy(p, p + 1);
      }
      while ( p = index(tbuf,'\15')) {
         strcpy(p, p + 1);
      }
   }
   strncpy(buf,tbuf,max);
}

static void resetace()  /* to default values */
{
   Current.cparity = Line.parity;
   Current.cdatabits = Line.databits;
   Current.cstopbits = Line.stopbits;
   Current.cbaudindex = Line.baudindex;
   initace(
      Current.cbaudindex,
      Current.cparity,
      Current.cdatabits,
      Current.cstopbits);
}

/* End of initialisation overlay */
