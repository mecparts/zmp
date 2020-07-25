/*************************************************************************/
/*									 */
/*		Configuration Overlay for ZMP - Module 4		 */
/*									 */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>
#include <time.h>
#include "zmp.h"

extern int chrin();
extern void cls();
extern int getline(char *,unsigned);
extern void stndout();
extern void stndend();
extern void addu(char *,int,int);
extern void wrerror(char *);

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

void saveconfig()
{
   int i;
   FILE *fd;

   strcpy(Pathname,Cfgfile);
   addu(Pathname,Overdrive,Overuser);
   fd = fopen(Pathname,"w");
   if (fd) {
      printf("\n\nSaving Configuration...");
      fprintf(fd,"%d %d %d %d %d %d\n",Crcflag,Wantfcs32,
         XonXoff,Filter,ParityMask,time_zone);
      for (i = 0; i < 10; i++)
         fprintf(fd,"%s\n",KbMacro[i]);
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

/************************* END OF ZMCONFIG MODULE 4 *************************/
