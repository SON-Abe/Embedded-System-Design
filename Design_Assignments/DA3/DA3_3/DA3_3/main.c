#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER2_OVF_vect)
{
	static uint16_t counter = 0;    // STATIC COUNTER HOLDS VALUE BETWEEN ISR
	counter++;                        // INCREMENT COUNTER
	if(counter == 2000)                // IF COUNTER REACHES 2 SECONDS
	{
		PORTB ^= 0x08;                // TOGGLE PORTB LED
		counter = 0;                //RESET COUNTER
	}
}

int main(void)
{
	DDRB |= 0X08;                    // PB3 IS AN OUTPUT
	TCCR2A |= 0X00;                    // SET REGISTER TCCR2A
	TCCR2B |= 0X04;                    // SET PRESCALAR TO 64 AND STARTS PWM
	TIMSK2 |= 0X01;                    // SET INTERRUPT ON OVERFLOW IN TIMER/COUNTER 2
	TCNT2 = 192;                    //  TIMER COUNTER 0 SET TO 0 ->  192
	sei();                            // ENABLE GLOBAL INTERRUPTS
	while(1);                        // INTERRUPT HANDLES LED
	return 0;
}