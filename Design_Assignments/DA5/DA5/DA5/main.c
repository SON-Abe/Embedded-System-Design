#define F_CPU 16000000UL									// 16MHz clock
#define BAUDRATE 9600										// baudrate
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1) //BAUD_PRESCALEFORMULA
#define  Trigger_pin	PINB1								//trigger pin
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

int TimerOverflow = 0;
char string[20];
char angle_str[10];
char dist_str[10];
unsigned long count;
int distance;
int angle;

void usart_init(void)
{
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8); 	// LOAD UBRR0 HIGH 8 BITS
	UBRR0L = (uint8_t) (BAUD_PRESCALE); 		// LOAD UBBR0 LOW BITS
	UCSR0B = (1 << TXEN0); 						// USART TRANSMITTER AND RECEIVER ENABLED
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 	// 8 DATA BITS, 1 STOP BIT,NO PARITY
}

void usart_send (unsigned char ch)
{
	while (! (UCSR0A & (1<<UDRE0))); 			// WAIT TILL UDR0 IS EMPTY
	UDR0 = ch; 									// TRANSMIT CHARACTER
}

void usart_print(char* ChArrPtr)
{
	while ((*ChArrPtr) != '\0') 				// WHILE CHAR ARRAY ISNT EMPTY
	{
		while (! (UCSR0A & (1 << UDRE0))); 		// WAIT TILL UDR0 IS EMPTY
		UDR0 = *ChArrPtr; 						// TRANSMIT CHAR
		ChArrPtr++; 							// MOVE TO NEXT CHAR IN ARRAY
	}
}

//COUNTER FOR ULTRASONIC
ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;							// Increment Timer Overflow count
}

//ULTRASONIC FUNCTION
void ultrasonic()
{
	
	PORTB |= (1 << Trigger_pin);				// 10us TRIGGER PULSE
	_delay_us(10);								// DELAY
	PORTB &= (~(1 << Trigger_pin));				// FALLING EDGE
	
	TCNT1 = 0;									// CLEAR TIMER1 COUNTER
	TCCR1B = 0x41;								// CAPTURE AT RISING EDGE SETUP
	TIFR1 = 1<<ICF1;							// CLEAR INPUT CAPTURE FLAG FOR TIMER1
	TIFR1 = 1<<TOV1;							// CLEAR TIMER1 OVERFLOW
	
	while ((TIFR1 & (1 << ICF1)) == 0);			//	WAITS FOR RISING EDGE
	TCNT1 = 0;									// CLEAR TIMER1 COUNTER
	TCCR1B = 0x01;								// SET TO CAPTURE FALLING EDGE
	TIFR1 = 1<<ICF1;							// CLEAR INPUT CAPTURE FLAG FOR TIMER1
	TIFR1 = 1<<TOV1;							// CLEAR TIMER1 OVERFLOW
	TimerOverflow = 0;							// CLEARS TIMER OVERFLOW COUNTER

	while ((TIFR1 & (1 << ICF1)) == 0); 		// WAITS TILL FALLING EDGE
	count = ICR1 + (65535 * TimerOverflow);		// COUNT FORMULA
	distance = (unsigned long)count / (58*16);	// DISTANCE FORMULA

	if(angle <= 9)                    			// IF THE ANGLE IS <= 9
		dtostrf(angle, 1, 0, angle_str);      	// CONVERT ANGLE TO STRING WITH 1 DIGIT RESERVED FOR INTEGER PART AND 0 DIGITS FOR THE DECIMAL PART
	else if(angle <= 99)						// ELSE IF THE ANGLE IS <= 99
		dtostrf(angle, 2, 0, angle_str);       	// CONVERT ANGLE TO STRING WITH 2 DIGIT RESERVED FOR INTEGER PART AND 0 DIGITS FOR THE DECIMAL PART
	else                                  		// ELSE
		dtostrf(angle, 3, 0, angle_str);     	// CONVERT ANGLE TO STRING WITH 3 DIGIT RESERVED FOR INTEGER PART AND 0 DIGITS FOR THE DECIMAL PART

	strcat(angle_str, ",");         			// APPEND "," TO ANGLE_STR CHAR ARRAY

	if(distance <= 9)          					// IF THE ANGLE IS <= 9
		dtostrf(distance, 1, 0, dist_str);     	// CONVERT DISTANCE TO STRING WITH 1 DIGIT RESERVED FOR INTEGER PART AND 0 DIGITS FOR THE DECIMAL PART
	else                                		// ELSE
		dtostrf(distance, 2, 0, dist_str);   	// CONVERT DISTANCE TO STRING WITH 2 DIGITS RESERVED FOR INTEGER PART AND 0 DIGITS FOR THE DECIMAL PART

	strcat(angle_str, dist_str);    			// APPEND DIST_STR
	strcat(angle_str, ".");    					// APPEND "."
	usart_print(angle_str);   					// SEND THE CHAR ARRAY TO USART

}

//Servo Motor
void motor()
{
	angle = 0;									// START ANGLE AT 0 DEGEREES
	//servo move forward 0-180
	for(OCR3A = 1000; OCR3A<=5200; OCR3A+=23)
	{
		angle++;								// INCREASE ANGLE
		ultrasonic();							// SEND PWM
		_delay_ms(5);							// DELAY
	}

	angle = 180;								// START ANGLE AT 180 DEGREES
	//servo move backward 180-0
	for(OCR3A=5200;OCR3A>1000;OCR3A-=23)
	{
		angle--;								// DECREASE ANGLE
		ultrasonic();							// SEND PWM
		_delay_ms(5);							// DLEAY
	}
}

int main ( )
{
	/*MOTOR*/
	TCCR3A |= (1 << COM3A1) | (1 << COM3B1) | (1 << WGM31);	// Compare Output Mode, Fast PWM: Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31); 	//FAST PWM & PRESCALE 8
	DDRD |= (1 << PIND0); 									//PWM PINS AS OUT
	ICR3 = 39999; 											//FWPM = 50 HZ, T = 20ms. 40k-1 
	
	/*ULTRASONIC*/
	DDRB = (1<<DDB1); 										// PB0 is the Echo Pin & PB1 is the Trigger in
	usart_init();											// INITIALIZE USART

	sei();													// ENABLE GLOBAL INTERRUPTS
	TIMSK1 = (1 << TOIE1); 									// ENABLE TIMER1 OVERFLOW INTERRUPTS
	TCCR1A = 0;												// SET TCCR1A 0

	while(1)
		motor();											// RUN THE MOTOR FUNCTION
}
