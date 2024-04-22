#define F_CPU 16000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1) //BAUD_PRESCALEFORMULA
#define  Trigger_pin	PINB1	/* Trigger pin */
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

char buffer[5];                //Output of the itoa function
int TimerOverflow = 0;
char string[20];
char angle_str[10];
char dist_str[10];
long count;
int distance;
int angle;

void usart_init(void)
{
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8); //LOAD UBRR0 HIGH 8 BITS
	UBRR0L = (uint8_t) (BAUD_PRESCALE); //LOAD UBBR0 LOW BITS
	UCSR0B = (1 << TXEN0); //usart TRANSMITTER AND RECEIVER ENABLED
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8 DATA BITS, 1 STOP BIT,NO PARITY
}

void usart_send (unsigned char ch)
{
	while (! (UCSR0A & (1<<UDRE0))); //WAIT TILL UDR0 IS EMPTY
	UDR0 = ch; //TRANSMIT CHARACTER
}

void usart_print(char* ChArrPtr)
{
	while ((*ChArrPtr) != '\0') //WHILE CHAR ARRAY ISNT EMPTY
	{
		while (! (UCSR0A & (1 << UDRE0))); //WAIT TILL UDR0 IS EMPTY
		UDR0 = *ChArrPtr; //TRANSMIT CHAR
		ChArrPtr++; //MOVE TO NEXT CHAR IN ARRAY
	}
}

//COUNTER FOR ULTRASONIC
ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;		//Increment Timer Overflow count
}

//ULTRASONIC FUNCTION
void ultrasonic()
{
	
	PORTB |= (1 << Trigger_pin);/* Give 10us trigger pulse on trig. pin to HC-SR04 */
	_delay_us(10);
	PORTB &= (~(1 << Trigger_pin));
	
	TCNT1 = 0;			/* Clear Timer counter */
	TCCR1B = 0x41;		/* Setting for capture rising edge, No pre-scaler*/
	TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
	TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */

	/*Calculate width of Echo by Input Capture (ICP) on PortD PD6 */
	
	while ((TIFR1 & (1 << ICF1)) == 0);	/* Wait for rising edge */
	TCNT1 = 0;			/* Clear Timer counter */
	TCCR1B = 0x01;		/* Setting for capture falling edge, No pre-scaler */
	TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
	TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */
	TimerOverflow = 0;	/* Clear Timer overflow count */

	while ((TIFR1 & (1 << ICF1)) == 0); /* Wait for falling edge */
	count = ICR1 + (65535 * TimerOverflow);	//Take value of capture register
	distance = (int)count / (58*16); //find distance
	angle = (int)180/1200 * (OCR3A - 1200); // find the angle degree
}

//Servo Motor
void motor()
{
	OCR3A = 1200;
	//servo move forward 0-180
	for(int i=0;i<=180;i++)
	{
		OCR3A += 22;
		//angle++;
		_delay_ms(10);
	}
	//servo move backward 180-0
	for(int i=180;i>0;i--)
	{
		OCR3A -= 22;
		//angle--;
		_delay_ms(10);
	}
}

//
int main ( )
{
	/*MOTOR*/
	TCCR3A |= (1 << COM3A1) | (1 << COM3B1) | (1 << WGM31);	// Compare Output Mode, Fast PWM: Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31); 	//FAST PWM & PRESCALE 8
	DDRD |= (1 << PIND0); 									//PWM PINS AS OUT
	ICR3 = 40000; 											//FWPM FOR 20ms
	
	/*ULTRASONIC*/
	DDRB = (1<<DDB1); // PB0 is the Echo Pin & PB1 is the Trigger in
	usart_init();

	sei();					// ENABLE GLOBAL INTERRUPTS
	TIMSK1 = (1 << TOIE1); // ENABLE TIMER1 OVERFLOW INTERRUPTS
	TCCR1A = 0;

	while(1)
	{
		motor();
		ultrasonic();

		if (angle%2==0)
		{
			sprintf(angle_str, "%d", angle);
			strcat(angle_str, ",");
			sprintf(dist_str, "%d", distance);
			strcat(angle_str, dist_str);
			strcat(angle_str, ".");
			usart_print(angle_str);
			_delay_ms(20);
		}
	}
}