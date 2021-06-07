# ZMP: A Zmodem terminal program for CP/M

> ZMP is a communications/file transfer program for CP/M which performs
> Xmodem, Xmodem-1k (often erroneously called Ymodem), true Ymodem and
> Zmodem file transfer protocols.

-- from the original ZMP.FOR file

This is the
[January 7th, 1989 source code for ZMP 1.5](http://www.classiccmp.org/cpmarchives/cpm/Software/WalnutCD/cpm/modems/zmodem/zmp-src.lbr).
Note that this is not the same as the 1.5 version included in
[ZMP15.LBR](http://www.classiccmp.org/cpmarchives/cpm/Software/WalnutCD/cpm/modems/zmodem/zmp15.lbr)
and is missing several bits and pieces than were included in that release.

This source has been *heavily* modified to compile and link with the
[v3.09 release of the Hi Tech C](http://www.classiccmp.org/cpmarchives/cpm/Software/WalnutCD/cpm/hitech-c/)
compiler. Though ZMP's author, Ron Murray, noted in the source code release
that he'd used Hi Tech C, it was clearly a much earlier release.

While I was pounding away at getting the source into a state that the
compiler would accept (and fixing issues in the compiler's libraries - how
did anyone actually get any work done with v3.09 back in the day?) I changed
a few things that niggled at me:

* Datestamps transferred in Y/Zmodem transfers.
* Fixed long filename conversions on Y/Zmodem transfers.
* Fixed issues displaying baud rate and send times when the baud rate
  is > 19.2K.
* Removed superfluous CR sent to the remote end when a connection was
  established, which was a pita when connecting to a Linux box.
* In keyboard macros, ! translates to a CR now, not CR/LF.
* \ escapes ! and ~ so those literal characters can be used in keyboard macros.
* Long distance access code has been removed.
* Quick dialing letters limited to A-T.

## Status

This is very much a work in progress. I have gotten it to compile, link and
run and it's to a point where I've replaced the original ZMP with this one
on my systems. But I'm still finding things to look at (and then there's
the bugs that Ron Murray fixed between the version of the source code he
released and the version that was used to produce the ZMP 1.5 binaries...).

The Z3 aliases are now able to build the COM and OVR files. ZMCOMPIL.COM
will build ZMPX.COM. OVRCOMP.COM will build all the OVR files.

*Probably!* I was able to get everything to build on a 62K system, which
has a far larger TPA than most Z-systems can manage (or at least what
they could manage back in the day).

Modified/new system files for Hi Tech C are included: cpm.h, time.h,
libc.lib, memset.c, and mktime.c.

