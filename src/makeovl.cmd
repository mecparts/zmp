IF ~EX ZMPOVL.HEX
M80 =ZMPOVL
TST M
IF ~ER
RELHEX ZMPOVL
FI
FI
IF EX ZMPOVL.HEX
ERA ZMP.COM
MLOAD ZMP.COM=ZMPX.COM,ZMPOVL
FI
