/********************** START OF XFER MODULE 5 ******************************/

#include <stdio.h>
#include <unixio.h>
#include <string.h>
#include <cpm.h>
#include "zmp.h"

/* Test if file exists, rename to .BAK if so */
void testexist(filename)
char *filename;
{
   int fd;
   char *p, newfile[20], *index();

   if ((fd = open(filename,0)) != UBIOT) {
      close(fd);
      strcpy(newfile,filename);
      if (p = index(newfile,'.'))
         *p = '\0';   /* stop at dot */
      strcat(newfile,".bak");
      unlink(newfile);   /* remove any .bak already there */
      rename(filename,newfile);
   }
}

/************************** END OF MODULE 5 *********************************/
