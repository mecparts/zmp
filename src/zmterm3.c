/*         TERM module File 3            */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>
#include "zmp.h"

extern int dio();
extern int getfirst(char *);
extern int getnext();
extern char *alloc(int);
extern int allocerror(void *);

int domore();
void sorted_dir(char *,unsigned);
void unsort_dir();
void updateace();
int printsep(short);
int cntbits(char);

static short Lines, Entries;

void directory()
{
   short factor, cpm3;
   long *lp = (long *) CPMBUF;
   unsigned i, dtotal, atotal, allen, remaining, bls, dirbufsize;
   char *grabmem(), *alloca, *p, *dirbuf;
   struct dpb *thedpb;

   cls();
   sprintf(Buf,"Directory for Drive %c%d:",Currdrive,Curruser);
   putlabel(Buf);
   bdos(CPMSDMA,CPMBUF);         /* set dma address */
   cpm3 = (bdoshl(CPMVERS) >= 0x30);   /* get cp/m version (BCD) */
   dirbuf = grabmem(&dirbufsize);
   if (dirbuf != (char *) MEMORY_FULL) {
      sorted_dir(dirbuf,dirbufsize);
   } else {           /* not enough room */
      unsort_dir();   /* do unsorted directory */
   }

   /* Now print free space on disk */
   if (cpm3) {   /* cp/m 3 */
      bdos(CPMGDFS,Currdrive - 'A');
      p = (char *) (CPMBUF + 3);
      *p = 0;   /* clear hi byte for long */
      remaining = (short) (*lp / 8L);
   } else {      /* cp/m 2.2 */
      thedpb = (struct dpb *) bdoshl(CPMGDPB); /* fill dpb */
      alloca = (char *) bdoshl(CPMGALL);
      bls = 0x0001;
      bls <<= thedpb->bsh + 7;
      factor = bls/1024;
      dtotal = factor * (thedpb->dsm+1);
      allen = (thedpb->dsm / 8) + 1;
      for (atotal = i = 0; i < allen; i++) {
         atotal += cntbits(*(alloca + i));
      }
      atotal *= factor;
      remaining = dtotal - atotal;
   }
   if (Lines >= DIRLINES - 1) {
      domore();
   }
   if (Entries) {
      printf("\n\t%d",Entries);
   } else {
      printf("\n\tNo");
   }
   printf(" File(s).\t\t");
   printf("Space remaining on %c:  %dk\n",Currdrive,remaining);
}

/* Do sorted directory with filesizes */
void sorted_dir(dirbuf,dirbufsize)
char *dirbuf;
unsigned dirbufsize;
{
   short count, limit, dirsort(), dircode, ksize, i;
   unsigned size;
   char filename[15];
   char *p, *q;
   struct sortentry *se;
   struct fcb srcfcb;

   q = dirbuf;
   Lines = 2;
   memset(&srcfcb,0,sizeof(struct fcb));
   memset(srcfcb.name,'?',8);   /* all filenames, all extents */
   memset(srcfcb.ft,'?',3);
   srcfcb.ex = '?';
   limit = dirbufsize / sizeof(struct sortentry); /* how many */
   dircode = bdos(CPMFFST,&srcfcb);   /* search first */
   count = 0;
   while (dircode != -1) {
      p = ((char *) CPMBUF + dircode * 32);
      if ((*(p+10) & 0x80) == 0) { /* don't list $SYS files */
         for (i = 0; i < 16; i++) {
            *q++ = *p++ & ((i > 0 && i < 12) ? 0x7f : 0xff);
         }
         if (count == limit) { /* can't fit them in */
            free(dirbuf);
            unsort_dir();   /* do unsorted directory */
            return;
         }
         ++count;
      }
      dircode = bdos(CPMFNXT,&srcfcb);   /* search next */
   }
   qsort(dirbuf,count,16,dirsort);   /* sort in alpha order */

   /* ok, now print them all */
   se = (struct sortentry *) dirbuf;
   for (i = Entries = 0; i < count; i++) {
      if (!i || memcmp(se, se - 1, 12)) {
         size = se->rc + (se->s2 * 32 + se->ex) * 128;
         ksize = (size / 8) + ((size % 8) ? 1 : 0);
         memcpy(filename,se->name,8);
         filename[8] = '.';
         memcpy(filename + 9,se->type,3);
         filename[12] = '\0';
         printf("%s%4dk",filename,ksize);
         if (printsep(SORTCOLS)) {
            break;
         }
         Entries++;
      }
      se++;
   }
   free(dirbuf);
   if (Entries % SORTCOLS) {
      printf("\n");
   }
}

/* Do unsorted directory */
void unsort_dir()
{
   short dircode, i;
   struct direntry *dp;

   Lines = 2;
   dircode = getfirst("????????.???");
   for (Entries = 0; dircode != 0xff; Entries++) {
      dp = (struct direntry *) (CPMBUF + dircode * 32);
      memcpy(Pathname,dp->flname,8);
      Pathname[8]= '.';
      memcpy(Pathname+9,dp->ftype,3);
      Pathname[12] = '\0';
      for (i = 0; i < 11; i++)   /* remove attributes */
         Pathname[i] = Pathname[i] & 0x7f;
      printf("%s",Pathname);
      if (printsep(UNSORTCOLS)) {
         break;
      }
      dircode = getnext();
   }
   if (Entries % UNSORTCOLS)
      printf("\n");
}

/* Print separator between directory entries. Do [more] if page full */
/* Return TRUE if end of page and ctl-c or ctl-k typed */
int printsep(count)
short count;
{
   if ((Entries % count) == count - 1) {
      printf("\n");
      if (++Lines == DIRLINES) {   /* bump line count */
         Lines = 0;      /* pause if done a page */
         return domore();   /* then do [more] */
      }
   } else {
      printf(" | ");
   }
   return FALSE;
}

/* Print [more] and wait for a key. Return TRUE if user hit ctl-c or ctl-k */
int domore()
{
   char c;

   printf("[more]");
   flush();
   while (!(c = bdos(CPMDCIO,0xFF)));   /* loop till we get one */
   printf("\b\b\b\b\b\b      \b\b\b\b\b\b");
   return c == CTRLC || c == CTRLK;
}

/* Function for qsort to compare two directory entries */
int dirsort(p1,p2)
char *p1, *p2;
{
   short j;

   if (j = memcmp(p1,p2,12))
      return j;

      /* Both are the same file -- sort on extent */
   if ((j = (p1[14] * 32 + p1[12]) - (p2[14] * 32 + p2[12])) > 0)
      return -1;
   else
      return 1;
}

int cntbits(byte)
char byte;
{
   int i,count;

   for (count=i=0; i<8; i++) {
      count += (byte & 1);
      byte >>= 1;
   }
   return count;
}

void resetace()  /* to default values */
{
   Current.cbaudindex = Line.baudindex;
   Current.cparity = Line.parity;
   Current.cdatabits = Line.databits;
   Current.cstopbits = Line.stopbits;
   updateace();
}

void updateace()
{
   initace(Current.cbaudindex,Current.cparity,
      Current.cdatabits,Current.cstopbits);
}

void hangup()
{
   char c;
   
   stndout();
   printf("\n ZMP: Disconnect (Y/N) <N>? \007");
   stndend();
   c = dio();
   if (c != 'Y' && c != 'y' ) {
      printf("\n");
      return;
   }
   printf("\nHanging up...\n");
   dtroff();                    /* Turn DTR off for a bit */
   mswait(200);                 /* Like 200 ms */
   dtron();                     /* Then back on again */
   mstrout(Modem.hangup,FALSE);
   resetace();
}

/* Prompt user and get any key */
char waitakey()
{
   char c;

   printf("\n Any key to continue: ");
   flush();
   while (!(c = bdos(CPMDCIO,0xFF)));
   return c;
}

/*         End of TERM module            */
