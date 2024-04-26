/*Werner Jessen + Dieter Rathmann */
/*Version: CrazyCar-V1.39 */

/* Anpassung
22.07.15 rueckwaerts(){	if(OCR1B >= STOP) # >= sonst von f0 aus kein Rückwerts

*/

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "zyklus.h"
#include "df.h"
#include "cc-lib.h"

void testServo(void){

	const char servoText[]="Nur + / - : ";
	const char testServoText[]="MaxWert ";

	//zum bestimmen von Anschlag rechts u. links, sowie geradeaus
	wert   = uart_getc();	// hole von USB ein Zeichen
	zeichen= (char)wert;
	if(kommand == 0) {
		uart_puts(servoText) ;
		itoa(OCR1A,texth,10);
		strcat(texth,tab);
		uart_puts(texth);
		uart_putc('\r');
		kommand = 1;
	}

	if ( (zeichen == '+') ||  ( zeichen == '-' ) ) {

		if ( zeichen == '+' ) {
			if (OCR1A > MAXVOLLRECHTS)
				OCR1A -= 4;
			else {
				uart_putc('\r');
				uart_puts(testServoText) ;
			}
		}
		if ( zeichen == '-' ) {
			if (OCR1A < MAXVOLLLINKS)
				OCR1A += 4;
			else {
				uart_putc('\r');
				uart_puts(testServoText) ;
			}
		}
		uart_puts(servoText) ;
		itoa(OCR1A,texth,10);
		strcat(texth,tab);
		uart_puts(texth);
		uart_putc('\r');

/*		uart_puts(tab);
		uart_puts(freeRAMText);
		itoa(freeRam(),texth,10);  			// Verbrach von SRAM prüfen
		strcat(texth,tab);
		uart_puts(texth);
		uart_putc('\r');
*/
	}
}

void initServo(uint16_t ml, uint16_t g, uint16_t mr){
//Speicher der Werte fuer Anschlag rechts u. links, sowie geradeaus
//unsinnige Werte blocken und durch andere ersetzen!
	if ( ml <= MAXVOLLLINKS) servoML = ml;
	else servoML = 750;
	if ( mr >= MAXVOLLRECHTS) servoMR = mr;
	else servoMR = 650;
	if (( g >= MAXVOLLRECHTS) && ( g < MAXVOLLLINKS)) servoG = g;
	else servoG = 700;
}

void servo(int16_t swert ){
	//Servo ansteuern,
	//Umsetzung von -10 bis +10 auf Servoparameter
	if(swert == 0)	      wert = servoG;       //gerade
	else if(swert >= 10)	wert = servoMR;			//max rechts
	else if(swert <= -10)	wert = servoML;		//max links
	else if(swert > 0) wert = servoG - (servoG - servoMR) / 10 * swert; //
	else 	             wert = servoG - (servoML - servoG) / 10 * swert; // links

	if ((wert > MAXVOLLLINKS) ||  (wert < MAXVOLLRECHTS)){ //Sicherheitsabfrage
		PORTC |= (1<<PC2); //LED PC2 an
		/* // bei unsinnigen Werten blinken
		for(int i=0; i < 30; i++){
			PORTB |= (1<<PB1); //LED PB1 an
			PORTB |= (1<<PB2); //LED PB2 an
			PORTC |= (1<<PC2); //LED PC2 an
			warte_ms(500);

			PORTB &= (~(1<<(PB1))); //LED PB1 aus
			PORTB &= (~(1<<(PB2))); //LED PB2 aus
			PORTC &= (~(1<<(PC2))); //LED PC2 aus
			warte_ms(500);
		}
		*/
	} else OCR1A = wert;							//Lenkwinkel setzen
}
void initFahr(uint8_t max){  //Festlegung der max Geschwindigkeit
	maxFahr = max;
}

void fahr(int16_t fwert ){  //fwert im gültigen Bereich? 
	//MAXRUEWAERTS=400, MAXVORWAERTS=1000, STOP=700
	if ((fwert < 18) && (fwert > -18)) OCR1B = STOP;
	else	if ((fwert >= 0) && (fwert <=  maxFahr)) OCR1B = STOP + 3*fwert;
	else  if ((fwert <  0) && (fwert >= -maxFahr))	rueckwaerts(STOP + 3*fwert);
	else ; //fahre weiter im der alten  Geschwindigkeit
}	

int8_t getServoPrivate(int16_t wertOCR1A){
	//Umsetzung von -10 bis +10 auf Servoparameter
	if(wertOCR1A == servoG)	      return 0;       //gerade
	else if(wertOCR1A == servoMR)	return 10;			//max rechts
	else if(wertOCR1A == servoML)	return -10;		//max links
	else if(wertOCR1A > servoG) return ((wertOCR1A - servoG) / ((servoML - servoG) / -10)); //links
	else                        return ((servoG - wertOCR1A) / ((servoG - servoMR) / 10)); //rechts
}
int8_t getServo(void){ 					 return getServoPrivate((int16_t) OCR1A); }
int8_t getServoM(int16_t wert_OCR1A){ return getServoPrivate(wert_OCR1A); };

int8_t getFahrPrivate(int16_t wertOCR1B){
	//Umsetzung von -100 bis +100 auf Fahrtenreglerparameter
	if(wertOCR1B == STOP)	      return 0;       //gerade
	else if(wertOCR1B == MAXVORWAERTS)	return 100;			//max rechts
	else if(wertOCR1B == MAXRUEWAERTS)	return -100;		//max links
	else return ((int16_t)(wertOCR1B - STOP ) / ((int16_t)(MAXVORWAERTS - STOP) / 100));
}
int8_t getFahr(void){ return getFahrPrivate(OCR1B); }
int8_t getFahrM(int16_t wert_OCR1B){ return getFahrPrivate(wert_OCR1B); }

void rueckwaerts(int16_t wert){
	if(OCR1B >= STOP){	// fährt das Fahrzeug vorwaerts?
		OCR1B = 600;	//Ja, zeige Rueckwaertsfahren an
		warte_ms(100);	//warte ms
		OCR1B = 700;	//Stop
		warte_ms(100);	//warte ms
	}
	OCR1B=wert;	// angegebene Geschwindigkeit einstellen
}

int freeRam (void) {
  extern int __heap_start, *__brkval;
  int v;
  return (int) (&v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}
