; DA2_3A.asm
; Created: 3/10/2024 12:52:21 PM
; Author : SON

#define F_CPU 8000000UL						;8MHz clock
.include "m328pbdef.inc"					;m328pb device header file
.org 0x0000									;set program counter the start of the code
    jmp main								;to main
.org 0x0004									;set to the address of INT1
	jmp INT1_ISR							;jmp to INT1_ISR for INT1 subroutine

main:										;main
	ldi r16, 1 << 4							;set PB4
	out DDRB, r16							;as the output

	ldi r17, 1 << 3							;set PD3
	out PORTD, r17							;as the input

	ldi r18, (1 << ISC11) | (1 << ISC10)	;load 1 << ISC11 or 1 << ISC10 for rising edge trigger for IN1
	sts EICRA, r18							;in register EICRA

	ldi r19, 1 << INT1						;enable INT1
	sts EIMSK, r19							;in register EIMSK

	sei										;enable global interrupts

loop:
	in r20, PIND             				;Read PD3
	andi r20, (1 << PIND3)    				;Mask out all bits except for PD3
	cpi r20, (1<<PIND3)						;check if PD3 on
	brne INT1_ISR               			;break if PD3 is GND
	ldi r21, (1 << 4)						;load '1' to PB4 
	out PORTB, r21							;output to PB4
	rjmp loop                				;loop

INT1_ISR:									;INT1 subroutine
	ldi r22, ~(1 << 4)						;load '0' to PB4
	out PORTB, r22							;output to PB4
	rcall delay								;call for delay
	jmp loop								;jmp back to loop when done

delay:										;delay
    ldi  r18, 244							;loaded value to help delay for 3000ms
    ldi  r19, 130							;loaded value to help delay for 3000ms
    ldi  r20, 6								;loaded value to help delay for 3000ms
L1: dec  r20								;dec value to help delay for 3000ms
    brne L1									;repeat till 0
    dec  r19								;dec value to help delay for 3000ms
    brne L1									;repeat till 0
    dec  r18								;dec value to help delay for 3000ms
    brne L1									;repeat till 0