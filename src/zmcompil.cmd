CC ZMP
CC ZMP2
C -V -FZMP.SYM USERDEF.OBJ ZMP.OBJ ZMP2.OBJ -LX
ERA ZMPX.COM
REN ZMPX.COM=USERDEF.COM
SYMTOAS ZMP
ZAS MAIN.AS
FIND+ ZMP.SYM [USERDEF|OVBGN]
