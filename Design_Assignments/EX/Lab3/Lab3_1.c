#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL

ISR(INT0_vect)
{
    // Toggle LED on PORTC while dip switch 7 is high
    while ((PIND & 0x04) == 0x04) 
    {
        PORTC ^= 0x01; // Toggle pin PC0
        _delay_ms(100); // Delay
    }
}

int main(void) 
{
    // Set PORTC as output and PORTD as input with pull-up
    DDRC = 0xFF; // Set PORTC as output
    DDRD = 0x00; // Set PORTD as input
    PORTD = 0xFF; // Enable pull-up on PORTD

    // Enable external interrupt INT0 on rising edge
    EICRA = 0x03; // Rising edge trigger
    EIMSK = 0x01; // Enable external interrupt INT0
    EIFR = 0x00;

    sei(); // Enable global interrupts

    while (1) 
    {
        PORTC ^= 0xFF;
        _delay_ms(1000);
    }

    return 0;
}
