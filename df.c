/*Werner Jessen + Dieter Rathmann */
/*Version: CrazyCar-V1.39 */
/* 25.09.2018 */

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
#include <stdint.h>
#include <avr/eeprom.h>
#include "EinAusgabeUSB.h"

uint16_t datenSatzZaehler=1;
uint16_t historyAverageL=0,historyAverageR=0,historyAverageV=0;
uint16_t historyAverageS=0,historyAverageFR=0;
union convUint hAL, hAR, hAV, hAS, hAFR;

//Hilfsvariable für eigene Daten
int16_t historyAverageH1=0; // Hilfsvariable
int16_t historyAverageH2=0; // Hilfsvariable 
int16_t historyAverageH3=0; // Hilfsvariable 
union convInt hAH1;
union convInt hAH2;
union convInt hAH3;

int8_t  historyDepthPointer=0;
uint8_t historyDepthL[HISTORYDEPTH][2]EEMEM,historyDepthR[HISTORYDEPTH][2]EEMEM,historyDepthV[HISTORYDEPTH][2]EEMEM;
uint8_t historyDepthS[HISTORYDEPTH][2]EEMEM,historyDepthFR[HISTORYDEPTH][2]EEMEM;
int8_t  historyDepthH1[HISTORYDEPTH][2]EEMEM,historyDepthH2[HISTORYDEPTH][2]EEMEM;
int8_t  historyDepthH3[HISTORYDEPTH][2]EEMEM;

uint8_t historyZzMAX[2]EEMEM;
//uint16_t historyDepthEx3[HISTORYDEPTH];

void initMikroController(void){
    //Ausgänge für Zyklustest
	DDRC |= (1<<PC1) ;

	// PWM Auagaenge fuer Motor und Lenkservo initialisieren
	DDRD |= (1<<PD4)|(1<<PD5)|(1<<PD6);

	pwm_init() ; 		//PWM Timer initialisieren
	adc_init() ; 		//Analogwandler initialisieren
	uart_init(9600); 	//serielle Schnittstelle initialisieren
	init_Timer2();   	//Timer für zyklischen Umlauf initialisieren
	sei() ;				//Interrupts freigeben

	OCR1B = STOP ;		//Geschwindigkeit B(400=schnell rückwärts/700=stop/1000=schnell vorwärts)
	OCR1A = GERADEAUS;	//A Lenkung?(750=geradeaus/950=voll rechts/550=voll links)

	uart_putc('\r');
	uart_puts(welcome) ;	// Ausgabe bei Initialisierung des uControllers
	uart_putc('\r');

	// LEDs
	DDRB |= (1 << DDB1) | (1 << DDB2); //Ausgänge LEDs
	DDRC |= (1<< DDC2) | (1<<DDC3);		//Ausgänge LEDs
	DDRC &=  ~((1 << DDC4) | (1 << DDC5) | (1 << DDC6) | (1 << DDC7)); //Eingaenge Switch
	PORTC |= ( (1<<PC4) | (1<<PC5) | (1<<PC6)| (1<<PC7));  /* Interne Pull-Up fuer beide einschalten */
	
}

void warte_sekunden(int zeit){
	// Funktion wartete die angegebene Zeit in Sekunden.
	int k = 0;
	do{
		for(int i = 1; i <= 100; i++){// warte 100 mal 10 ms
			_delay_ms(10);
		}
	}while( ++k < zeit );
}

void warte_ms(int zeit){
	// Funktion wartete die angegebene Zeit in Millisekunden.
	int k = 0;
	do{
		_delay_ms(1); 	// warte mindestens 1 ms
	}while( ++k < zeit );
}

void ledPB1(int8_t an) {
	if ( an == 0 )	PORTB = PORTB & (~(1<<PB1)); //LED PB1 aus
	else 				PORTB = PORTB | (1<<PB1);  	//LED PB1 an
}
void ledPB2(int8_t an) {
	if ( an == 0 )	PORTB &=  (~(1<<PB2));	//LED PB2 aus, kurze Version
	else 				PORTB |= (1<<PB2);		//LED PB2 an, kurze Version
}
void ledPC2(int8_t an) {
	if ( an == 0 )	PORTC &=  (~(1<<PC2));	//LED PC2 aus
	else 				PORTC |= (1<<PC2);		//LED PC2 an
}
void ledPC3(int8_t an) {
	if ( an == 0 )	PORTC &=  (~(1<<PC3));	//LED PC3 aus
	else 				PORTC |= (1<<PC3);		//LED PC3 an
}

int8_t s1(void) {
	return ((PINC & (0x1 << PINC7)) ^ (0x1 << PINC7)) >> PINC7; } //Schalter 1,PC7,PIN 29
int8_t s2(void) {
	return ((PINC & (0x1 << PINC6)) ^ (0x1 << PINC6)) >> PINC6; } //Schalter 2,PC6,PIN 28
int8_t s3(void) {
	return ((PINC & (0x1 << PINC5)) ^ (0x1 << PINC5)) >> PINC5; } //Schalter 3,PC5,PIN 27
int8_t s4(void) {
	return ((PINC & (0x1 << PINC4)) ^ (0x1 << PINC4)) >> PINC4; } //Schalter 4,PC4,PIN 26

void mittelWerteBerechnen(void){ 

	//Aufsummieren der Analogwerte
	historyAverageL = historyAverageL + analogwertL;
	historyAverageR = historyAverageR + analogwertR;
	historyAverageV = historyAverageV + analogwertV;
	historyAverageS  = historyAverageS  + OCR1A;
	historyAverageFR = historyAverageFR + OCR1B;

	if ( !(datenSatzZaehler % MWERTE)){
		OutHL = historyAverageL / MWERTE; // Mittelwerte bilden und
		OutHR = historyAverageR / MWERTE; // 10ms X 	MWERTE merken
		OutHV = historyAverageV / MWERTE;
		OutHS  = historyAverageS / MWERTE;
		OutHFR = historyAverageFR / MWERTE;

		hAL.Word = OutHL; //Werte in union speichern
		hAR.Word = OutHR; //Vorbereitung fuer EEMEM schreiben
		hAV.Word = OutHV;
		hAS.Word = OutHS;
		hAFR.Word = OutHFR;

		historyAverageL = 0; // MittelwertSpeicher zurücksetzen
		historyAverageR = 0;
		historyAverageV = 0;
		historyAverageS  = 0;
		historyAverageFR = 0;

		//Wenn  hspStart == 1, beginne die Speicherung von Daten ins EEPROM
		if (hspStart == 1) hspStart++; 
	}
}

void datenspeichern(void){  //Nur digitale Werte!
	mittelWerteBerechnen();			// Mittelwertbildung
	
	historyAverageH1 = historyAverageH1 + h1; // Aufsummieren
	historyAverageH2 = historyAverageH2 + h2; // 
	historyAverageH3 = historyAverageH3 + h3; // 
	
	if ( !(datenSatzZaehler % MWERTE)){
		OutHH1 = historyAverageH1 / MWERTE; // Mittelwerte bilden und
		OutHH2 = historyAverageH2 / MWERTE; // 10ms X 	MWERTE merken	
		OutHH3 = historyAverageH3 / MWERTE; // 
	
		hAH1.Word = ausgabe1;	//Werte in union speichern
		hAH2.Word = ausgabe2;	//Vorbereitung fuer EEMEM schreiben
		hAH3.Word = ausgabe3;	
		
		historyAverageH1  = 0; // MittelwertSpeicher zurücksetzen
		historyAverageH2  = 0;
		historyAverageH3  = 0;
		h1 = 0;
		h2 = 0;
		h3 = 0;

	}

// Mittelwerte in EEMEM Byteweise speichern,
	if ( (historyDepthPointer < hisDepth) && (hspStart > 1) ){ //old HISTORYDEPTH

		switch (datenSatzZaehler % MWERTE){
			case 0:	EEPWriteByte(&historyDepthL[historyDepthPointer][0],hAL.Byte[0] );
						if (hmod == '+') //Nur speichen wqenn gewuenscht
						 EEPWriteByte(&historyDepthH1[historyDepthPointer][0],hAH1.Byte[0] );break;
						
			case 1:	EEPWriteByte(&historyDepthL[historyDepthPointer][1],hAL.Byte[1] ); 
						if (hmod == '+') 
						 EEPWriteByte(&historyDepthH1[historyDepthPointer][1],hAH1.Byte[1] );break;
						
			case 2:	EEPWriteByte(&historyDepthR[historyDepthPointer][0],hAR.Byte[0] );
						if (hmod == '+') 
						 EEPWriteByte(&historyDepthH2[historyDepthPointer][0],hAH2.Byte[0] );break;	
						
			case 3:	EEPWriteByte(&historyDepthR[historyDepthPointer][1],hAR.Byte[1] ); 
						if (hmod == '+') 
						 EEPWriteByte(&historyDepthH2[historyDepthPointer][1],hAH2.Byte[1] );break;
						
			case 4:	EEPWriteByte(&historyDepthV[historyDepthPointer][0],hAV.Byte[0] );
						if (hmod == '+') 
						 EEPWriteByte(&historyDepthH3[historyDepthPointer][0],hAH3.Byte[0] );break;
						
			case 5:	EEPWriteByte(&historyDepthV[historyDepthPointer][1],hAV.Byte[1] ); 
						if (hmod == '+') 
						 EEPWriteByte(&historyDepthH3[historyDepthPointer][1],hAH3.Byte[1] );break;
						 
			case 6: 	EEPWriteByte(&historyDepthS[historyDepthPointer][0],hAS.Byte[0] );  break;
						
			case 7: 	EEPWriteByte(&historyDepthS[historyDepthPointer][1],hAS.Byte[1] );  break;
			case 8: 	EEPWriteByte(&historyDepthFR[historyDepthPointer][0],hAFR.Byte[0] );break;		
						
			case 9: 	EEPWriteByte(&historyDepthFR[historyDepthPointer][1],hAFR.Byte[1] );
					//	historyDepthEx3[historyDepthPointer] = hAabtastzeit;
						historyDepthPointer++; break;
						
			default: break;
		}
		if (historyDepthPointer == hisDepth) {	//längste Zykluszeit speichern
			convertUint.Word = abtastzeitMAX;
			EEPWriteByte(&historyZzMAX[0],convertUint.Byte[0] );
			EEPWriteByte(&historyZzMAX[1],convertUint.Byte[1] );
		}
	}
}
void terminal_aktiv(void){
	eingabeUSB();
	
	if ( logOnOff == 1 ){
		ausgabeUSB2(); 			//Log-Datenausgabe
	}
	else if ( logOffAus == 1) {
		ausgabeUSBlogoff();
	}

	if( !(datenSatzZaehler % 100) ) abtastzeitZyklus = 0;	//Auf 0 setzen für neuen Zyklus
	if (abtastzeit > abtastzeitZyklus) abtastzeitZyklus = abtastzeit; //Max pro MWERTE Zyklen
	if (abtastzeit > abtastzeitMAX) abtastzeitMAX = abtastzeit; //seit Start
	
	if(history == 1) {
		wert = abtastzeitMAX; 	//alter Wert merken
		ausgabehistory();				
		history = 0;				//Ausgabe der Aufzeichung fertig
		logOnOff = 0;				//Keine weiter Ausgaben
		abtastzeitMAX = wert; 	//alter Wert zurückschreiben
	}
}
void ledPB1_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPB1(1);	//LED PB1 an
		else ledPB1(0);	//LED PB1 aus
}
void ledPB2_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPB2(1);	//LED PB2 an
		else ledPB2(0);	//LED PB2 aus
}
void ledPC2_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPC2(1);	//LED PB2 an
		else ledPC2(0);	//LED PB2 aus
}
void ledPC3_flash(uint8_t flash_time,uint8_t invert ){ //10ms x flash_time
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		if (invert == 0) 	ledPC3(1);	//LED PB2 an
		else 					ledPC3(0);	//LED PB2 aus
	else
		if (invert == 0) 	ledPC3(0);	//LED PB2 aus
		else 					ledPC3(1);	//LED PB2 an
}
