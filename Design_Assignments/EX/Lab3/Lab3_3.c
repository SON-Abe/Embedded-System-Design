#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(PCINT1_vect)
{
		PORTB ^= 0xC3;
		_delay_ms(100);
}

int main(void)
{
	DDRB = 0xFF;
	DDRC  = 0x00;
	PORTC = 0xFF;

	// Enable pin change interrupt on PC0 pin
	PCICR |= (1 << PCIE1); // Enable PCIE1 (PCINT[14:8] interrupts)
	PCMSK1 |= (1 << PCINT8); // Enable PCINT8 (PC0) interrupt

	// Enable global interrupts
	sei();

	while(1)
	{
		PORTB ^= 0x3C; // Toggle LEDs with the original sequence
		_delay_ms(80);
	}
}
