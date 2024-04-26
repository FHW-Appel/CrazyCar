
/************************************************************************
Title:    Crazy Car Funktionen
Author:   D. Rathmann
File:     V1.39 2018/05/11
Software: AVR-GCC
Hardware: any AVR ,tested on ATmega32
Usage:
************************************************************************/
1: ist zu vervollständigen, eigner Code ist zu entwickeln
2: ist fertig und funktioniert


Information zu Variable und Funktionen im Projekt

Datei: myFunktions.c
void fahren1(void){} //steuert das CrazyCar									//1
	
uint16_t  linearisierungAD(uint16_t analogwert, uint8_t cosAlpha){} 	//1
	// berechnet aus den digitalen Werte einen Abstand in cm 
	
int16_t pReglerServoRechts(void){} oder int16_t ro(void){} 				//1
	// berechnet den Linkwinkel / Lenkfaktor zur Rückkehr auf die ideale Line
	//int16_t e;				//e=Regelabweichung in cm
	//int16_t sollwert = 35;//Sollwert
	//int16_t y;				//y=Stellgroeße / Winkel


void akkuSpannungPruefen(int schwellwert){}									//1
	// signialisiert über die LED eine AKKU-Unterspanng / AKKU ist leer
	
void ledSchalterTest(void){}														//1
	// Funktionstest für Schalter und LEDs
	
void uebung1(void) bis void uebung3(void)										//1
	//Übungen zum Erarbeiten der CC-Funktionalitäten

Datei: myFunktions.c
void warte_sekunden(int zeit){}													//2
	// susppendiert den Controller über die angegeben Sekunden 
	
void warte_ms(int zeit){}															//2
	// susppendiert den Controller über die angegeben MilliSekunden
	
void ledPB1(int8_t an) {} 	bis ledPC3(int8_t an){}							//2
	// schaltet die jeweilige LED an / aus
	
int8_t s1(void) {} bis int8_t s4(void) {}										//2
	// liest die jeweilige Schalterstellung aus
	
	
Datei: cc-lib.c
void servo(int16_t swert ){}														//2
	// setzt den Lenkeinschalt von -10 bis +10
	
void fahr(int16_t fwert ){}														//2
	// setzt der Geschindigkeit von -100 bis +100

int8_t getFahr(){}																	//2
	//z.B. getFahr(); liefert die aktuelle Geschindigkeit

int8_t getServo(){}																	//2
	//z.B. getServo(); liefert den aktuellen Lenkeinschlag 


Datei: CrazyCar-V1.39.c
int main (void){}																		//2
	//Hier, bei der ersten Zeile, beginnt das Programm 					

void autonomFahren(void){}															//2
	// muss freigegeben werden zur Aktivierung der Fahrfunktionen

Datei: EinAusgabeUSB.c
void eingabeUSB(void){}																//2
	//Liest und wertet die Terminmaleingaben aus
	
void ausgabeUSB2(void){}															//2
	//Ausgabe auf das Terminal