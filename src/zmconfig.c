/*************************************************************************/
/*									 */
/*		Configuration Overlay for ZMP - Module 1		 */
/*									 */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"

extern void cls();
extern void stndout();
extern void etndend();
extern int getline(char *,unsigned);
extern int chrin();
extern void setparity();
extern void setdatabits();
extern void setstopbits();
extern void phonedit();
extern void edit();
extern void savephone();
extern void saveconfig();

extern int time_zone;

static void settransfer();
static void setsys();
static void setmodem();
static void gnewint(char *, int *);
static void gnewstr(char *, char *, short);
static void setline();
static void setbaud();
static int goodbaud(unsigned);

ovmain()
{
   int c, cfgchanged, phonechanged;

   cfgchanged = phonechanged = FALSE;

   for (;;) {
      cls();
      printf("\r\t\t");
      stndout();
      printf(" CONFIGURATION MENU ");
      stndend();

      printf("\n\n\tK - Edit keyboard macros\n");
      printf("\tL - Set line parameters\n");
      printf("\tM - Set modem parameters\n");
      printf("\tP - Edit phone number list\n");
      printf("\tS - Set system parameters\n");
      printf("\tT - Set file transfer parameters\n");
      printf("\tZ - Exit\n");
      printf("\n   Select:  ");
      flush();
      c = chrin();
      c = toupper(c);
      switch (c) {

         case 'K':
            edit();
            cfgchanged = TRUE;
            break;

         case 'L':
            setline();
            cfgchanged = TRUE;
            break;

         case 'M':
            setmodem();
            cfgchanged = TRUE;
            break;

         case 'P':
            phonedit();
            phonechanged = TRUE;
            break;

         case 'S':
            setsys();
            cfgchanged = TRUE;
            break;

         case 'T':
            settransfer();
            cfgchanged = TRUE;
            break;

         case ESC:
         case 'Z':
            if (cfgchanged || phonechanged) {
               printf("\nMake changes permanent? ");
               c = chrin();
               c = toupper(c);
               if (c == 'Y') {
                  if (cfgchanged) {
                     saveconfig();
                  }
                  if (phonechanged) {
                     savephone();
                  }
                  kbwait(2);
               }
            }
            cls();
            return;      /* Return from overlay */

         default:
            break;
   
      }         /* end of switch */
   }
}

static void settransfer()
{
   int c;

   for (;;) {
      cls();
      printf("\r\t\t\t");
      stndout();
      printf(" FILE TRANSFER PARAMETERS ");
      stndend();
      printf("\n\n\tC - Set Checksum/CRC default - now %s\n",
         Crcflag?"CRC":"Checksum");
      printf("\tD - Set delay after each character in ASCII send - now %d mS\n",
         Chardelay);
      printf("\tF - Toggle 32-bit FCS capability - now %s\n",
         Wantfcs32?"Enabled":"Disabled");
      printf("\tL - Set delay after each line in ASCII send - now %d mS\n",
         Linedelay);
      printf("\tW - Change Zmodem receive window size - now %d\n",Zrwindow);
      printf("\tX - Toggle X-on/X-off protocol - now %s\n",
         XonXoff?"Enabled":"Disabled");
      printf("\tZ - Exit\n\n");
      printf("   Select:  ");
      c = chrin();
      c = toupper(c);
      putchar('\n');
      switch (c) {
            case 'C':
            Crcflag = !Crcflag;
            break;
   
         case 'D':
            gnewint("character delay",&Chardelay);
            break;
   
         case 'F':
            Wantfcs32 = !Wantfcs32;
            break;
   
         case 'L':
            gnewint("line delay",&Linedelay);
            break;
   
         case 'W':
            gnewint("window size",&Zrwindow);
            break;
   
         case 'X':
            XonXoff = !XonXoff;
            break;
   
         case ESC:
         case 'Z':
            return;
   
         default:
            break;
      }
   }
}

static void setsys()
{
   int c;
   char d;

   for (;;) {
      cls();
      printf("\r\t\t\t");
      stndout();
      printf(" SYSTEM PARAMETERS ");
      stndend();
      printf("\n\n\tB - Set print buffer size - now %d bytes\n",Pbufsiz);
      printf("\tF - Toggle T-mode control character filter - now %s\n",
         Filter ? "ON" : "OFF");
      printf("\tM - Set maximum drive on system - now %c:\n",Maxdrive);
      printf("\tP - Toggle T-mode parity bit removal - now %s\n",
         ParityMask ? "ON" : "OFF");
      printf("\tT - Set minutes west of UTC - now %d\n",time_zone);
      printf("\tZ - Exit\n\n");
      printf("   Select:  ");
      c = chrin();
      c = toupper(c);
      switch (c) {

         case 'B':
            gnewint("print buffer size",&Pbufsiz);
            Pbufsiz = Pbufsiz < 1 ? 512 : Pbufsiz;
            break;

         case 'F':
            Filter = !Filter;
            break;

         case 'M':
            printf("\n\nEnter new maximum drive: ");
            d = chrin();
            d = toupper(d);
            Maxdrive = ((d >= 'A') && (d <= 'P')) ? d : 'B';
            break;

         case 'P':
            ParityMask = !ParityMask;
            break;

         case 'T':
            gnewint("minutes west of UTC",&time_zone);
            break;

         case ESC:
         case 'Z':
            return;

         default:
            break;
      }
   }
}

static char *Mdmstring[] = {
   "Modem init string.....",
   "Dialling command......",
   "Dial command suffix...",
   "Connect string........",
   "No Connect string 1...",
   "No Connect string 2...",
   "No Connect string 3...",
   "No Connect string 4...",
   "Hangup string.........",
   "Redial timeout delay..",
   "Redial pause delay...."
};

static void setmodem()
{
   int c;

   for (;;) {
      cls();
      printf("\r\t\t\t");
      stndout();
      printf(" MODEM PARAMETERS ");
      stndend();
      printf("\n\n\tA - %s%s\n",Mdmstring[0],Modem.init);
      printf("\tB - %s%s\n",Mdmstring[1],Modem.dialcmd);
      printf("\tC - %s%s\n",Mdmstring[2],Modem.dialsuffix);
      printf("\tD - %s%s\n",Mdmstring[3],Modem.connect);
      printf("\tE - %s%s\n",Mdmstring[4],Modem.busy1);
      printf("\tF - %s%s\n",Mdmstring[5],Modem.busy2);
      printf("\tG - %s%s\n",Mdmstring[6],Modem.busy3);
      printf("\tH - %s%s\n",Mdmstring[7],Modem.busy4);
      printf("\tI - %s%s\n",Mdmstring[8],Modem.hangup);
      printf("\tJ - %s%d\n",Mdmstring[9],Modem.timeout);
      printf("\tK - %s%d\n",Mdmstring[10],Modem.pause);
      printf("\tZ - Exit\n\n");
      printf("   Select:  ");
      c = chrin();
      c = toupper(c);
      putchar('\n');
      switch (c) {

         case 'A':
            gnewstr(Mdmstring[0],Modem.init,40);
            break;

         case 'B':
            gnewstr(Mdmstring[1],Modem.dialcmd,20);
            break;

         case 'C':
            gnewstr(Mdmstring[2],Modem.dialsuffix,20);
            break;

         case 'D':
            gnewstr(Mdmstring[3],Modem.connect,20);
            break;

         case 'E':
            gnewstr(Mdmstring[4],Modem.busy1,20);
            break;

         case 'F':
            gnewstr(Mdmstring[5],Modem.busy2,20);
            break;

         case 'G':
            gnewstr(Mdmstring[6],Modem.busy3,20);
            break;

         case 'H':
            gnewstr(Mdmstring[7],Modem.busy4,20);
            break;

         case 'I':
            gnewstr(Mdmstring[8],Modem.hangup,20);
            break;

         case 'J':
            gnewint(Mdmstring[9],&Modem.timeout);
            break;

         case 'K':
            gnewint(Mdmstring[10],&Modem.pause);
            break;

         case ESC:
         case 'Z':
            return;

         default:
            break;
      }
   }
}

static void gnewint(prompt,intp)
char *prompt;
int *intp;
{
   static char *temp;

   temp = Pathname;
   printf("\n\nEnter new %s:  ",prompt);
   if (getline(temp,20)) {
      *intp = atoi(temp);
   }
}

static void gnewstr(prompt,mstring,length)
char *prompt, *mstring;
short length;
{
   char *temp;

   temp = Pathname;
   printf("\n\nEnter new %s:  ",prompt);
   if (getline(temp,length)) {
      strcpy(mstring,temp);
   }
}

static void setline()
{
   int c;

   for (;;) {
      cls();
      printf("\r\t\t\t");
      stndout();
      printf(" LINE PARAMETERS ");
      stndend();
      printf("\n\n\tB - Bits per second.......%u0\n",
         Baudtable[Line.baudindex]);
      printf("\tD - Number data bits......%d\n",Line.databits);
      printf("\tP - Parity................%c\n",Line.parity);
      printf("\tS - Number stop bits......%d\n",Line.stopbits);
      printf("\tZ - Exit\n\n");
      printf("   Select:  ");
      c = chrin();
      c = toupper(c);
      cls();
      switch (c) {
         case 'B':
            setbaud();
            break;

         case 'D':
            setdatabits();
            break;

         case 'P':
            setparity();
            break;

         case 'S':
            setstopbits();
            break;

         case ESC:
         case 'Z':
            return;

         default:
            break;
      }
   }
}

static void setbaud()
{
   unsigned baud;
   char *buffer;

   buffer = Pathname;
   do {
      printf("\nEnter default modem bit rate:  ");
      if (!(getline(buffer,6)))
         break;
      baud = (unsigned)(atol(buffer)/10);
      printf("\n");
   } while (!goodbaud(baud));
}

static int goodbaud(value)
unsigned value;
{
   int i;
   
   for (i = 0; i < 14; i++) {
      if (value == Baudtable[i]) {
         Line.baudindex = i;
         return TRUE;
      }
   }
   printf("\nInvalid entry\n");
   wait(1);
   return FALSE;
}

/********************** END OF ZMCONFIG MODULE 1 ****************************/
