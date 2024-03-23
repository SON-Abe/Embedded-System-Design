/*
 * main.c
 *
 * Created: 3/22/2024 5:15:35 PM
 *  Author: abrah
 */ 
#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>

int main(void)
{
	int counter = 0;                    // INITIALIZE COUNTER
	DDRB |= 0X20;                        // SET PB5 AS AN OUTPUT
	TCCR0A = 0X00;                        // NORMAL OPERATION
	TCNT0 = 231;                        // TIMER COUNTER 0 SET TO 0 ->  231
	TCCR0B |= 0X03;                        // PRSCALE 64
	while(1)
	{
		while((TIFR0 & 0X01) == 0);        // WAIT FOR THE OVERFLOW EVENT
		if(counter == 10000)            // WHEN COUNTER REACHES 1 SECOND
		{
			PORTB ^= (1 << DDB5);            // TOGGLE PORTB LED
			counter = 0;                // RESET COUNTER
		}
		counter ++;                        // INCREMENT COUNTER
		TCNT0 = 231;                    // RESET TCNT0
		TIFR0 = 0X01;                    // CLEAR FLAG OVERFLOW
	}
	return 0;
}