#include <avr/io.h>

//Phasen- und Frequenzrichtiger PWM Modus  (Motor)


void pwm_init(void) {
/*set timer counter initial value*/
	TCNT1 = 0 ; 
	
/*Set TOP value in the ICR1x registers */
	ICR1 = 7143 ;
	
/*start timer:
prescaler = 8; Non inverted mode in OC1A pin; Phase and Frequency Correct
PWM*/

	TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(0<<COM1B0);//|(1<<COM1A0) ;
	TCCR1B |= (1<<WGM13)|(1<<CS11);

}


