#ifndef _FUNKTIONEN1_H_
#define _FUNKTIONEN1_H_

// Funktionsprototypen
void testServo(void);
void initServo(uint16_t ml, uint16_t g, uint16_t mr);
void servo(int16_t swert );
void initFahr(uint8_t max);
void fahr(int16_t fwert );
int8_t getServo(void);
int8_t getServoM(int16_t wert_OCR1A);
int8_t getFahr(void);
int8_t getFahrM(int16_t wert_OCR1B);

void rueckwaerts(int16_t wert);
int  freeRam (void);

#endif
