#ifndef _MYFUNKIONS_H_
#define _MYFUNKIONS_H_

/************************************************************************
Title:    Crazy Car Funktionen
Author:   Schuelergruppe GHO/RBZ/WHG
File:     myFunktions.h, V1.39 2018/11/06 1 $
Software: AVR-GCC
Hardware: any AVR ,tested on ATmega32
Usage:
************************************************************************/

// Funktionsprototypen
void fahren1(void);
void fahren2(void);
void fahren3(void);
void uebung1(void);
void uebung2(void);
void uebung3(void);

uint16_t  linearisierungAD(uint16_t analogwert, uint8_t cosAlpha);
void akkuSpannungPruefen(uint16_t);
void ledSchalterTest(void);
int16_t ro(void);
//int16_t  pReglerServoRechts(void);


#endif

