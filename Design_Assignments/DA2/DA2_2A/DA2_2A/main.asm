; DA2_2A.asm
; Created: 3/10/2024 2:23:43 AM
; Author : SON

#define F_CPU 16000000UL	;16MHz clock
.include "m328pbdef.inc"	;m328pb device header file

.org 0x00					;set program counter the start of the code
	jmp main				;jump to main

main:
	cbi DDRC, 2      		;set PC2 as input
	sbi DDRB, 5      		;set PB5 as output
	cbi PORTB, 5			;enable pull-up resistor

loop:
	sbic PINC, 2			;skip next lin if PINC 2 clear
	jmp loop				;loop
	sbi PORTB, 5			;turn on PB5
	call delay				;for 2 seconds
	cbi PORTB, 5			;turn off PB5
	call delay				;for 2 seconds
	jmp loop				; loop`

delay:
    ldi  r18, 163			;loaded value to help delay for 2000ms
    ldi  r19, 87			;loaded value to help delay for 2000ms
    ldi  r20, 3				;loaded value to help delay for 2000ms
L1: dec  r20				;dec value to help delay for 2000ms
    brne L1					;loop till 0
    dec  r19				;dec value to help delay for 2000ms
    brne L1					;loop till 0
    dec  r18				;dec value to help delay for 2000ms
    brne L1					;loop till 0
    nop
ret							;return when done with delay