#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect)                // TIMER1 ISR CTC MODE
{
	PORTB ^= 0X10;                    // TOGGLE  PORTB4
}


int main(void)
{
	DDRB |= 0x10;                    // PB4 AS OUTPUT
	TCCR1A |= 0X00;                    // SET TCCR1A REGISTER TO 0
	TCCR1B |= 0X0D;                    // SET TIMER1 TO CTC MODE & PRESCALE 1024
	OCR1A = 49152;                    // INITIALIZE COMPARE VALUE
	TIMSK1 |= 0X02;                    //ENABLE TIMER/COUNTER 1 OUTPUT COMPARE A MATCH INTERRUPT
	sei();                            // ENABLE GLOBAL INTERRUPT
	while(1)
	{
		if(TIFR1 & (1 << TOV1))        // CHECK IF TIMER 1 OVERFLAG IS 1
		TIFR1 |= (1 << TOV1);    // CLEAR OVERFLOW FLAG
	}
}