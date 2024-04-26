/************************************************************************
Title:    Crazy Car Variable und Funktionen
Author:   D. Rathmann
File:     V1.39 2018/15/10 1
Software: AVR-GCC
Hardware: any AVR ,tested on ATmega32
Usage:
************************************************************************/


- int16_t mya, myb, myc; // Terminal-ImportHilfsVariable 
- int16_t ausgabe1, ausgabe2, ausgabe3; char ausgabeT[] //Zur Ausgabe eigener Daten im Terminal
																		  // und ggf. Speichung im EEPROM	

- int16_t h1,h2,h3;		 // Hilsvariable frei für eigene Daten
// Mittelwertbildung, Aktualisierung alle 100ms
-	uint16_t  OutHH1, OutHH2, OutHH3; 			// Mittelwert von h1,h2,h3 über 100ms
-  uint16_t  OutHL,OutHR,OutHV,OutHS,OutHFR; //M-Wert Sensor, Servo, Fahrtenregler über 100ms

- uint16_t analogwertL,analogwertR,analogwertV,analogwertAkku; // ADC gewandelte Werte
- uint16_t abstandlinks,abstandrechts,abstandvorne; 				// Abstandswerte in cm





- Laufzeitkontrolle:
		uint16_t abtastzeitZyklus:	max Aufenthaltsdauer im Steuerzyklusses in der letzen Sekunde
		uint16_t abtastzeitMAX:    max Aufenthaltsdauer im Steuerzyklusses seit Programmstart, 
											log: ZzMax
		
//Mittelwerte der letzeten 500ms, Sensoren, Servo, Fahrtenregler
- uint16_t  OutL,OutR,OutV,OutS,OutFR;  
