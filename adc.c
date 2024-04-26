#include <avr/io.h>
#include <inttypes.h>

void adc_init(void) {
	DDRA = 0x00 ;//PA alle als Eingänge
	PORTA = 0x00 ;//Pull-up-Widerstand wird low gesetzt
	

//ADC Initialization
// The following code segment initializes the ADC.

    // AREF = AVcc // Set the internal 2.56 V Voltage Reference
    ADMUX = (1<<REFS1) | (1<<REFS0);
 
    // ADC Enable and prescaler of 32
    // 8000000/32 = 250000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0);
}

//Reading ADC Value
uint16_t adc(uint8_t ch)
{
  // select the corresponding channel 0~7
  // ANDing with ’7' will always keep the value
  // of ‘ch’ between 0 and 7
  ch &= 0b00000111;  // AND operation with 7
  ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
 
 
  // Store the converted value
   uint16_t val = 0;
   uint8_t i;
  for ( i=0; i<4;i++) {
	// start single convertion
	// write ’1' to ADSC
	ADCSRA |= (1<<ADSC);
 
	// wait for conversion to complete
	// ADSC becomes ’0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	val+= ADC;
  }
  val/=4;
 
  return (ADC);
}

/*
// Old 09.07.2015

void adc_init(void) {
	DDRA = 0x00 ;//PA alle als Eingänge
	PORTA = 0x00 ;//Pull-up-Widerstand wird low gesetzt
}

uint16_t adc(uint8_t admux){

   // Enable ADC, ADC Prescaler --> 8
   ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);

   // Set the channel
   ADMUX = admux;

   // Set the internal 2.56 V Voltage Reference
   ADMUX |= (1<<REFS1) | (1<<REFS0);

   // Start conversion
   ADCSRA |= (1<<ADSC);

   // Wait until conversion is complete
   while (ADCSRA & (1<<ADSC))
      ;

   // Store the converted value
   uint16_t val = ADCW;
   uint8_t i;
   // Disable the ADC
   ADCSRA &= ~(1<<ADEN);
   
   //Digitalisiere 4 Werte und bilde den Mittelwert
   val = 0;
   for ( i=0; i<4;i++) {
		ADCSRA |= (1<<ADSC);	// Start conversion
		while ( ADCSRA & (1<<ADSC)){
			;					// Wait until conversion is complete
		}
		val += ADCW;			
   }	
   // Disable the ADC
   ADCSRA &= ~(1<<ADEN);
   val /=4;

   return val;
}
*/

