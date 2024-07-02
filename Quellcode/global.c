#include "global.h"
#include <stdint.h>
#include <avr/eeprom.h>

//Variablen zur Übergabe für Interrupts

uint16_t  abtastzeit=0,abtastzeitMAX=0,abtastzeitZyklus=0; 

//  Globale Variable
uint16_t abstandlinks,abstandrechts,abstandvorne;
uint16_t messwertL,messwertR,messwertV,messwertAkku;

uint16_t  OutL=0,OutR=0,OutV=0, OutS=0,OutFR=0;
uint16_t  OutHL=0,OutHR=0,OutHV=0, OutHS=0,OutHFR=0;

uint16_t  OutHH1=0, OutHH2=0, OutHH3=0, OutH1=0,OutH2=0,OutH3=0 ; 
int16_t h1=0,h2=0,h3=0;  //Hilfsvariable
//char   hChar[35] = {""}; //Hilfsvariable
char hmod='o';		//Wenn +, dann h1,h2,h3 ins EEPROM schreiben

char eingabe[MAXEINGABE];		//15 siehe global.h
uint8_t j=0,logOnOff = 0,logOffAus=0, logCmOnOff = 0, history = 0, logBTOnOff=0;
uint8_t	 logStart=0;
uint8_t	 hspStart=0;

//Gobale Variable für Funktion eingabeUSB()
const int8_t  anzahl = 9;
int8_t befehl, mod=0;
uint8_t logMod=0, hisDepth=HISTORYDEPTH; 
uint8_t ausgabefaktor=2; // wenn 1 u. MwWerte=10 dann alle 500ms Ausgabe mit log
int	kommand = 0;		// 1 wenn neues Kommando eingegeben wurde

const char welcome[]="CC v2.0.0";
const char freeRAMText[]="freeRAM";
const char abtastzeitMAXText[]="ZzMax";
const char akkuText[]="Akku";
const char gleichText[]="=";
const char grosserText[]=">";
const char kommandos[9][4]={"s","f","log","cm","ha","hs","mm","bt","my"};
const char modTEXT[]="Mod";
const char myTEXT[]="my";

char texth[MAXTEXTH], tab[4] ="\t\0" ;
char anText[] = "\tan" ; // log cm history
char ausText[] = "\taus" ;	// log cm history
char textF[] = "ungültig" ;
char textF1[] = "ungültig_1" ;

uint16_t wert;
char zeichen;
int16_t servoML;
int16_t servoG;
int16_t servoMR;
int8_t  maxFahr;	//in %

//P-Regler, m = m1/m2
int16_t m1=67;			 //float Operation vermeiden!
int16_t m2=100;		 //Divisor Steigung
//int16_t b=0;			 //Durchbruch durch die Y-Achse, wird nicht bwnötigt
int16_t e;				 //e=Regelabweichung in cm
int16_t y;				 //y=Stellgröße / Winkel
int16_t sollwert = 35;//Sollwert 45 cm  

union convInt  convertInt;
union convUint convertUint;

//Üebnehmen die Daten von den Kommandos mya, myb, myc, zB. mya65 myInt1=65
int16_t mya, myb, myc; //HilfsVariable
int16_t ausgabe1=0, ausgabe2=0, ausgabe3=0; //Zur Ausgabe eigener Daten
char	  ausgabeT[MAXTEXTA] = " " ;				//Zur Ausgabe von Text

