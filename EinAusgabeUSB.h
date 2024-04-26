#ifndef _EinAusgabeUSB_H_
#define _EinAusgabeUSB_H_


/************************************************************************
Title:    Crazy Car Funktionen
Author:   Dieter Rathmann
File:     EinAusgabeUSB.h, V1.39 1 $
Software: AVR-GCC
Hardware: any AVR ,tested on ATmega32
Usage:
************************************************************************/

// Funktionsprototypen
void ausgabeUSB2(void);
void ausgabehistory(void);
void eingabeUSB(void);

int pruefeWerteBereich(int unten, int oben, int wert);
void ausgabeUSBlogoff(void);


#endif

