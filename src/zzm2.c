
/* ZZM.C Part 2 */

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

#undef DEBUG

extern long updc32(int,long);
extern unsigned updcrc(unsigned,unsigned);
extern void zperr(char *,int);
extern int readline(int);
extern void xmchout(char);

int zdlread();
int zrb32hdr(char *);
int zrhhdr(char *);
int zgethex();
void zputhex(int);
void zsendline(int);
int zgeth1();
int noxrd7();
void stohdr(long);
long rclhdr(char *);

extern int Zctlesc;
extern char Txhdr[4];
extern int Rxtimeout;
extern int Rxtype;

/* Receive a binary style header (type and position) */
int zrbhdr(hdr)
char *hdr;
{
   static int c, n;
   static unsigned crc;

   if ((c = zdlread()) & ~0377) {
      return c;
   }
   Rxtype = c;
   crc = updcrc(c, 0);

   for (n=4; --n >= 0; ++hdr) {
      if ((c = zdlread()) & ~0377) {
         return c;
      }
      crc = updcrc(c, crc);
      *hdr = c;
   }
   if ((c = zdlread()) & ~0377) {
      return c;
   }
   crc = updcrc(c, crc);
   if ((c = zdlread()) & ~0377) {
      return c;
   }
   crc = updcrc(c, crc);
   if (crc & 0xFFFF) {
      zperr("Bad Header CRC",TRUE); 
      return NERROR;
   }
   Zmodem = 1;

#ifdef DEBUG
   printf("\nReceived BINARY header type %d: ",Rxtype);
   for (n = -4; n < 0; n++) {
      prhex( *(hdr + n));
   }
   printf("\n");
#endif

   return Rxtype;
}

/* Receive a binary style header (type and position) with 32 bit FCS */
int zrb32hdr(hdr)
char *hdr;
{
   static int c, n;
   static long crc;

   if ((c = zdlread()) & ~0377) {
      return c;
   }
   Rxtype = c;
   crc = 0xFFFFFFFFL; 
   crc = updc32(c, crc);

   for (n=4; --n >= 0; ++hdr) {
      if ((c = zdlread()) & ~0377) {
         return c;
      }
      crc = updc32(c, crc);
      *hdr = c;
   }
   for (n=4; --n >= 0;) {
      if ((c = zdlread()) & ~0377) {
         return c;
      }
      crc = updc32(c, crc);
   }
   if (crc != 0xDEBB20E3) {
      zperr("Bad Header CRC",TRUE); 
      return NERROR;
   }
   Zmodem = 1;

#ifdef DEBUG
   printf("\nReceived 32-bit FCS BINARY header type %d: ",Rxtype);
   for (n = -4; n < 0; n++)
      prhex( *(hdr + n));
   printf("\n");
#endif

   return Rxtype;
}


/* Receive a hex style header (type and position) */
int zrhhdr(hdr)
char *hdr;
{
   static int c;
   static unsigned crc;
   static int n;

   if ((c = zgethex()) < 0) {
      return c;
   }
   Rxtype = c;
   crc = updcrc(c, 0);

   for (n=4; --n >= 0; ++hdr) {
      if ((c = zgethex()) < 0) {
         return c;
      }
      crc = updcrc(c, crc);
      *hdr = c;
   }
   if ((c = zgethex()) < 0) {
      return c;
   }
   crc = updcrc(c, crc);
   if ((c = zgethex()) < 0) {
      return c;
   }
   crc = updcrc(c, crc);
   if (crc & 0xFFFF) {
      zperr("Bad Header CRC",TRUE); 
      return NERROR;
   }
   if (readline(INTRATIME) == '\r') {   /* Throw away possible cr/lf */
      readline(INTRATIME);
   }
   Zmodem = 1; 

#ifdef DEBUG
   printf("\nReceived HEX header type %d: ",Rxtype);
   for (n = -4; n < 0; n++) {
      prhex( *(hdr + n));
   }
   printf("\n");
#endif

   return Rxtype;
}

/* Send a byte as two hex digits */
void zputhex(c)
int c;
{
   static char digits[] = "0123456789abcdef";

   xmchout(digits[(c&0xF0)>>4]);
   xmchout(digits[(c)&0xF]);
}

/*
 * Send character c with ZMODEM escape sequence encoding.
 *  Escape XON, XOFF. Escape CR following @ (Telenet net escape)
 */
void zsendline(c)
int c;
{
   static lastsent;

   switch (c &= 0377) {
   case ZDLE:
      xmchout(ZDLE);
      xmchout (lastsent = (c ^= 0100));
      break;
   case 015:
   case 0215:
      if (!Zctlesc && (lastsent & 0177) != '@') {
         goto sendit;
      }
      /* **** FALL THRU TO **** */
   case 020:
   case 021:
   case 023:
   case 0220:
   case 0221:
   case 0223:
      xmchout(ZDLE);
      c ^= 0100;
sendit:
      xmchout(lastsent = c);
      break;
   default:
      if (Zctlesc && ! (c & 0140)) {
         xmchout(ZDLE);
         c ^= 0100;
      }
      xmchout(lastsent = c);
   }
}

/* Decode two lower case hex digits into an 8 bit byte value */
int zgethex()
{
   int c;

   c = zgeth1();
   return c;
}

int zgeth1()
{
   static int c, n;

   if ((c = noxrd7()) < 0)
      return c;
   n = c - '0';
   if (n > 9) {
      n -= ('a' - ':');
   }
   if (n & ~0xF) {
      return NERROR;
   }
   if ((c = noxrd7()) < 0) {
      return c;
   }
   c -= '0';
   if (c > 9) {
      c -= ('a' - ':');
   }
   if (c & ~0xF) {
      return NERROR;
   }
   c += (n<<4);
   return c;
}

/*
 * Read a byte, checking for ZMODEM escape encoding
 *  including CAN*5 which represents a quick abort
 */

int zdlread()
{
   static int c;

again:
   switch (c = readline(Rxtimeout)) {
   case ZDLE:
      break;
   case 023:
   case 0223:
   case 021:
   case 0221:
      goto again;
   default:
      if (Zctlesc && !(c & 0140)) {
         goto again;
      }
      return c;
   }
again2:
   if ((c = readline(Rxtimeout)) < 0) {
      return c;
   }
   if (c == CAN && (c = readline(Rxtimeout)) < 0) {
      return c;
   }
   if (c == CAN && (c = readline(Rxtimeout)) < 0) {
      return c;
   }
   if (c == CAN && (c = readline(Rxtimeout)) < 0) {
      return c;
   }
   switch (c) {
      case CAN:
         return GOTCAN;
      case ZCRCE:
      case ZCRCG:
      case ZCRCQ:
      case ZCRCW:
         return (c | GOTOR);
      case ZRUB0:
         return 0177;
      case ZRUB1:
         return 0377;
      case 023:
      case 0223:
      case 021:
      case 0221:
         goto again2;
      default:
         if (Zctlesc && ! (c & 0140)) {
            goto again2;
         }
         if ((c & 0140) ==  0100) {
            return (c ^ 0100);
         }
         break;
   }
   sprintf(Buf,"Bad escape %x", c);
   zperr(Buf,TRUE);
   return NERROR;
}

/*
 * Read a character from the modem line with timeout.
 *  Eat parity, XON and XOFF characters.
 */
int noxrd7()
{
   static int c;

   for (;;) {
      if ((c = readline(Rxtimeout)) < 0) {
         return c;
      }
      switch (c &= 0177) {
         case XON:
         case XOFF:
            continue;
         default:
            if (Zctlesc && !(c & 0140)) {
               continue;
            }
         case '\r':
         case '\n':
         case ZDLE:
            return c;
      }
   }
}

/* Store long integer pos in Txhdr */
void stohdr(pos)
long pos;
{
   Txhdr[ZP0] = pos;
   Txhdr[ZP1] = (pos>>8);
   Txhdr[ZP2] = (pos>>16);
   Txhdr[ZP3] = (pos>>24);
}

/* Recover a long integer from a header */
long rclhdr(hdr)
char *hdr;
{
   static long l;

   l = (unsigned)(hdr[ZP3] & 0377);
   l = (l << 8) | (unsigned)(hdr[ZP2] & 0377);
   l = (l << 8) | (unsigned)(hdr[ZP1] & 0377);
   l = (l << 8) | (unsigned)(hdr[ZP0] & 0377);
#ifdef DEBUG
   lreport(FBLOCKS,l);
#endif
   return l;
}

/***************************** End of hzm2.c *********************************/
