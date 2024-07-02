#ifndef _MYFUNKIONS_H_
#define _MYFUNKIONS_H_

// Funktionsprototypen
void fahren1(void);
void fahren2(void);
void fahren3(void);
void uebung1(void);
void uebung2(void);
void uebung3(void);

uint16_t  linearisierungAD(uint16_t messwert, uint8_t cosAlpha);
void akkuSpannungPruefen(uint16_t);
void ledSchalterTest(void);
int16_t ro(void);
//int16_t  pReglerServoRechts(void);


#endif

