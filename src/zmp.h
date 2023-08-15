/****************************** zmp.h *************************************/

#define	FALSE	0
#define	TRUE	1

#define	MAXOVLY	0x5100	/* Largest overlay size */

#ifndef	MAIN
#define	EXTERN	extern
#else
#define	EXTERN
#endif

#undef	ZCPR3		/* #define if using zcpr3 */

/* Make allowances for the vagaries of different C compilers (sigh!) */

#define	MEMORY_FULL 0
#define	NBDOS	bdoshl

#define	PBUFSIZ	2048		/*2k printer buffer*/
#define	DFLTINDEX	5	/* pointer to default baud */

/*************************************************************************/
/*																								*/
/*  These #defines determine which keys will be interpreted as			*/
/*  command characters.																	*/
/*																								*/
/*************************************************************************/

#define	RECEIVE		'R'	/*receive file*/
#define	CAPTURE		'M'	/*toggle capture mode*/
#define	DIR		'D'	/*get disk directory*/
#define	PRTSCRN		'Y'	/*print screen*/
#define	SEND		'S'	/*send file*/
#define	HANGUP		'X'	/*hangup*/
#define	COMMAND		'L'	/*change line parameters*/
#define	HELP		'H'	/*get instructions*/
#define	QUIT		'Q'	/*quit*/
#define	DIAL		'I'	/*make phone call*/
#define	HOST		'O'	/* host mode */
#define	CONFIG		'C'	/*configure system*/
#define	TOGPRT		'P'	/*toggle printer*/
#define	DISK		'F'	/*file/disk operations*/
#define	CLRSCR		'Z'	/*clear local screen*/
#define	BRK		'B'	/*send break*/
#define	DISPKEYS	'K'	/*display keyboard macros*/

/* Constants for directory routines */
#define	DIRLINES	28	/* # of lines per page */
#define	SORTCOLS	4	/* # of columns of sorted entries */
#define	UNSORTCOLS	5	/* # of columns of unsorted entries */

/* Locations for file transfer box */
#define	RPTPOS		42	/* Where the status goes */
#define	LC		RPTPOS-23
#define	TR		2
#define	HT		12
#define	WD		41
#define	RC		LC+WD-1
#define	BR		TR+HT-1
#define	HORIZ		'\xC4'
#define	VERT		'\xB3'
#define	UL		'\xDA'
#define	UR		'\xBF'
#define	LL		'\xC0'
#define	LR		'\xD9'

/***************** line numbers for report function *************************/

#define	PROTOCOL	3
#define	PATHNAME	4
#define	FILESIZE	5
#define	BLKCHECK	6
#define	SENDTIME	7
#define	KBYTES		8
#define	BLOCKS		9
#define	FBLOCKS		10
#define	ERRORS		11
#define	MESSAGE		12


#define	BUFSTART	16384	/* 16k text buffer starting size */
#define	OK		0
#define NOT_OK          (-1)
#define	NERROR		(-1)
#define	ERROR		0
#define	KEYHIT		0
#define	MAXFILES	255	/* max number of expanded files */
#define	FNSIZE		17	/* filename: 1(A)+3(15:)+8+1+3+null */
#define	BUFIOT		0	/* error return on buffered fopen */
#define	UBIOT		-1	/* unbuffered open */


/**************************************************************************/
/*																								*/
/*		Miscellaneous ASCII characters.												*/
/*																								*/
/**************************************************************************/

#define	NUL		0
#define	SOH		1
#define	STX		2
#define	CTRLC		3
#define	EOT		4
#define	CTRLE		5
#define	ENQ		5
#define	ACK		6
#define	BELL		7
#define	BS		8
#define	LF		10
#define	CTRLK		11
#define	CR		13
#define	CTRLQ		17		/* xon */
#define	XON		('q'&037)
#define	CTRLS		19		/* xoff */
#define	XOFF		('s'&037)
#define	NAK		21
#define	CAN		('X'&037)
#define	CTRLX		24
#define	CPMEOF		26
#define	CTRLZ		26		/*end of text-file character*/
#define	ESC		27
#define	DEL		127

/******************* constants used in file transfer **********************/

/* Ward Christensen / CP/M parameters - Don't change these! */

#define	TIMEOUT		-2
#define	RCDO		(-3)
#define	ERRORMAX	5
#define	RETRYMAX	10
#define	WCEOT		(-10)
#define	SECSIZ		128	/* cp/m's Magic Number record size */
#define	PATHLEN		257	/* ready for 4.2 bsd ? */
#define	KSIZE		1024	/* record size with k option */
#define	WANTCRC		'C'
#define	ORIGIN		0x0100  /*starting point for cp/m programs*/
#define	COMPSIZ		35	/*cpm bdos compute file size command*/


#define	CPMBUF		0x80	/*address of cpm file buffer*/
#define	DONE		0xff	/*flag for no more entries*/
#define	RET		'!'	/*symbol for CR string terminator */
#define	WAITASEC	'~'	/* one second delay */
#define	BIOSADDR	1	/*ptr to base of bios + 3*/
#define	LISTST		42	/*index to list status routine*/
#define	ENABLE		0xfb	/*code for EI*/
#define	RTN		0xc9	/*code for RET*/
#define	JUMP		0xc3	/*code for JMP*/

/* BIOS entry functions */
#define	BIOS_CONIN	3	/* console input */
#define	BIOS_CONOUT	4	/* console output */

/************************ configuration variables **********************************/

EXTERN int BFlag
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int PFlag
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int FDx
#ifdef	MAIN
= TRUE
#endif
;
EXTERN int RemEcho
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int Online
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int Filter
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int ParityMask
#ifdef	MAIN
= FALSE
#endif
;
EXTERN char Msgfile[20]
#ifdef	MAIN
= "ZMP.MSG"
#endif
;
EXTERN char Phonefile[20]
#ifdef	MAIN
= "ZMP.FON"
#endif
;
EXTERN char Logfile[20]
#ifdef	MAIN
= "ZMP.LOG"
#endif
;
EXTERN char Cfgfile[20]
#ifdef	MAIN
= "ZMP.CFG"
#endif
;
EXTERN char Configovly[9]
#ifdef	MAIN
= "ZMCONFIG"
#endif
;
EXTERN char Initovly[9]
#ifdef	MAIN
= "ZMINIT"
#endif
;
EXTERN char Termovly[9]
#ifdef	MAIN
= "ZMTERM"
#endif
;
EXTERN char Xferovly[9]
#ifdef	MAIN
= "ZMXFER"
#endif
;
EXTERN char KbMacro[10][22]
#ifdef	MAIN
 = {
 	"Macro Key 0!",
	"Macro Key 1!",
	"Macro Key 2!",
	"Macro Key 3!",
	"Macro Key 4!",
	"Macro Key 5!",
	"Macro Key 6!",
	"Macro Key 7!",
	"Macro Key 8!",
	"Macro Key 9!" 
}
#endif
;

EXTERN struct modemparms {
	char init[41];
	char dialcmd[9];
	char dialsuffix[9];
	char connect[21];
	char busy1[21];
	char busy2[21];
	char busy3[21];
	char busy4[21];
	char hangup[21];
	int timeout;
	int pause;
} Modem
#ifdef	MAIN
	= {
	"ATE0V1X4S0=0!",
	"ATD",
	"!",
	"CONNECT",
	"BUSY",
	"NO CARRIER",
	"NO ANSWER",
	"ERROR",
	"~+++~ATH0!",
	40,8
	}
#endif
;

EXTERN struct lineparms {
	int baudindex;
	char parity;
	int databits;
	int stopbits;
} Line
#ifdef MAIN
= { DFLTINDEX, 'N', 8, 1 }
#endif
;
EXTERN int Pbufsiz
#ifdef	MAIN
= PBUFSIZ
#endif
;
EXTERN int Maxdrive
#ifdef	MAIN
= 'P'
#endif
;
EXTERN struct phonebook {
	char name[19];
	char number[19];
	int pbaudindex;			
	char pparity;
	int pdatabits;
	int pstopbits;
	int echo;
} *Book;

EXTERN char *Prtbuf;
EXTERN unsigned Buftop;
EXTERN char *Prthead;
EXTERN char *Prttail;
EXTERN char *Prttop;
EXTERN char *Prtbottom;
EXTERN int Inhost
#ifdef MAIN
 = FALSE
#endif
;
#ifdef INIT
char Version[41]
=  "Version 1.5+/- -- 24 July 2020"	/* version number for ZMP */
=  "Version 1.5+/- -- 15 Aug 2023 "	/* version number for ZMP */
/*  ^_____________________________________^		Maximum length! */
#endif
;
EXTERN char Lastlog[20];

EXTERN int Dialing;
EXTERN int Lastkey;

EXTERN int Chardelay
#ifdef	MAIN
= 0
#endif
;
EXTERN int Linedelay
#ifdef	MAIN
= 0
#endif
;
EXTERN int FirsTerm		/* Flag -- first time terminal overlay runs */
#ifdef	MAIN
= TRUE
#endif
;

EXTERN int Invokdrive, Currdrive, Invokuser, Curruser;
EXTERN int Overdrive, Overuser, Userid;

EXTERN char *Userover;

EXTERN struct settings {
	int cbaudindex;
	char cparity;
	int cdatabits;
	int cstopbits;
} Current;

EXTERN unsigned Baudtable[14]
#ifdef MAIN
 = { 11,30,45,60,71,120,240,480,960,1920,3840,5760,7680 }
#endif
;
EXTERN int QuitFlag		/* Operator abort */
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int StopFlag		/* abort/stop from remote end */
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int Crcflag
#ifdef	MAIN
= TRUE
#endif
;
EXTERN int XonXoff
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int XonXoffOk
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int Sending	/* file transfer direction */
#ifdef	MAIN
= FALSE
#endif
;
EXTERN int Zmodem
#ifdef MAIN
 = FALSE
#endif
;
EXTERN int Nozmodem
#ifdef MAIN
 = FALSE
#endif
;
EXTERN int Blklen
#ifdef MAIN
 = SECSIZ
#endif
;
EXTERN int Xmodem
#ifdef MAIN
 = FALSE
#endif
;
EXTERN int Zrwindow			/* RX window size (controls garbage count) */
#ifdef MAIN
 = 1400
#endif
;
EXTERN unsigned Bufsize
#ifdef	MAIN
= BUFSTART
#endif
;

EXTERN char Pathname[128];
EXTERN char Buf[128];			/* general purpose buffer */
EXTERN char *MainBuffer;
EXTERN unsigned TxtPtr
#ifdef	MAIN
= 0
#endif
;
EXTERN int Stopped
#ifdef MAIN
 = FALSE
#endif
;
EXTERN char *Mspeed
#ifdef MAIN
 = (char *) 0x3c
#endif
;
EXTERN char *Wheel
#ifdef MAIN
 = (char *) 0x3e
#endif
;
EXTERN int Wantfcs32
#ifdef MAIN
 = TRUE
#endif
;

EXTERN int ZsDos
#ifdef MAIN
 = TRUE
#endif
;

EXTERN unsigned long FileModTime;

struct stat {
	char fname[9];
	char fext[4];
	unsigned long modtime;
	int records;
};

struct direntry {
	char userno;
	char flname[8];
	char ftype[3];
	char fextent;
	char reserved[2];
	char record;
	char map[16];
};

struct sortentry {
	unsigned char user;	/* user area */
	char name[8];		/* filename */
	char type[3];		/* filetype */
	char ex;		/* low byte of extent (0 - 31) */
	char s1;		/* unused */
	char s2;		/* count of extents over 31 */
	unsigned char rc;	/* record count */
};

EXTERN struct fcb Thefcb;

struct dpb {
	unsigned spt;
	char bsh;
	char blm;
	char exm;
	unsigned dsm;
	unsigned drm;
	char al0;
	char al1;
	unsigned cks;
	unsigned off;
};

EXTERN char **Pathlist;

/******************************* end of zmp.h ******************************/

