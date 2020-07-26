	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA

	global	start,_main,_exit,__Hbss, __Lbss, __argc_, startup

start:	jp	start2
	defm	'Z3ENV'
	defb	1
	defw	0
start2:	ld	hl,(6)		;base address of fdos
	ld	sp,hl		;stack grows downwards
	ld	de,__Lbss	;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,nularg
	push	hl
	ld	hl,80h		;argument buffer
	ld	c,(hl)
	inc	hl
	ld	b,h		; b = 0 (h was 0)
	add	hl,bc
	ld	(hl),b		;zero terminate it (b was 0)
	ld	hl,81h
	push	hl
	call	startup
	pop	bc		;unjunk stack
	pop	bc
	push	hl
	ld	hl,(__argc_)
	push	hl
	call	_main
	push	hl
	call	_exit
	rst	0
	nop			; should be at 0144H

	psect	data
nularg:	defb	0
	end	start
