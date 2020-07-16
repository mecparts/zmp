; Jump table and dummy routines for user-defined stuff
;
;Written rjm August, 1988
; Added setport() rjm 25/3/89

	psect	text

	global	userdef
	global	_screenpr,_mrd,_mchin,_mchout,_moutrdy,_mirdy
	global	_sendbrk,_locate,_cls,_stndout,_getvarsr
	global	_stndend,_hidecurs,_showcurs,_savecurs,_restcurs
	global	_minterru,_initvect,_deinitve,_mdmerror,_dtron
	global	_dtroff,_initace,_wait,_mswait,_userin,_userout
	global	_getvars,_setport

;General equates:
bdos	equ	5
cr	equ	0dh
lf	equ	0ah

userdef:		; Marker so we know where the overlay starts

;Organise as jump table for the overlay

_screenpr:		; screen print for ZMP, get variables for RZMP
_getvarsr:
	jp	abort
_mrd:
	jp	abort
_mchin:
	jp	abort
_mchout:
	jp	abort
_moutrdy:
	jp	abort
_mirdy:
	jp	abort
_sendbrk:
	jp	abort
_locate:
	jp	abort
_cls:
	jp	abort
_stndout:
	jp	abort
_stndend:
	jp	abort
_hidecurs:
	jp	abort
_showcurs:
	jp	abort
_savecurs:
	jp	abort
_restcurs:
	jp	abort
_minterru:
	jp	abort
_initvect:
	jp	abort
_deinitve:
	jp	abort
_mdmerror:
	jp	abort
_dtron:
	jp	abort
_dtroff:
	jp	abort
_initace:
	jp	abort
_wait:
	jp	abort
_mswait:
	jp	abort
_userin:
	jp	abort
_userout:
	jp	abort
_getvars:
	jp	abort
_setport:
	jp	abort

;Spares
	jp	abort
	jp	abort
	jp	abort
	jp	abort
	jp	abort


abort:
	call	print
	defb	cr,lf
	defm	'This copy of ZMP is not installed. Obtain a '
	defm	'suitable overlay for your system.'
	defb	cr,lf,lf,0
	jp	0		; warm boot

;In-line print routine
print:
	ex	(sp),hl		; get return address
print1:
	ld	a,(hl)		; get next character
	inc	hl		; bump pointer
	or	a		; done if zero
	jr	nz,print2
	ex	(sp),hl		; so swap back
	ret
print2:
	call	cout		; ok, print it
	jr	print1		; and loop

;Print character in A
cout:
	push	bc
	push	de
	push	hl
	ld	c,2
	ld	e,a
	call	bdos
	pop	hl
	pop	de
	pop	bc
	ret

	defs	0400h - ($ - userdef)	; Allow room for overlay

