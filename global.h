#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <inttypes.h>
#include <avr/eeprom.h>

#define ABTASTZEIT 100 // in 100us, entspricht 10ms
#define MAXEINGABE 25

#define A 22894  //z.B. 21567,Faktoren A und B ermittelt für Hyperbelangleichung
#define B -8 //z.B. 39

#define  MAXVOLLLINKS		1100
#define  MAXVOLLRECHTS		500

#define  GERADEAUS		760
//#define  VOLLLINKS		940
//#define  VOLLRECHTS		560

#define  MAXRUEWAERTS	400
#define  STOP			700
#define  MAXVORWAERTS	1000

#define  AKKUGRENZWERT	830

#define MAXTEXTH	25
#define MAXTEXTA	15
#define HISTORYDEPTH 60

#define   EEPReadByte(addr)         eeprom_read_byte((uint8_t *)addr)
#define   EEPWriteByte(addr, val)   eeprom_write_byte((uint8_t *)addr, val)
#define   EEPUpdateByte(addr, val)  eeprom_update_byte((uint8_t *)addr, val)
#define   EEPWriteBlock(__src,__dst, anz) eeprom_write_block ((const void *)__src, (void *)__dst, anz)
#define   EEPReadBlock(__dst,__src, anz) eeprom_read_block ((void *)__dst, (const void *)__src, anz)
#define   EEPUpdateBlock(__src,__dst, anz)	eeprom_update_block ((const void *)__src, (void *)__dst, anz)
#define   EEPReadWord(addr)          eeprom_read_word((const uint16_t *)addr)
#define   EEPWriteWord(addr, val)        eeprom_write_word((uint16_t *)addr,(uint16_t) val )
#define   EEPUpdateWord(addr, val)       eeprom_update_word((uint16_t *)addr,(uint16_t) val )

#define	 SensorWinkelLinks 70 		//Sensorwinkel cosinusAlpha*100
#define	 SensorWinkelRechts 70 		//Sensorwinkel cosinusAlpha*100
#define	 SensorWinkelGeradeaus 100 //Sensorwinkel cosinusAlpha*100

//Variablen zur Übergabe für Interrupts
extern uint16_t  abtastzeit,abtastzeitMAX,abtastzeitZyklus;

//  Globale Variable
extern uint16_t abstandlinks,abstandrechts,abstandvorne,analogwertAkku;
extern uint16_t analogwertL,analogwertR,analogwertV;
//extern uint16_t analogwertEx;

extern uint16_t  OutL,OutR,OutV,OutS,OutFR;;
extern uint16_t  OutHL,OutHR,OutHV,OutHS,OutHFR;

extern uint16_t  OutHH1, OutHH2, OutHH3, OutH1,OutH2,OutH3;
extern int16_t h1,h2,h3;
//extern char   hChar[];
extern char hmod;

extern char eingabe[MAXEINGABE];
extern uint8_t j,logOnOff,logOffAus,logCmOnOff,history,logBTOnOff;
extern uint8_t	 logStart;
extern uint8_t	 hspStart;

extern const int8_t  anzahl;
extern int8_t befehl, mod;
extern uint8_t  logMod, hisDepth, ausgabefaktor;
extern 	int	kommand;		// 1 wenn neues Kommando eingegeben wurde
extern const	char kommandos[9][4];


extern const char welcome[];
extern const char freeRAMText[];
extern const char abtastzeitMAXText[];
extern const char akkuText[];
extern const char gleichText[];
extern const char grosserText[];
extern const char kommandoText[];
extern const char modTEXT[];
extern const char myTEXT[];

extern 	char texth[MAXTEXTH],tab[4] ;
extern 	char anText[];
extern 	char ausText[];
extern 	char textF[];
extern 	char textF1[];

extern 	uint16_t wert;
extern 	char zeichen;
extern	int16_t servoML;
extern	int16_t servoG;
extern	int16_t servoMR;
extern	int8_t maxFahr;	//in %

//P-Regler, m = m1/m2
extern 	int16_t m1;		 //float Operation vermeiden!
extern 	int16_t m2;		 //Divisor Steigung
//extern 	int16_t b;		 //Durchbruch durch die Y-Achse, wird nicht bwnötigt
extern 	int16_t e;		 //e=Regelabweichung in cm
extern 	int16_t y;			//y=Stellgröße / Winkel
extern 	int16_t sollwert;//Sollwert 45 cm

extern union convUint
{
    uint8_t Byte[2]; //2 Byte
    uint16_t Word;  //1 Word
}convertUint;

extern union convInt
{
    uint8_t Byte[2]; //2 Byte
    int16_t Word;  //1 Word
}convertInt;

//Üebnehmen die Daten von den Kommandos mya, myb, myc, zB. mya65 myInt1=65
//extern int16_t myInt1, myInt2, myInt3;	
extern 	int16_t mya, myb, myc;
extern 	int16_t ausgabe1, ausgabe2, ausgabe3;  //Zur Ausgabe eigener Daten
extern 	char	  ausgabeT[MAXTEXTA];				//Zur Ausgabe von Text

#endif
