#define F_CPU 16000000UL               //CPU FREQUENCY
#define BAUDRATE 9600                  //BAUDRATE
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)  //BAUD_PRESCALE FORMULA

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char str[51];       				//str char array of length 51

int adc_init(void)
{
	ADMUX |= (1 << REFS0);             	//Set ADC reference voltage to AVCC
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Enable ADC, Auto Trigger, Interrupt, Set ADC prescale
	ADCSRB = (1 << ADTS2) | (1 << ADTS1);  //SET ADC AUTO TRIGGER TO TIMER1 OVF
	while (ADCSRA & (1 << ADSC));       //WAIT TILL ADC CONVERSION FINISH
	return ADC;                         //RETURN ADC
}

void timer_init (void)
{
	TCCR1B |= (1 << CS11);             		//SET TIMER1 PRESCALE TO 8
	TIMSK1 = (1 << TOIE1);             	 	//TIMER1 OVERFLOW INTERRUPT ENABLED
	TCNT1 = 1999;                     		//SET TCNT1 TO 1999 FOR 10ms
	sei();                       			//ENABLE GLOBAL VARIABLES
}

void usart_init(void)
{
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8);  	//LOAD UBRR0 HIGH 8 BITS
	UBRR0L = (uint8_t) (BAUD_PRESCALE);   		//LOAD UBBR0 LOW BITS
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); 		//USART TRANSMITTER AND RECEIVER ENABLED
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);		//8 DATA BITS, 1 STOP BIT, NO PARITY
}

void usart_send (unsigned char ch)
{
	while (! (UCSR0A & (1<<UDRE0))); 		//WAIT TILL UDR0 IS EMPTY
	UDR0 = ch;								//TRANSMIT CHARACTER
}

void usart_print(char* ChArrPtr)
{
	while ((*ChArrPtr) != '\0')				//WHILE CHAR ARRAY ISNT EMPTY
	{
		while (! (UCSR0A & (1 << UDRE0)));  //WAIT TILL UDR0 IS EMPTY
		UDR0 = *ChArrPtr;                 	//TRANSMIT CHAR
		ChArrPtr++;                       	//MOVE TO NEXT CHAR IN ARRAY
	}
}

ISR (ADC_vect)
{
	ADCSRA |= (1 << ADSC);	           		//START ADC CONVERSION
	while ((ADCSRA & (1 << ADIF)) == 0); 	//WAIT TILL CONVERSION FINSHES
	ADCSRA |= (1 << ADIF);               	//CLEAR ADC INTERRUPT FLAG

	int ADC_VALUE = ADCL;                	//ADD ADCL TO ADC_VALUE LOW
	ADC_VALUE = ADC_VALUE | (ADCH << 8); 	//ADD ADCH TO ADC_VALUE HIGH

	int8_t i = ADC / 20.48;         		//INDEX FORMULA
	str[i] = '*';              				//STORE * IN STR[I]

	usart_print(str);           			//TRANSMIT STRING TO USART
	usart_print("\n");              		//NEWLINE
	
	str[i] = ' ';             				//CLEAR STR[I]
	TCNT1 = 1999;                      		//RESET TCNT1
}

int main(void)
{
	memset(str, ' ', 49);        		//INITIALIZE OUTPUT WITH SPACES
	str[50] = '\0';             		//NULL TERMINATE OUTPUT

	adc_init();                   			//INITIALIZE ADC
	timer_init();                  			//INITIALIZE TIMER1
	usart_init();                 			//INITIALIZE USART

	while(1);
	return 0;
}