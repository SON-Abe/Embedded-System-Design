// DA2_2C.c
// Created: 3/10/2024 12:07:15 AM
// Author : SON

#define F_CPU 16000000UL        //16MHz clock
#include <avr/io.h>             //AVR io header file
#include <util/delay.h>         //delay header file

int main(void)                  //main
{
	DDRB |= (1<<5);             //set PB5 as output
	PORTB &= ~(1<<5);           //set PB5 to '0'
	DDRC &= ~(1<<2);            //set PC2 as input
	PORTC |= (1<<2);            //enable pull-up resistor
	while(1)                    //forever loop
	{
		if (!(PINC & (1<<2)))   //if PC2 is GND
		{
			PORTB |= (1<<5);    //turn on PB5
			_delay_ms(2000);    //for 2000ms
			PORTB &= ~(1<<5);   //turn off PB5
			_delay_ms(2000);    //for 2000ms
		}
	}
}