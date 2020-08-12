/*         TERM module File 2            */

#include <stdio.h>
#include <unixio.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"

extern int openerror(int,char *,int);
extern int fopenerror(FILE *,char *);
extern int getch();
extern int dio();
extern int allocerror(void*);
extern int getline(char *,unsigned);
extern int chrin();
extern int readline(int);

void printfile();
void viewfile();
int possdirectory(char*);

/* Display current keyboard macros */
keydisp()
{
   short i;

   cls();
   printf("\r\t\t");
   stndout();
   printf(" KEYPAD MACRO LIST \n\n");
   stndend();
   for (i=0; i<10; i++)
      printf("\t%d - %s\n",i,KbMacro[i]);
   printf("\n\n");
}

/* Save a buffer to a file. If flag set, send ctl-s/ctl-q to stop remote */
keep(filename,flag)
char *filename;
short flag;
{
   short fl;

   if (!BFlag) {
      return;
   }
   if (!TxtPtr) {
      goto cleanup;
   }
   if (flag) {
      mcharout(CTRLS);
   }
   while (TxtPtr % 128) {
      MainBuffer[TxtPtr++] = 0;
   }
   strcpy(Pathname,filename);
   addu(Pathname,Invokdrive,Invokuser);
   if ((fl = open(Pathname,1)) == UBIOT) {
      if ((fl = creat(Pathname,0)) == UBIOT) {
         openerror(fl,Pathname,UBIOT);
      }
   }
   if (fl != UBIOT) {
      lseek(fl,0L,2);       
      write(fl,MainBuffer,TxtPtr);
      close(fl);
   }
   if (flag) {
      mcharout(CTRLQ);
   }
   TxtPtr = 0;

cleanup:
   free(MainBuffer);
}

startcapture()     /* allocate capture buffer */
{
   char *grabmem();

   if (!BFlag)
      return;
   MainBuffer = grabmem(&Bufsize);
   Buftop = Bufsize - 1;
   TxtPtr = 0;

#ifdef DEBUG
   printf("\ncapture Bufsize = %u\n",Bufsize);
#endif

}

/* Allow user to change things */
void docmd()
{
   char c,parity,databits,stopbits;
   int i,oldfdx,oldremecho,oldbaud;
   unsigned baud;

   for (;;) {
      cls();
      printf("\t\t");
      stndout();
      printf(" CHANGE LINE PARAMETERS \n");
      stndend();
      printf("\n\tB - Change Baud Rate (now %u0)\n",
         Baudtable[Current.cbaudindex]);
      printf("\tC - Set Checksum/CRC default for X/Ymodem: now %s\n",
         Crcflag ? "CRC" : "Checksum" );
      printf("\tD - Set Half/Full Duplex/Echo\n");
      printf("\tF - Toggle Control Character Filter: now %s\n",
         Filter ? "ON" : "OFF");
      printf("\tH - Toggle 32-bit FCS request in Zmodem: now %s\n",
         Wantfcs32 ? "ON" : "OFF");
      printf("\tI - Initialise Modem\n");
      /* ### printf("\tO - Switch Port: now %c\n", Port); */
      printf("\tP - Handle Parity Bit in Terminal Mode: now %sed\n",
         ParityMask ? "Stripp" : "Pass");
      printf("\tU - Set up UART: Parity, Stop Bits, Data Bits\n");
      /* ### printf("\tW - Toggle Word Wrap in Terminal Mode: now %s\n",
         WordWrap ? "ON": "OFF"); */
      printf("\tX - Toggle Xon-Xoff Protocol: now %s\n", XonXoff ? "ON" : "OFF");
      printf("\tZ - Exit\n");
      printf("\n Select: ");
      c = chrin();
      c = toupper(c);
      switch (c) {
   
         case 'B' :
            do {
               printf("\nEnter new baud rate: ");
               scanf("%d",&baud);
               if (baud==-7936) {
                 baud = 5760;
               } else if (baud==11264) {
                  baud = 7680;
               } else {
                  baud /= 10;
               }
               for (i = 0; i < 13; i++) {
                  if (Baudtable[i] == baud) {
                     break;
                  }
               }
            } while (i > 12); /* Don't leave till it's right */
            oldbaud = Current.cbaudindex;
            Current.cbaudindex = i;
            updateace();

            /* error if change is intended but rejected */
            if ((*Mspeed == oldbaud) && (i != oldbaud)) {
               Current.cbaudindex = oldbaud;
               printf("\nBaud rate %u0 not available.\n",baud);
               waitakey();
            }
            break;

         case 'C':
            Crcflag = !Crcflag;
            break;
            
         case 'D':
            oldfdx = FDx;   /* Save original */
            oldremecho = RemEcho;
            FDx = TRUE;
            RemEcho = FALSE;
            printf("\n<F>ull Duplex, <H>alf Duplex or Remote <E>cho? ");
            c = chrin();
            c = toupper(c);
            switch (c) {

               case 'H' :
                  FDx = FALSE;
                  break;

               case 'E' :
                  FDx = FALSE;
                  RemEcho = TRUE;
                  break;

               case 'F' :
                  break;

               default  :
                  FDx = oldfdx;   /* no change */
                  RemEcho = oldremecho;
                  break;
            }
            break;

         case 'F':
            Filter = !Filter;
            break;

         case 'H':
            Wantfcs32 = !Wantfcs32;
            break;
            
         case 'I':
            initace(Current.cbaudindex,Current.cparity,
               Current.cdatabits,Current.cstopbits);
            mstrout("\r\r",FALSE);
            mstrout(Modem.init,FALSE);
            while (readline(10) != TIMEOUT);
            break;

         case 'O':
            /* ### Port ^= 0x03; */
            break;

         case 'P':
            ParityMask = !ParityMask;
            break;

         case 'U':
            do {
               printf("\nEnter new parity (E, O, N): ");
               parity = chrin();
               parity = toupper(parity);
            } while ((parity != 'E') && (parity != 'O')
                  && (parity != 'N'));
            Current.cparity = parity;
            do {
               printf("\nEnter number of data bits (7,8): ");
               databits = chrin();
            } while ((databits != '7') && (databits != '8'));
            Current.cdatabits = databits - '0';
            do {
               printf("\nEnter number of stop bits (1,2): ");
               stopbits = chrin();
            } while ((stopbits != '1') && (stopbits != '2'));
            Current.cstopbits = stopbits - '0';
            updateace();
            break;

         case 'W':
            break;

         case 'X':
            XonXoff = !XonXoff;
            break;

         case ESC:
         case 'Z':
            return;

         default:
            break;

      }      /* End of main switch */
   }         /* Loop till esc or Z */
}

capturetog(filename)
char *filename;
{
   if (!BFlag) {
      BFlag = TRUE;
      startcapture();
      if (allocerror(MainBuffer)) {
         BFlag = FALSE;
      } else {
         printf("\nEnter capture filename (cr = %s): ",filename);
         if (getline(Buf,16)) {
            strcpy(Lastlog,Buf);
         } else {
            strcpy(Lastlog,filename);
         }
         printf("\nCapture ON\n");
      }
   }
   else {
      keep(Lastlog,FALSE);   /* assume remote not running */
      BFlag = FALSE;
      printf("\nCapture OFF\n");
   }      
}

diskstuff()
{
   static int c,drive,user;
   char newname[20];
   char q, *j;

   for (;;) {
      cls();
      printf("\r\t\t");
      stndout();
      printf(" FILE AND DISK COMMANDS \n");
      stndend();
      printf("\n\tC - Change disk in default drive\n");
      printf("\tD - Directory of current disk\n");
      printf("\tE - Erase file on default drive\n");
      printf("\tF - Change default name of capture file (currently %s)\n",
         Logfile);
      printf("\tL - Log into new du: (currently %c%d:)\n",Currdrive,Curruser);
      printf("\tP - Print a file on default drive\n");
      printf("\tR - Rename a file on default drive\n");
      printf("\tV - View a file on default drive\n");
      printf("\tZ - Exit\n");
      printf("\n Select: ");
      flush();
      c = chrin();
      c = toupper(c);
      switch (c) {

   case 'C':
      printf("\nChange disk in %c: then press any key...",Currdrive);
      flush();
      chrin();
      reset(Currdrive,Curruser);
      break;

   case 'D':
      directory();
      waitakey();
      break;

   case 'E':
      if (!possdirectory("Erase"))
         break;
      printf("\nAre you sure? (Y/N) <N>  ");
      flush();
      c = chrin();
      c = toupper(c);
      if (c == 'Y')
         unlink(Pathname);
      break;

   case 'F':
      if (!BFlag) {
         printf("\nEnter new filename: ");
         scanf("%s",Logfile);
      }
      else {
         printf("\nNot while capture on!\n");
         waitakey();
      }
      break;

   case 'L':
      printf("\nEnter the new default du:  ");
      flush();
      if (!getline(Pathname,10))
         break;
      drive = Currdrive;
      user = Curruser;
      j = Pathname;
      q = j[0];
      q = toupper(q);
      if (isalpha(q)) {
         drive = q;
         j++;
      }
      if (isdigit(q = j[0]))
         user = q - '0';
      if (isdigit(q = j[1]))
         user = user * 10 + q - '0';
      reset(drive,user);
      break;

   case 'P':
      if (!possdirectory("Print"))
         break;
      addu(Pathname,Currdrive,Curruser);
      printfile(Pathname);
      break;

   case 'R':
      if (!possdirectory("Rename"))
         break;
      flush();
      printf("\nNew name for %s: ",Pathname);
      if (!getline(newname,16))
         break;
      rename(Pathname,newname);
      break;

   case 'V':
      if (!possdirectory("View"))
         break;
      addu(Pathname,Currdrive,Curruser);
      cls();
      viewfile(Pathname);
      waitakey();
      break;

   case 'Z':
   case ESC:
      return;

   default:
      break;
      }
   }
}

/* Prompt user and possibly give directory */
possdirectory(prompt)
char *prompt;
{
   short x;

   do {
      printf("\n%s which file (CR = quit, ? = directory)? ",prompt);
      if (!getline(Pathname,16))
         return FALSE;
      if (x = (Pathname[0] == '?'))
         directory();
   }
   while (x);
   return TRUE;
}

help()
{
   int c;

   cls();
   printf("\r\t\t\t\t");
   stndout();
   printf(" ZMP HELP \n\n");
   stndend();
   strcpy(Pathname,"ZMP.HLP");
   addu(Pathname,Overdrive,Overuser);
   viewfile(Pathname);
   printf("\nEnter function (cr to abort): ");
   return ((c = dio()) == CR ? 0 : c);
}

/* View a file set up in Pathname */
void viewfile()
{
   int i;
   char c, kbdata;
   FILE *fd;

   fd = fopen(Pathname,"rb");   /* Use binary or it ignores CR */
   if (fopenerror(fd,Pathname)) {
      return;
   }
   for (;;) {
      for (i = 0; (i < 21) && ((c=getc(fd)) != EOF) && (c != CTRLZ); ) {
         if (c != LF) {
            putchar(c);
         }
         if (c == CR) {      /* This will even print TRS80 files */
            putchar(LF);
            ++i;
         }
      }
      if ((c == EOF) || (c == CTRLZ)) {
         break;
      }
      printf("\n\n Typing %s - ",Pathname);
      flush();
      if (((kbdata = chrin()) == CTRLC) || (kbdata == CTRLX)) {
         break;
      }
      printf("\n");
   }
   fclose(fd);
}

/* Print a file set up in Pathname */
void printfile()
{
   char c;
   FILE *fd;

   fd = fopen(Pathname,"rb");   /* Use binary or it ignores CR */
   if (fopenerror(fd,Pathname)) {
      return;
   }
   printf("\nAny key to abort..\n");
   while (((c=getc(fd)) != EOF) && (c != CTRLZ)
      && !(bdos(CPMDCIO,0xFF))) {
      if (c != LF) {
         bdos(CPMWLST,c);   /* list output */
      }
      if (c == CR) {      /* This will even print TRS80 files */
         bdos(CPMWLST,LF);
      }
   }
   fclose(fd);
}

/*         End of TERM module File 2         */
