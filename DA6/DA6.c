#define F_CPU 16000000UL
#define BAUD 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)  //BAUD_PRESCALE FORMULA
#define SHIFT_REGISTER DDRB
#define SHIFT_PORT PORTB
#define DATA (1<<PB3) //MOSI (SI)
#define LATCH (1<<PB2) //SS (RCK)
#define CLOCK (1<<PB5) //SCK (SCK)

#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// capture Flag
char rT[20];
char outs[20];
volatile unsigned int ctov;
volatile uint8_t revCtr = 0;
volatile uint16_t T1Ovs2;
volatile uint32_t tickv, ticks, revTick[200];
volatile float revTime, revTickAvg, CountAvg = 0;
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8}; /* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90}; /* Segment byte maps for numbers 0 to 9 */


void init_IO(void)
{
	//Setup IO
	SHIFT_REGISTER |= (DATA | LATCH | CLOCK); //Set control pins as outputs
	SHIFT_PORT &= ~(DATA | LATCH | CLOCK); //Set control pins low
}

void init_SPI(void)
{
	//Setup SPI
	SPCR0 = (1<<SPE) | (1<<MSTR); //Start SPI as Master
}

void spi_send(unsigned char byte)
{
	SPDR0 = byte; //Shift in some data
	while(!(SPSR0 & (1<<SPIF))); //Wait for SPI process to finish
}

void USART_Init(void)
{
	UBRR0H = (uint8_t)(BAUD_PRESCALE >> 8);   //LOAD UBRR0 HIGH 8 BITS
	UBRR0L = (uint8_t)(BAUD_PRESCALE);        //LOAD UBBR0 LOW BITS
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);      //USART TRANSMITTER AND RECEIVER ENABLED
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    //8 DATA BITS, 1 STOP BIT, NO PARITY
}

char USART_RxChar()                            /* Data receiving function */
{
	while (!(UCSR0A & (1 << RXC0)));           /* Wait until new data receive */
	return (UDR0);                             /* Get and return received data */
}

void USART_TxChar(char data)                   /* Data transmitting function */
{
	UDR0 = data;                               /* Write data to be transmitting in UDR */
	while (!(UCSR0A & (1 << UDRE0)));          /* Wait until data transmit and buffer get empty */
}

void USART_SendString(char *str)               /* Send string of USART data function */
{
	int i = 0;
	while (str[i] != 0)
	{
		USART_TxChar(str[i]);                  /* Send each char of string till the NULL */
		i++;
	}
}

ISR(TIMER3_CAPT_vect)							// capture ISR
{
	tickv = ICR3; // save duration of last revolution
	uint32_t revTickSig = tickv + (T1Ovs2 * 0x10000L);
	revTick[revCtr] = revTickSig;
	revCtr++;  // add to revolution count

	if (revCtr == 200)
	{
		for (int i = 0; i <= 200; ++i)
		revTickAvg += revTick[i];
		revTickAvg = (float)revTickAvg / 200;
		revTime = (float)(60 * 1000000) / (144 * revTickAvg * 0.0625);
		snprintf(outs, sizeof(outs), "RPM: %.2f \r\n", revTime);
		snprintf(rT, sizeof(outs), "%05.2f", revTime);
		USART_SendString(outs);
		revCtr = 0;
	}
	TCNT3 = 0; // restart timer for next revolution
	T1Ovs2 = 0;
}

ISR(TIMER3_OVF_vect)   // Overflow ISR
{
	// increment overflow counter
	T1Ovs2++;
}

void StartTimer3(void)
{
	// Start timer without pre-scaler
	TCCR3B |= (1 << CS30);
	// Enable global interrupts
	sei();
}

void InitTimer3(void)
{
	// Set PE2 as input
	DDRE &= ~(1 << DDE2);
	PORTE |= (1 << DDE2);

	// Set Initial Timer value
	TCNT3 = 0;
	////First capture on rising edge
	TCCR3A = 0;
	TCCR3B = (0 << ICNC3) | (1 << ICES3);
	TCCR3C = 0;
	// Interrupt setup
	// ICIE3: Input capture
	// TOIE3: Timer3 overflow
	TIFR3 = (1 << ICF3) | (1 << TOV3);    // clear pending
	TIMSK3 = (1 << ICIE3) | (1 << TOIE3); // and enable
}

void read_adc(void)
{
	ADMUX |= (1 << REFS0);                 //Set ADC reference voltage to AVCC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0); //Enable ADC, Auto Trigger, Interrupt, Set ADC prescale

	ctov = 0;
	unsigned char i = 4;
	while (i--)
	{
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));
		ctov += ADC;
		_delay_ms(50);
	}
	ctov = ctov / 4;  // Average a few samples
	ctov = ctov * 50 / 1024; // Convert to volts
}

int main()
{
	USART_Init();
	InitTimer3();
	StartTimer3();
	init_IO();
	init_SPI();

	OCR0A = 200;
	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescale 128
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00); // TODO: Fast PWM, non-inverted
	DDRD |= (1 << DDD6); // PD2, PD1, and PD0 as outputs :  Rotate Clockwise

	while (1)
	{
		read_adc();
		OCR0A = ctov * 15;
		
		for (int i = 0; i < 4; i++)
		{
			// Extract digit from the string
			char digit_char = rT[i];
			if (digit_char == '.')
			{
				// If it's a decimal point, set the corresponding segment
				spi_send((unsigned char)0x7F);
			}
			else
			{
				// Convert the character digit to integer and get its segment byte map
				int digit = digit_char - '0';
				spi_send((unsigned char)SEGMENT_MAP[digit]);
			}
			// Send byte map to select the digit
			spi_send((unsigned char)SEGMENT_SELECT[i]);
			// Toggle latch to copy data to the storage register
			SHIFT_PORT |= LATCH;
			SHIFT_PORT &= ~LATCH;
			// Delay to control the display rate
			_delay_ms(50);
		}
	}
}