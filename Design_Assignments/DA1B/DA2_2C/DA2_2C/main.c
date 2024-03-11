#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1<<5)|(1<<4);
	DDRC &= (0<<2);
	PORTC |= (1<<2);
	while(1)
	{
		if (!(PINC & (1<<2)))
		{
			PORTB |= (1<<5);
			_delay_ms(2000);
			PORTB &= ~(1<<5);
		}
	}
}