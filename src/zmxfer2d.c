/************************ START OF XFER MODULE 2 ****************************/

/* sz.c By Chuck Forsberg modified for cp/m by Hal Maney */

#include <stdio.h>

/* itoa - convert n to characters in s. */
char *itoa(n, s)
short n;
char *s;
{
   static short c, k;
   static char *p, *q;

   if ((k = n) < 0) {   /* record sign */
      n = -n;    /* make n positive */
   }
   q = p = s;
   do {                         /* generate digits in reverse order */
      *p++ = n % 10 + '0';      /* get next digit */
   } while ((n /= 10) > 0);     /* delete it */
   if (k < 0) {
      *p++ = '-';
   }
   *p = 0;
   /* reverse string in place */
   while (q < --p) {
      c = *q; *q++ = *p; *p = c;
   }
   return s;
}

/************************** END OF MODULE 7 *********************************/
