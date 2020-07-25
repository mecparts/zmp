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
extern void stndout();
extern void stndend();
extern void locate(int,int);
extern void addu(char *,int,int);

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

void cshownos()
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

int cloadnos()
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

/************************* END OF ZMCONFIG MODULE 2 *************************/
