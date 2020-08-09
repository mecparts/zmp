/*         TERM module File #4            */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"

extern int getch();
extern char *alloc(int);
extern int allocerror(void *);
extern int readstr(char *,int);
extern int readline(int);
extern int getline(char *, unsigned);
extern int kbwait(unsigned);
extern int isin(char *, char *);

static int shownos();
static int loadnos();

void setace(n)   /* for a particular phone call */
int n;
{
   Current.cbaudindex = Book[n].pbaudindex;
   Current.cparity = Book[n].pparity;
   Current.cdatabits = Book[n].pdatabits;
   Current.cstopbits = Book[n].pstopbits;
   updateace();
}

int gettimedchar(seconds)
unsigned seconds;
{
   unsigned t;
   int c;

   t = seconds * 10;
   while(!(c=getch()) && (t--)) {
      mswait(100);
   }
   return c ? c : TIMEOUT;
}


int dial()
{
   static char *number;         /* buffer for number to be sent to modem */
   static char *result;         /* buffer for responses from modem */
   static char *instr;          /* buffer for numbers entered at keyboard */
   static int connect;
   static int status, i, j, n, nocnt, action, c;
   static char *p;

   if (allocerror(number = alloc(128))) {
      return;
   }
   if (allocerror(result = alloc(128))) {
      free(number);
      return;
   }
   if (allocerror(instr = alloc(128))) {
      free(number);
      free(result);
      return;
   }
   QuitFlag = connect = FALSE;
   Dialing = TRUE;
   status = gettimedchar(5);
   if (status != TIMEOUT) {
      instr[0] = status;
      instr[1] = '\000';
      j = 1;
      status = loadnos();
   } else {
      status = shownos();
      printf("\nEnter letters and/or numbers, separated by commas..\n: ");
      j=getline(instr,80);
   }
   if (j) {
      putlabel("Automatic Redial:  Press ESC to stop");
      for (i=0,nocnt=1; instr[i]; i++)
         if (instr[i] == ',') {
            instr[i] = 0;
            nocnt++;
         }
      i = nocnt;
      for (;;) {
         p = instr;
         nocnt = i;
         while (nocnt--) {
            n = -1;
            strcpy(number,Modem.dialcmd);
            if ((status == OK) && (j=strlen(p))==1) {
               n = *p;
               if (isalpha(n)) {
                  n = toupper(n) - 'A';
                  p = Book[n].number;
                  if (*p != '_') {
                     setace(n);
                     strcat(number,p);
                     strcat(number,Modem.dialsuffix);
                     mstrout(number,FALSE);
                     p = Book[n].name;
                     printf("\nDialing %s...", p);
                  } else {
                     printf("\nInvalid Number\n");
                     goto abort;
                  }
               } else {
                  printf("\nInvalid Number\n");
                  goto abort;
               }
            } else {
               strcat(number,p);
               strcat(number,Modem.dialsuffix);
               mstrout(number,FALSE);
               printf("\nDialing %s...",p);
            }
            /* flush modem input */
            while (readline(10) != TIMEOUT);
            do {
               action = readstr(result,Modem.timeout);
               if (action == TIMEOUT) {
                  goto abort;
               }
               printf("%s\n",result);
            } while (!(c=isin(result,Modem.connect))
               && !isin(result,Modem.busy1)
               && !isin(result,Modem.busy2)
               && !isin(result,Modem.busy3)
               && !isin(result,Modem.busy4));

            if (c) {      /* got connect string */
               printf("\007\nOn Line to %s\n", p);
               if (n >= 0) {
                  FDx = !Book[n].echo;
               }
               connect = TRUE;
               goto done;
            }
            mcharout(CR);
            /* wait for modem */
            while (readline(10) != TIMEOUT);
            p += j+1;
         }
         if (kbwait(Modem.pause)) {
            goto abort;
         }
      }
   }

abort:
   printf("Call Aborted.\n");
   mcharout(CR);
   readstr(result,1);      /*gobble last result*/
   resetace();

done:
   flush();
   if (Book != (struct phonebook *) MEMORY_FULL) {
      free(Book);
   }
   free(instr);
   free(result);
   free(number);
   Dialing = FALSE;
   return connect;
}

static int shownos()
{
   static int i, j, status;

   cls();
   if ((status=loadnos()) == OK) {
      stndout();
      printf("         NAME                NUMBER          B   P D S E");
      stndend();
      for (i=0,j=1; i<20; i++,j++) {
         locate(i+1,0);
         printf("%c - %s",i+'A',Book[i].name);
         locate(i+1,41-strlen(Book[i].number));
         printf(Book[i].number);
         locate(i+1,44);
         printf("%4u0 %c",
            Baudtable[Book[i].pbaudindex],
            Book[i].pparity);
         printf(" %d %d %c\n",
            Book[i].pdatabits,
            Book[i].pstopbits,
            Book[i].echo ? 'H' : 'F');
      }
   }
   return status;
}

static int loadnos()
{
   static unsigned amount;
   char dummy;
   int i,result;
   FILE *fd;

   result = NERROR;
   amount = 21 * sizeof(struct phonebook);
   Book = (struct phonebook *) alloc(amount);
   if (!allocerror(Book)) {
      strcpy(Pathname,Phonefile);
      addu(Pathname,Overdrive,Overuser);
      fd = fopen(Pathname,"r");
      if (fd) {
         for (i = 0; i < 20; i++) {
            fgets(Book[i].name,17,fd);
            fscanf(fd,"%c %s %d %c",
               &dummy,
               Book[i].number,
               &Book[i].pbaudindex,
               &Book[i].pparity);
            fscanf(fd,"%d %d %d",
               &Book[i].pdatabits,
               &Book[i].pstopbits,
               &Book[i].echo);
            fgetc(fd);      /* remove LF */
         }
         fclose(fd);
         result = OK;
      }
   }
   return result;
}
/*         End of TERM module            */
/*         End of TERM module File 1         */

