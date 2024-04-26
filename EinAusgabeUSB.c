#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"
#include "global.h"
#include "zyklus.h"
#include "EinAusgabeUSB.h"
#include "myFunktions.h"
#include "df.h"
#include "adc.h"
#include "cc-lib.h"

#define  MAXLINKS		500
#define  MAXRECHTS	1100

void eingabeUSB(void){
//Diese Funktion liest Eingaben von der USB-Terminal-Schnittstelle ein und 
//wertet sie aus. Eine Eingabe muss mit einem Leerzeichen (Space) abschließen!
//Die eingegebenen Kommandows werden ausgewertet, z.B. "log " aktiviert 
//die Kontrollausgabe (logging) auf der USB-Terminal-Schnittstelle.

		//lese von USB solang Zeichen ein bis ein Leerzeichen kommt
		wert   = uart_getc();	//
		zeichen= (char)wert;

		if (wert != USART_NO_DATA){	//ein neues Zeichen?
		//	uart_putc(zeichen) ;			//Testausgabe
			eingabe[j] = zeichen ;
			j++;		
		}
		if (j >= MAXEINGABE) j=0;	//neue Zeichenkette einlesen		
		
		if (zeichen == ' ') {
			if (j != 1){
				eingabe[j-1] = '\0';		//Endekennung
				eingabe[j] = '\0';		//Endekennung
				//uart_putc('\r');	
				kommand = 1;	//neues Kommando 
			}
			j=0;	//neue Zeichenkette einlesen			
		}
		
		//Überprüfe das neue Kommando und führe es ggf. aus
		if (kommand == 1){ 
			//Prüfe Kommandows
			int i,zahl,n=0;
			befehl = -1;
			for (i=0; (i < anzahl) && (befehl == -1); i++){		
				for (n=0; (kommandos[i][n] == eingabe[n]) && (kommandos[i][n] != '\0'); n++);
				if (kommandos[i][n] == '\0') befehl = i;		
			}
			  //itoa(befehl,texth,10);  //Kontrollausgabe
			  //strcat(texth2,texth);
			  //uart_puts(texth2); uart_putc('\r');
			if ( !(datenSatzZaehler % 100) ){ //nur zu vollen Sekunde eintreten
				//Kommandow ausgeben
				uart_putc('\r');
				uart_puts(gleichText); uart_puts(grosserText);
				uart_putc(' ');uart_putc(' ');
				uart_puts(eingabe);		
		
				switch (befehl) {
				case 0:{	//Servo
					zahl=atoi(&eingabe[n]);
					servo(zahl);						//Ansteuerung des Servos
					break;  			
				}
				case 1: {	//Fahrtenregler
					zahl=atoi(&eingabe[n]); //SRAM sparen	
					fahr(zahl);
					break;  			
				}
				case 2: 	// Loggen auf USB-Terminal on / off
					logMod = 0;
					ausgabefaktor=2;
				
					if ((logOnOff == 1) && (eingabe[n] == '\0')) {
						logOnOff = 0;
						logOffAus = 1; //Text einmal ausgeben
						uart_puts(ausText);
					}
					else {
						logOnOff = 1;		//aktiviere Ausgabe
						logStart=0;			//sync auf ersten Datenblock
						uart_puts(anText); uart_puts(tab);
						uart_puts(modTEXT); uart_puts(gleichText); uart_putc( (char) mod+48);
					//	uart_puts(tab); 
					//	uart_puts(abtastzeitMAXText); 						
					//	itoa(abtastzeitMAX,texth,10);  
					//	uart_puts(texth);																		
						do{
							if (eingabe[n] == '2') {
								ausgabefaktor = 1; // log: 2 Werte pro Sek
								n++;
							}
							if (eingabe[n] == '1') {
								ausgabefaktor = 2; // log: 1 Werte pro Sek
								n++;
							}
							if (eingabe[n] == '\0') logMod = logMod | 0x0E; //l,v,r
							if (eingabe[n] == '-')  logMod = logMod | 0x0E; //l,v,r
							if (eingabe[n] == '+')  logMod = logMod | 0x3E; //l,v,r,s,f					
							if (eingabe[n] == 'l')  logMod = logMod | 0x02; //links, l
							if (eingabe[n] == 'v')  logMod = logMod | 0x04; //vorne, v
							if (eingabe[n] == 'r')  logMod = logMod | 0x08; //rechts, r
							if (eingabe[n] == 's')  logMod = logMod | 0x10; //Servo, s
							if (eingabe[n] == 'f')  logMod = logMod | 0x20; //Fahrtenregler, f
							if (eingabe[n] == 'm')  logMod = logMod | 0x40; //mod, m
							if ( (eingabe[n] == '+') && 
							   (eingabe[n-1] == '+') ) logMod = logMod | 0xC0; //l,v,r,s,f,m,h1,h2,h3,hchr
							n++;
						}while((eingabe[n] != '\0') && (n < 10));
						//h3 = logMod;
						
					}
					break; 					
				case 3: // Logdaten auf USB-Terminal in cm on / off
					if (logCmOnOff == 1) {
						logCmOnOff = 0;
						uart_puts(ausText);
					}
					else {
						logCmOnOff = 1;
						uart_puts(anText);
					}
					break; 
					
				case 4: //die mit hs aufgezeichneten Daten ausgeben
					if (history == 1) {
						history = 0;
						uart_puts(ausText);
					}
					else {
						history = 1;
						uart_puts(anText);
					}
					break;    
				case 5: //Daten ins EEPROM speichern
					j=0;	//neue Zeichenkette einlesen
					uart_puts(anText);	
					//abtastzeitMAX=0;		//Zyklusmessung zurücksetzen
					historyDepthPointer=0; 		//neue Datenaufzeichung
					hspStart=1;			// beginne Speicherung bein naechsten Block
					hmod = 'o';				  // Standard fuer History, l,v,r,s,f
					hisDepth=HISTORYDEPTH; // Standard fuer History, in global.h
					if (((eingabe[n] >= '1') && (eingabe[n] <= '6')) || (eingabe[n] == '+') ){

						if ((eingabe[n] >= '1') && (eingabe[n] <= '6')){
							hisDepth =  atoi(&eingabe[n]) * 10;
						}
						if ((eingabe[n+1] >= '1') && (eingabe[n+1] <= '6')){
							hisDepth =  atoi(&eingabe[n]) * 10;
						}
						if ((eingabe[n] == '+') || (eingabe[n+1] == '+')){
							hmod = '+';
							
						}
						else hmod = 'o';						
					}
					if ((hmod == '+') && (hisDepth > 60)) {
							hisDepth = 60;
					}
					
					break;    
				case 6: {	//Mod wechseln
					if((eingabe[n] > 47) && (eingabe[n] < 58)){ //dann ist es eine Ziffer
						mod = (int8_t) eingabe[n] - 48;
						ledPB2(0);	//LED PB2 aus
						strcpy(texth,tab); uart_puts(texth);
						uart_puts(modTEXT); uart_puts(gleichText); uart_putc( (char) mod+48); 
					}
					else{
						uart_puts(gleichText); uart_puts(grosserText);
						uart_puts(textF);
					}
					break;  			
				}	
				case 7: {	//Bluetooth betrieb, kurze Ueberschrift
				if (logBTOnOff == 1) {
						logBTOnOff = 0;
						strcpy(tab,"\t\0");
						uart_puts(ausText);
					}
					else {
						logBTOnOff = 1;
						strcpy(tab,"   \0");
						uart_puts(anText);	
					}
					break;  			
				}	
				case 8: { //my = eigene Daten
						n++;
						int8_t start = n, i= 0, maxSt=5;
						int16_t wert;	
					
					if ( ((eingabe[n] <'0') || (eingabe[n] > '9'))  ){
						if ( (eingabe[n] != '+' ) && (eingabe[n] != '-' )){
							uart_puts(tab); uart_puts(textF1);
							break;	//beenden, keine sinnvolle Eingabe
						}
						else{
							texth[i] = eingabe[n];
							n++; i++; maxSt++;
						}
					}

			//		do{
						
						for ( ; (i < maxSt) && (eingabe[n] >= '0') && (eingabe[n] <= '9'); n++, i++){
							texth[i] = eingabe[n];
						};
						texth[i] = '\0';
						wert = atoi( texth );
						uart_puts(tab);
						switch(eingabe[start-1]){
							case 'a' : mya = wert; uart_puts(myTEXT); uart_puts("a="); break;
							case 'b' : myb = wert; uart_puts(myTEXT); uart_puts("b="); break;
							case 'c' : myc = wert; uart_puts(myTEXT); uart_puts("c="); break;
							default:   uart_puts(textF); uart_puts(tab); break;
						}
						itoa(wert,texth,10);   
						strcat(texth,tab);
						uart_puts(texth);	
						
			//		}while((eingabe[n] != '\0') && (n < 18));
					break; 
				}
					
				default: { //uart_puts(kommandoText);
					strcpy(texth,tab); uart_puts(texth);uart_puts(tab);
					uart_puts(textF); break;}     // für den Fall einer falschen Zeile
				} //Ende switch-case
											
				uart_putc('\r');
				
				kommand = 0;	// Kommando abgearbeitet
			}			
		}	
}

int pruefeWerteBereich(int unten, int oben, int wert){
//Rückgabe = 1 , wenn wert gößerbzw gleich unten und zuätzlich 
//kleine bzw. gleich oben ist
// 	
	if (( wert >= unten) && (wert <= oben)) return 1;
	else return 0;
}

void ausgabeUSB2(void){
	//Die Funktion gibt folgende Datensatz je 0,5Sekunden über die USB-Terminal-Schnittstelle aus:
	//Analogwerte der Sensoren rechts. links und vorne
	//die Werte für den Fahrtenregler und den Servo
	//ggf. den AKKU-Analogwerte
	//es können zusätzliche Werte ausgegeben erden
		
	// Es fehlt die Mittelwertbildung,		//??	
	// die Ausgabe der Senorwerte in cm
	// das festlegen von Schwellwerten der Akku-Spannung

	if(logBTOnOff == 0)
		// Ausgabe der Überschrift, in verschiedenen Zyklen	
			switch(datenSatzZaehler % (1000)){
				case 21: uart_puts("\rt in s\t");	break;
				case 23: if (logMod & 0x02) uart_puts("links\t");	break;
				case 25: if (logMod & 0x04) uart_puts("vorne\t");	break;
				case 27: if (logMod & 0x08) uart_puts("rechts\t");	break;
				case 30: if (logMod & 0x10) uart_puts("Servo\t"); break;
				case 33: if (logMod & 0x20) uart_puts("Fahr\t");  break;
				case 35: if (analogwertAkku < AKKUGRENZWERT){ 
								uart_puts(akkuText); uart_puts("\t");
							}	 												break;
				//case 39: if (logMod & 0x40) uart_puts("Mod\t");	break;
				case 41: if (logMod & 0x80) uart_puts("a1\t");	break;
				case 43: if (logMod & 0x80) uart_puts("a2\t");	break;	
				case 47: if (logMod & 0x80) uart_puts("a3\t");	break;
				case 48: if (logMod & 0x80) uart_puts("aT");		break;
				
				case 49: uart_putc('\r');	break;
				default:	break;
			}
	
//*		Berechnungen in verschiedenen Zyklen
		switch(datenSatzZaehler % MWERTE){
			case 0: ; break;
			case 1: OutL  += OutHL;break;
			case 2: OutR  += OutHR;break;
			case 3: OutV  += OutHV;break;
			case 4: OutS  += OutHS;break;
			case 5: OutFR += OutHFR;break;
			case 6: OutH1 += OutHH1;break; // 
			case 7: OutH2 += OutHH2;break; // 
			case 8: OutH3 += OutHH3;break;
			default: break;
		}

//		*/
		if ( (!(datenSatzZaehler % (50*ausgabefaktor))) &&
					(logStart > 0) )  { //50 = 0,5Sekunden
			OutL  /=  5;	// Mittelwertbildung
			OutR  /=  5;		
			OutV  /=  5;
			OutS  /=  5;
			OutFR /=  5;
			OutH1 /=  5;
			OutH2 /=  5;
			OutH3 /=  5;
				
			if(logCmOnOff == 1) {
				//Linearisierung der Analogwerte mittels einer Hyperbel 2. Ordnung
				//x0,7 Sensor schaut im Winkel von 45° auf die Wand
				// SensorWinkel... als globale Vatiable in global.h definiert
				OutL = linearisierungAD (OutL,SensorWinkelLinks);
				OutR = linearisierungAD (OutR,SensorWinkelRechts);
				OutV = linearisierungAD (OutV,SensorWinkelGeradeaus);
//					historyAverage1OutEx = linearisierungAD (historyAverage1OutEx,10);	
			}
	
			itoa(datenSatzZaehler/100,texth,10);	// wandle die Ausgabezeit (interger) in acsii um
			strcat(texth,tab);						// Text anhaengen
			uart_puts(texth);
				
			if (logMod & 0x02){	
				itoa(OutL,texth,10);  // Sensor Links
				strcat(texth,tab);	
				uart_puts(texth);
			}
			if (logMod & 0x04){				
				itoa(OutV,texth,10);  // Sensor Vorne		
				strcat(texth,tab);	
				uart_puts(texth);
			}
			if (logMod & 0x08){				
				itoa(OutR,texth,10);  // Sensor Rechts		 
				strcat(texth,tab);	
				uart_puts(texth);
			}
			if (logMod & 0x10){				
				itoa(getServoM((int16_t)OutS),texth,10);  			// Servo / Lenkung			 
				strcat(texth,tab);	
				uart_puts(texth);
			}
			if (logMod & 0x20){				
				itoa(getFahrM((int16_t)OutFR),texth,10);  			// Antrieb / Fahrtenregler			 
				strcat(texth,tab);
				uart_puts(texth);
			}
			if (analogwertAkku < AKKUGRENZWERT){	
				itoa(analogwertAkku,texth,10);  // Akku, analog			 
				strcat(texth,tab);
				uart_puts(texth);	
			}
	
		//	if (logMod & 0x40){	
		//		itoa(mod,texth,10);   
		//		strcat(texth,tab);
		//		uart_puts(texth);	
		//	}
			if (logMod & 0x80){	
				itoa(ausgabe1,texth,10);   
				strcat(texth,tab);
				uart_puts(texth);	
				
				itoa(ausgabe2,texth,10);   
				strcat(texth,tab);
				uart_puts(texth);	
				
				itoa(ausgabe3,texth,10);   
				strcat(texth,tab);
				uart_puts(texth);	
				
				uart_puts(ausgabeT);		
			}			
							
			if ( (datenSatzZaehler % (50*ausgabefaktor)) == 0 ){
				uart_putc('\r');			
			}		
		}
		if ( !(datenSatzZaehler % 50) ) {
			//Mittelwerte für neue Berechung auf 0 zurücksetzen 
			OutL  = 0;
			OutR  = 0;		
			OutV  = 0;
			OutS  = 0;
			OutFR = 0;
			OutH1 = 0;
			OutH2 = 0;
			OutH3 = 0;
			
			logStart=1;
		}	
}

void ausgabehistory(void){
	uart_putc('\r');
	uart_puts("Historyausgabe: ");
	uart_puts(tab); 
	uart_puts(abtastzeitMAXText); uart_puts(gleichText);
	
	convertUint.Byte[0] = EEPReadByte(&historyZzMAX[0]);
	convertUint.Byte[1] = EEPReadByte(&historyZzMAX[1]);					
	itoa(convertUint.Word,texth,10);  
	uart_puts(texth);						
	uart_putc('\r');

	uart_puts("\rt in 0,1s\tlinks\tvorne\trechts\tServo\tFahrr");	
	if (hmod == '+')  uart_puts("\th1\th2\th3");	
	uart_putc('\r');
	for(int i=0; i < historyDepthPointer; i++){
			
		itoa(i,texth,10);	  // wandle die Ausgabezeit (interger) in acsii um
		strcat(texth,tab);  // Text anhaengen
		uart_puts(texth);
		
		if (logCmOnOff == 0) {
			convertUint.Byte[0] = EEPReadByte(&historyDepthL[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthL[i][1]);
			itoa(convertUint.Word,texth,10);  // Sensor Links, analog 
			strcat(texth,tab);	
			uart_puts(texth);

			convertUint.Byte[0] = EEPReadByte(&historyDepthV[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthV[i][1]);
			itoa(convertUint.Word,texth,10);  // Sensor Vorne, analog		 
			strcat(texth,tab);	
			uart_puts(texth);

			convertUint.Byte[0] = EEPReadByte(&historyDepthR[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthR[i][1]);
			itoa(convertUint.Word,texth,10);  // Sensor Rechts, analog			 
			strcat(texth,tab);	
			uart_puts(texth);			
		}
		else{
			convertUint.Byte[0] = EEPReadByte(&historyDepthL[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthL[i][1]);
			itoa(linearisierungAD(convertUint.Word,70),texth,10);  // Sensor Links, analog	 
			strcat(texth,tab);	
			uart_puts(texth);

			convertUint.Byte[0] = EEPReadByte(&historyDepthV[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthV[i][1]);
			itoa(linearisierungAD(convertUint.Word,100),texth,10);  // Sensor Vorne, analog		 
			strcat(texth,tab);	
			uart_puts(texth);
		
			convertUint.Byte[0] = EEPReadByte(&historyDepthR[i][0]);
			convertUint.Byte[1] = EEPReadByte(&historyDepthR[i][1]);		
			itoa(linearisierungAD(convertUint.Word,70),texth,10);  // Sensor Rechts, analog
			strcat(texth,tab);	
			uart_puts(texth);

		}
		convertUint.Byte[0] = EEPReadByte(&historyDepthS[i][0]);
		convertUint.Byte[1] = EEPReadByte(&historyDepthS[i][1]);				
		itoa(getServoM(convertUint.Word),texth,10);  			// Servo / Lenkung		 
		strcat(texth,tab);	
		uart_puts(texth);	

		convertUint.Byte[0] = EEPReadByte(&historyDepthFR[i][0]);
		convertUint.Byte[1] = EEPReadByte(&historyDepthFR[i][1]);			
		itoa(getFahrM(convertUint.Word),texth,10);  			// Antrieb / Fahrtenregler		 
		strcat(texth,tab);	
		uart_puts(texth);		
		
		if (hmod == '+') {
			convertInt.Byte[0] = EEPReadByte(&historyDepthH1[i][0]);
			convertInt.Byte[1] = EEPReadByte(&historyDepthH1[i][1]);			
			itoa(convertInt.Word,texth,10);  			// Antrieb / Fahrtenregler		 
			strcat(texth,tab);	
			uart_puts(texth);
			
			convertInt.Byte[0] = EEPReadByte(&historyDepthH2[i][0]);
			convertInt.Byte[1] = EEPReadByte(&historyDepthH2[i][1]);			
			itoa(convertInt.Word,texth,10);  			// Antrieb / Fahrtenregler		 
			strcat(texth,tab);	
			uart_puts(texth);

			convertInt.Byte[0] = EEPReadByte(&historyDepthH3[i][0]);
			convertInt.Byte[1] = EEPReadByte(&historyDepthH3[i][1]);			
			itoa(convertInt.Word,texth,10);  			// Antrieb / Fahrtenregler		 
			strcat(texth,tab);	
			uart_puts(texth);
		}
		uart_putc('\r');
	}
	uart_putc('\r');
	// abtastzeit=0; //Auf Null setzen
	// abtastzeitMAX=0;
}

void ausgabeUSBlogoff(void){
	
	if(logOffAus == 1){
		// Ausgabe in verschiedenen Zyklen	
			switch(datenSatzZaehler % (100)){
				case 3: 	uart_puts(tab);										break;
				case 5:  uart_puts(akkuText); uart_puts(gleichText);	break;
				case 7:  itoa(analogwertAkku,texth,10);					
							uart_puts(texth); uart_puts(tab);				break;
				case 9:  uart_puts(modTEXT); uart_puts(gleichText);	break;			
				case 11: itoa(mod,texth,10);					
							uart_puts(texth); uart_puts(tab);				break;
			
				case 13: uart_puts(freeRAMText);uart_puts(gleichText);break;
				case 15: itoa(freeRam(),texth,10);		break; // Verbrach von SRAM prüfen;
				case 19: uart_puts(texth); uart_puts(tab);							break;
				case 23: uart_puts(abtastzeitMAXText);uart_puts(gleichText);	break;
				case 29: itoa(abtastzeitMAX,texth,10);									break;
				case 33: uart_puts(texth); uart_putc('\r');							break;
				case 35: uart_puts(tab);													break;
				case 39: for(int i=0; i < 4; i++,uart_puts(", ")){
								uart_puts(&kommandos[i][0]);
							}; 											break;	
				case 43: for(int i=4; i < 7; i++,uart_puts(", ")){
								uart_puts(&kommandos[i][0]);
							};	break;	
				case 47: for(int i=7; i < anzahl; i++){
								uart_puts(&kommandos[i][0]);
								if (i < (anzahl-1)) uart_puts(", ");
							};	break;
				case 48: ;	break;
				
				case 49: uart_putc('\r');	break;
				case 99: logOffAus = 0;								break; //Ausgabe ausschalten
				default:	break;
			}
	}
}
