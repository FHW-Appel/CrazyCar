#include "global.h"
#include "zyklus.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//Freigabe des Timer 2 Interruptes (Comparewert erreicht) eingestellt so das jede 0,1ms ausgeführt wird
ISR( TIMER2_COMP_vect )
{
//	zeitzms++;     
	abtastzeit++;
}

//Timer 2 für zyklische Ausführung (alle ms) von Programmen nutzen
void init_Timer2(void)
{
/*set timer counter initial value*/
	TCNT2= 0x00; 
		
// Timerwert zum Rücksetzen
	OCR2 = 0x66 ;
	
//  Compare Output Mode mit Prescaler 64|(1<<CS20)
//WGM21 CTC Mode -- CS21 CLKT2s /8 prescaler -- COM2A0  Toggle OC"a on Compare Match 
	TCCR2 |= (1<<WGM01)|(1<<CS21)|(1<<COM20);  //geänder 20.12.07Je vorher WGM01
	//Interrupt für Timer 2 aktivieren
	TIMSK |= (1<<OCIE2);
}

uint16_t cnt1_top = 0x07D0;

void init_pwm(void)
{
	/*set timer counter initial value*/
	TCNT1H = 0x00; TCNT1L = 0x00 ;
	
	/*Set TOP value in the ICR1x registers */
	// Base frequency 2000 Hz
	ICR1H = (uint8_t)(cnt1_top >> 8) ; ICR1L = (uint8_t)cnt1_top ;
	
	/*Set timer output compare register auf 0*/
	OCR1AH = 0x00 ; OCR1AL = 0x0 ;
	
	// Set OCnA/B/C on compare match when counting up
	// Clear OCnA/B/C on compare when counting down
	TCCR1A |= (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0);
	// No Prescaling
	TCCR1B |= (1<<WGM13)|(1<<CS10);
}
