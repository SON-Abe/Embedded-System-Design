/*
 * main.c
 *
 * Created: 3/9/2024 2:48:21 AM
 *  Author: kiman
 */ 

/*
connect a switch to INT1 (PD3) 
(active high turn on pull up transistor)
and using an interrupt mechanism turn on 
the led at PORTB4 for 3 sec after event
*/
#define F_CPU 8000000UL
#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	DDRB  |= 1<<4;			//PB4 AS AN OUTPUT
	PORTD |= 1<<3;			//PULL-UP ACTIVATED 
	EICRA = (1<<ISC11);		//MAKE INT1 RISING EDGE TRIGGERED
	EIMSK = (1 << INT1);	//ENABLE EXTERNAL INTERRUPT 1
	sei();					//enable interrupts		
	
    while(1)				// wait here
	{
		PORTB |= (1<<4);	//TOGGLE PORTB4
	}
	return 0;
}

ISR(INT1_vect)// ISR FOR EXTERNAL INTERRUPT 1
{
	PORTB &= ~(1<<4);
	_delay_ms(3000);
}