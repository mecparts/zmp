/*************************************************************************/
/*									 */
/*		Configuration Overlay for ZMP - Module 3		 */
/*									 */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"

extern int chrin();
extern void cls();
extern int getline(char *,unsigned);
extern void addu(char *,int,int);
extern void wrerror(char *);
extern int cloadnos();
extern void cshownos();

void phonedit()
{
   int i, c, change;
   char *answer,pad[2];

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
         pad[1] = '\000';
         if (answer[0] == '_') {
            pad[0] = '_';
         } else {
            pad[0] = ' ';
         }
         while (strlen(answer) < 17) {
            strcat(answer,pad); /* Pad with spaces or underlines */
         }
         strcpy(Book[c].name,answer);
      }
      if (answer[0] != '_') {
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
            Book[c].pparity = toupper(answer[0]);
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
      } else {
         strcpy(Book[c].number,"_________________");
         Book[c].pbaudindex = 5;   /* default to 1200 baud */
         Book[c].pparity = 'N';
         Book[c].pdatabits = 8;
         Book[c].pstopbits = 1;
         Book[c].echo = FALSE;
     }
   }
   flush();
   cls();
   if (Book != (struct phonebook *)MEMORY_FULL) {
      free(Book);
   }
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

/************************* END OF ZMCONFIG MODULE 3 *************************/
