/*
 *   Z M . C
 *    ZMODEM protocol primitives
 *    07-28-87  Chuck Forsberg Omen Technology Inc
 *
 * Entry point Functions:
 *   zsbhdr(type, hdr) send binary header
 *   zshhdr(type, hdr) send hex header
 *   zgethdr(hdr, eflag) receive header - binary or hex
 *   zsdata(buf, len, frameend) send data
 *   zrdata(buf, len) receive data
 *   stohdr(pos) store position data in Txhdr
 *   long rclhdr(hdr) recover position offset from header
 */

#define  ZM

#undef DEBUG

#include <stdio.h>
#include <cpm.h>
#include "zmp.h"
#include "zmodmdef.h"

extern long updc32(int,long);
extern int zdlread();
extern int readline(int);
extern int noxrd7();
extern int zrb32hdr(char *);
extern int zrbhdr(char *);
extern int zrhhdr(char *);
extern void xmchout(char);
extern void zsendline(int);
extern unsigned updcrc(unsigned,unsigned);
extern void purgeline();
extern void zputhex(int);

void zsbh32(char *,int);
int zrdat32(char *,int);

extern int Txfcs32;
extern int Rxcount;
extern int Baudrate;
extern int Rxtype;
extern int Rxtimeout;
extern long Rxpos;
extern int Crc32;
extern int Crc32t;
extern char *frametypes[];

static int Rxframeind;      /* ZBIN ZBIN32, or ZHEX type of frame received */

/* Send ZMODEM binary header hdr of type type */
void zsbhdr(type, hdr)
char *hdr;
{
   static int n;
   static unsigned crc;

#ifdef DEBUG
   printf("\nSending BINARY header Type %d:",type);
   for (n = 0; n < 4; n++) {
      prhex( *(hdr + n));
   }
   printf("\n");
#endif

   xmchout(ZPAD); 
   xmchout(ZDLE);

   if (Crc32t=Txfcs32) {
      zsbh32(hdr, type);
   } else {
      xmchout(ZBIN); 
      zsendline(type); 
      crc = updcrc(type, 0);

      for (n=4; --n >= 0; ++hdr) {
         zsendline(*hdr);
         crc = updcrc((0377& *hdr), crc);
      }
      crc = updcrc(0,updcrc(0,crc));
      zsendline(crc>>8);
      zsendline(crc);
   }
   if (type != ZDATA) {
      purgeline();
   }
}


/* Send ZMODEM binary header hdr of type type */

void zsbh32(hdr, type)
char *hdr;
{
   static int n;
   static long crc;

   xmchout(ZBIN32);  
   zsendline(type);
   crc = 0xFFFFFFFFL; 
   crc = updc32(type, crc);

   for (n=4; --n >= 0; ++hdr) {
      crc = updc32((0377 & *hdr), crc);
      zsendline(*hdr);
   }
   crc = ~crc;
   for (n=4; --n >= 0;) {
      zsendline((int)crc);
      crc >>= 8;
   }
}

/* Send ZMODEM HEX header hdr of type type */

void zshhdr(type, hdr)
int type;
char *hdr;
{
   static int n;
   static unsigned crc;

#ifdef DEBUG
   printf("\nSending HEX header Type %d:",type);
   for (n = 0; n < 4; n++) {
      prhex( *(hdr + n));
   }
   printf("\n");
#endif

   xmchout(ZPAD); 
   xmchout(ZPAD); 
   xmchout(ZDLE); 
   xmchout(ZHEX);
   zputhex(type);
   Crc32t = 0;

   crc = updcrc(type, 0);
   for (n=4; --n >= 0; ++hdr) {
      zputhex(*hdr); 
      crc = updcrc((0377 & *hdr), crc);
   }
   crc = updcrc(0,updcrc(0,crc));
   zputhex(crc>>8); 
   zputhex(crc);

   /* Make it printable on remote machine */
   xmchout(CR); 
   xmchout(LF);
   /*
    * Uncork the remote in case a fake XOFF has stopped data flow
    */
   if (type != ZFIN && type != ZACK) {
      xmchout(CTRLQ);
   }
   purgeline();
}

/*
 * Send binary array buf of length length, with ending ZDLE sequence frameend
 */
void zsdata(buf, length, frameend)
int length, frameend;
char *buf;
{
   static unsigned crc;

   if (Crc32t) {
      zsda32(buf, length, frameend);
   } else {
      crc = 0;
      for (;--length >= 0; ++buf) {
         zsendline(*buf); 
         crc = updcrc((0377 & *buf), crc);
      }
      xmchout(ZDLE); 
      xmchout(frameend);
      crc = updcrc(frameend, crc);

      crc = updcrc(0,updcrc(0,crc));
      zsendline(crc>>8); 
      zsendline(crc);
   }
   if (frameend == ZCRCW) {
      xmchout(XON);  
      purgeline();
   }
}

zsda32(buf, length, frameend)
char *buf;
int length, frameend;
{
   static long crc;

   crc = 0xFFFFFFFFL;
   for (;--length >= 0;++buf) {
      crc = updc32((0377 & *buf), crc);
      zsendline(*buf);
   }
   xmchout(ZDLE); 
   xmchout(frameend);
   crc = updc32(frameend, crc);

   crc = ~crc;
   for (length=4; --length >= 0;) {
      zsendline((int)crc);  
      crc >>= 8;
   }
}

/*
 * Receive array buf of max length with ending ZDLE sequence
 *  and CRC.  Returns the ending character or error code.
 *  NB: On errors may store length+1 bytes!
 */
int zrdata(buf, length)
char *buf;
int length;
{
   static int c;
   static unsigned crc;
   static char *end;
   static int d;

   if (Rxframeind == ZBIN32)
      return zrdat32(buf, length);

   crc = Rxcount = 0;  
   end = buf + length;
   while (buf <= end) {
      if ((c = zdlread()) & ~0377) {
crcfoo:
         switch (c) {
            case GOTCRCE:
            case GOTCRCG:
            case GOTCRCQ:
            case GOTCRCW:
               crc = updcrc((d=c)&0377, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updcrc(c, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updcrc(c, crc);
               if (crc & 0xFFFF) {
                  zperr("Bad data CRC",TRUE);

#ifdef   DEBUG
                  printf("\nCRC = %u\n",crc);
#endif

                  return NERROR;
               }
               Rxcount = length - (end - buf);
               return d;
            case GOTCAN:
               zperr("Sender CANceled",TRUE);
               return ZCAN;
            case TIMEOUT:
               zperr("TIMEOUT",TRUE);
               return c;
            default:
               zperr("Bad data subpkt",TRUE);
               return c;
         }
      }
      *buf++ = c;
      crc = updcrc(c, crc);
   }
   zperr("Subpkt too long",TRUE);
   return NERROR;
}

int zrdat32(buf, length)
char *buf;
int length;
{
   static int c, d;
   static long crc;
   static char *end;

#ifdef DEBUG
   printf("\n(32)\n");
#endif

   crc = 0xFFFFFFFFL;  
   Rxcount = 0;  
   end = buf + length;
   while (buf <= end) {
      if ((c = zdlread()) & ~0377) {
crcfoo:
         switch (c) {
            case GOTCRCE:
            case GOTCRCG:
            case GOTCRCQ:
            case GOTCRCW:
               d = c;  
               c &= 0377;
               crc = updc32(c, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updc32(c, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updc32(c, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updc32(c, crc);
               if ((c = zdlread()) & ~0377)
                  goto crcfoo;
               crc = updc32(c, crc);
               if (crc != 0xDEBB20E3) {
                  zperr("Bad data CRC",TRUE);
                  return NERROR;
               }
               Rxcount = length - (end - buf);
               return d;
            case GOTCAN:
               zperr("Sender CANceled",TRUE);
               return ZCAN;
            case TIMEOUT:
               zperr("TIMEOUT",TRUE);
               return c;
            default:
               zperr("Bad data subpkt",TRUE);
               return c;
         }
      }
      *buf++ = c;
      crc = updc32(c, crc);
   }
   zperr("Subpkt too long",TRUE);
   return NERROR;
}


/*
 * Read a ZMODEM header to hdr, either binary or hex.
 *  eflag controls local display of non zmodem characters:
 *   0:  no display
 *   1:  display printing characters only
 *   2:  display all non ZMODEM characters
 *  On success, set Zmodem to 1, set Rxpos and return type of header.
 *   Otherwise return negative on error.
 *   Return NERROR instantly if ZCRCW sequence, for fast error recovery.
 */
int zgethdr(hdr, eflag)
char *hdr;
int eflag;
{
   int c, cancount;
   unsigned long n;

   n = Zrwindow + Baudrate * 10;      /* Max bytes before start of frame */
   Rxframeind = Rxtype = 0;

startover:
   cancount = 5;
again:
   /* Return immediate NERROR if ZCRCW sequence seen */
   switch (c = readline(Rxtimeout)) {
      case RCDO:
      case TIMEOUT:
         goto fifi;
      case CAN:
gotcan:
         if (--cancount <= 0) {
            c = ZCAN; 
            goto fifi;
         }
         switch (c = readline(INTRATIME)) {
            case TIMEOUT:
               goto again;
            case ZCRCW:
               c = NERROR;
               /* **** FALL THRU TO **** */
            case RCDO:
               goto fifi;
            default:
               break;
            case CAN:
               if (--cancount <= 0) {
                  c = ZCAN; 
                  goto fifi;
               }
               goto again;
         }
         /* **** FALL THRU TO **** */
      default:
agn2:
         if ( --n == 0) {
            zperr("Grbg ct exceeded",TRUE);
            return(NERROR);
         }
         goto startover;
      case ZPAD|0200:      /* This is what we want. */
      case ZPAD:      /* This is what we want. */
         break;
   }
   cancount = 5;
splat:
   switch (c = noxrd7()) {
      case ZPAD:
         goto splat;
      case RCDO:
      case TIMEOUT:
         goto fifi;
      default:
         goto agn2;
      case ZDLE:      /* This is what we want. */
         break;
   }

   switch (c = noxrd7()) {
      case RCDO:
      case TIMEOUT:
         goto fifi;
      case ZBIN:
         Rxframeind = ZBIN;  
         Crc32 = FALSE;
         c =  zrbhdr(hdr);
         break;
      case ZBIN32:
         Crc32 = Rxframeind = ZBIN32;
         c =  zrb32hdr(hdr);
         break;
      case ZHEX:
         Rxframeind = ZHEX;  
         Crc32 = FALSE;
         c =  zrhhdr(hdr);
         break;
      case CAN:
         goto gotcan;
      default:
         goto agn2;
   }
   Rxpos = (unsigned)(hdr[ZP3] & 0377);
   Rxpos = (Rxpos<<8) + (unsigned)(hdr[ZP2] & 0377);
   Rxpos = (Rxpos<<8) + (unsigned)(hdr[ZP1] & 0377);
   Rxpos = (Rxpos<<8) + (unsigned)(hdr[ZP0] & 0377);
fifi:
   switch (c) {
      case GOTCAN:
         c = ZCAN;
         /* **** FALL THRU TO **** */
      case ZNAK:
      case ZCAN:
      case NERROR:
      case TIMEOUT:
      case RCDO:
         sprintf(Buf,"Got %s", frametypes[c+FTOFFSET]);
         zperr(Buf,TRUE);
         /* **** FALL THRU TO **** */
      default:
         break;
   }
   return c;
}

#ifdef DEBUG

/* Print a byte in hex on the console */

prhex(byte)
char byte;
{
   static char digits[] = "0123456789abcdef";
   char hi, lo;

   hi = digits[(byte & 0xf0) >> 4];
   lo = digits[byte & 0x0f];
   printf(" %c%c",hi,lo);
}

#endif

/***************************** End of hzm.c *********************************/
