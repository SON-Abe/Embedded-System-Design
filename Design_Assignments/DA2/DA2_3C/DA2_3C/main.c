// DA2_3C.c
// Created: 3/10/2024 4:27:09 AM
// Author : SON

#define F_CPU 8000000UL						//8MHz clock
#include <avr/io.h>							//AVR io header file
#include <util/delay.h>						//delay header file
#include <avr/interrupt.h>					//interrupt header file

ISR(INT1_vect) 								//interrupt subroutine for INT1_vect
{
	PORTB &= ~(1 << 4); 					//turn off PB4
	_delay_ms(3000);						//delay for 3000ms
}

int main(void)								//main code
{
	DDRB |= (1 << 4);  						//set PB4 as output
	PORTD |= (1 << 3); 						//enable pull-up resistor for PD3

	EICRA |= (1 << ISC11) | (1 << ISC10); 	//rising edge trigger
	EIMSK |= (1 << INT1);                 	//enable INT1
	sei();                                 	//enable global interrupts

	while (1)								//forever loop
	{
		PORTB |= (1 << 4);					//turn on PB4
	}

	return 0;								//return 0
}