/*************************************************************************/
/*									 */
/*		Configuration Overlay for ZMP - Module 2		 */
/*									 */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"

extern char *alloc(int);
extern int allocerror(void *);
extern int chrin();
extern void cls();
extern int getline(char *,unsigned);
extern void stndout();
extern void stndend();
extern void locate(int,int);
extern void addu(char *,int,int);
extern void wrerror(char *);

static int cloadnos();
static void cshownos();

void setparity()
{
   int c;

   cls();
   do {
      printf("(N)o parity, (O)dd parity, or (E)ven parity?  ");
      c = chrin();
      if (c>='a' && c<='z') {
         c = c - 'a' + 'A';
      }
   } while (c != 'N' && c != 'O' && c != 'E' && c != ESC);
   if (c=='N' || c=='O' || c=='E') {
      Line.parity = c;
   }
}

void setdatabits()
{
   int c;

   cls();
   do {
      printf("(7) data bits or (8) data bits?  ");
      c = chrin();
   } while (c != '7' && c != '8' && c != ESC);
   if (c=='7' || c=='8') {
      Line.databits = c-'0';
   }
}

void setstopbits()
{
   int c;

   cls();
   do {
      printf("(1) stop bit or (2) stop bits?  ");
      c = chrin();
   } while (c != '1' && c != '2' && c != ESC);
   if (c=='1' || c=='2') {
      Line.stopbits = c-'0';
   }
}

static void cshownos()
{
   int i, j;
   char echo;
   
   cls();
   stndout();
   printf("         NAME                NUMBER           B   P D S E");
   stndend();
   for (i = 0, j = 1; i < 20; i++, j++) {
      locate(i+1,0);
      printf("%c - %s",i+'A',Book[i].name);
      printf(" %s",Book[i].number);
      locate(i+1,44);
      printf("%4u0 %c",Baudtable[Book[i].pbaudindex],
         Book[i].pparity);
      echo = Book[i].echo ? 'H' : 'F';
      printf(" %d %d %c\n",Book[i].pdatabits,Book[i].pstopbits,echo);
   }
}

static int cloadnos()
{
   int i,result;
   char dummy;
   FILE *fd;
   unsigned amount;

   result = NERROR;
   amount = 21 * sizeof(struct phonebook);
   Book = (struct phonebook *)alloc(amount);
   if (!allocerror(Book)) {
      strcpy(Pathname,Phonefile);
      addu(Pathname,Overdrive,Overuser);
      fd = fopen(Pathname,"r");
      if (fd!=NULL) {
         for (i = 0; i < 20; i++) {
            fgets(Book[i].name,17,fd);
            fscanf(fd,"%c %s %d %c %d %d %d",
               &dummy,
               Book[i].number,
               &Book[i].pbaudindex,
               &Book[i].pparity,
               &Book[i].pdatabits,
               &Book[i].pstopbits,
               &Book[i].echo);
            fgetc(fd);   /* remove LF */
         }
         fclose(fd);
         result = OK;
      }
   }
   return result;
}      

void phonedit()
{
   int i, c, change;
   char *answer;

   cloadnos();
   answer = Pathname;
   for (;;) {
      flush();
      cshownos();
      printf("\nEnter letter of phone number to change/enter,\n");
      printf("or anything else to EXIT:  ");
      c = chrin();
      if (c>='a' && c<='z') {
         c = c - 'a' + 'A';
      }
      c = c - 'A';
      if (c < 0 || c > 20) {
         break;
      }
      change = TRUE;
      flush();
      printf("\n\t  Name:  %s\nEnter new name:  ",
         Book[c].name);
      if (getline(answer,18)) {
         while (strlen(answer) < 17) {
            strcat(answer," "); /* Pad with spaces */
         }
         strcpy(Book[c].name,answer);
      }
      printf("\n\t  Number:  %s\nEnter new number:  ",
         Book[c].number);
      if (getline(answer,18)) {
         strcpy(Book[c].number,answer);
      }
      printf("\n\t  Bit rate:  %u0\nEnter new bit rate:  ",
         Baudtable[Book[c].pbaudindex]);
      if (getline(answer,18)) {
         for (i = 0; i < 13; i++) {
            if (atoi(answer)/10 == Baudtable[i]) {
               Book[c].pbaudindex = i;
               break;
            }
         }
      }
      printf("\n\t  Parity:  %c\nEnter new parity:  ",
         Book[c].pparity);
      if (getline(answer,18)) {
         c = answer[0];
         if (c>='a' && c<='z') {
            c = c - 'a' + 'A';
         }
         Book[c].pparity = c;
      }
      printf("\n    Nr data bits:  %d\nEnter new number:  ",
         Book[c].pdatabits);
      if (getline(answer,18)) {
         Book[c].pdatabits = atoi(answer);
      }
      printf("\n    Nr stop bits:  %d\nEnter new number:  ",
         Book[c].pstopbits);
      if (getline(answer,18)) {
         Book[c].pstopbits = atoi(answer);
      }
      printf("\n\t\tDuplex:  %s\nEnter (H)alf or (F)ull:  ",
         Book[c].echo?"Half":"Full");
      if (getline(answer,18)) {
         c = answer[0];
         Book[c].echo = (c == 'H') || (c == 'h');
      }
   }
   flush();
   cls();
   if (Book != (struct phonebook *)MEMORY_FULL) {
      free(Book);
   }
}

void ldedit()
{
   char *p, *answer;
   int c;

   answer = Pathname;
   for (;;) {
      cls();
      printf("\r\t\t\t");
      stndout();
      printf(" LONG DISTANCE ACCESS CODE ");
      stndend();
      printf("\n\nEnter access code to edit:\n\n");
      printf(
         "  + (currently '%s')\n  - (currently '%s')\n\tor Z to exit: ",
         Sprint,Mci);
      c = chrin();
      if (c=='+') {
         p = Sprint;
      } else if (c=='-') {
         p = Mci;
      } else if (c==ESC || c=='Z' || c=='z') {
            return;
      }
      if (c=='+' || c=='-') {
         printf("\nEnter new code: ");
         if (getline(answer,20)) {
            strcpy(p,answer);
         }
      }
   }
}

void edit()
{
   int i;
   char *buffer;
   char key;

   buffer = Pathname;
   do {
      cls();
      flush();
      printf("\r\t\t");
      stndout();
      puts(" KEYPAD MACRO LIST \n");
      stndend();
      for (i=0; i<10; i++) {
         printf("%d - %s\n",i,KbMacro[i]);
      }
      printf("\nPress key of macro to edit, esc to quit:  ");
      key = (char)chrin();
      if (isdigit(key)) {
         i = key-'0';
         flush();
         puts("\nIf you want the macro to end with a RETURN,");
         puts("add a '\041' to the end of your entry (20 chars. max).\n");
         printf("Old Macro:  ");
         puts(KbMacro[i]);
         printf("\nNew Macro:  ");
         if (getline(buffer,21)) {
            strcpy(KbMacro[i],buffer);
         }
      }
   } while (isdigit(key));
   flush();
}

void savephone()
{
   int i;
   FILE *fd;

   strcpy(Pathname,Phonefile);
   addu(Pathname,Overdrive,Overuser);
   fd = fopen(Pathname,"w");
   if (fd) {
      printf("\nSaving Phone numbers...");
      for (i = 0; i < 20; i++) {
         fprintf(fd,"%s %s %d %c %d %d %d\n",
            Book[i].name,
            Book[i].number,
            Book[i].pbaudindex,
            Book[i].pparity,
            Book[i].pdatabits,
            Book[i].pstopbits,
            Book[i].echo);
      }
      fclose(fd);
      printf("Successful.\n");
   } else {
      wrerror(Phonefile);
   }
}

void saveconfig()
{
   int i;
   FILE *fd;

   strcpy(Pathname,Cfgfile);
   addu(Pathname,Overdrive,Overuser);
   fd = fopen(Pathname,"w");
   if (fd) {
      printf("\n\nSaving Configuration...");
      fprintf(fd,"%d %d %d %d %d\n",Crcflag,Wantfcs32,
         XonXoff,Filter,ParityMask);
      for (i = 0; i < 10; i++)
         fprintf(fd,"%s\n",KbMacro[i]);
      fprintf(fd,"%s\n%s\n",Mci,Sprint);
      fprintf(fd,"%s\n%s\n%s\n",Modem.init,Modem.dialcmd,
         Modem.dialsuffix);
      fprintf(fd,"%s\n%s\n%s\n",Modem.connect,Modem.busy1,
         Modem.busy2);
      fprintf(fd,"%s\n%s\n%s\n",Modem.busy3,Modem.busy4,
         Modem.hangup);
      fprintf(fd,"%d %d\n",Modem.timeout,Modem.pause);
      fprintf(fd,"%d %c %d %d\n",Line.baudindex,Line.parity,
         Line.databits,Line.stopbits);
      fprintf(fd,"%d %u %c %d %d\n",Zrwindow,Pbufsiz,Maxdrive,
         Chardelay,Linedelay);
      fclose(fd);
      printf("Successful.\n");
   } else {
      wrerror(Cfgfile);
   }
}

/************************* END OF ZMCONFIG MODULE 2 *************************/
