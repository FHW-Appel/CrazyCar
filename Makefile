
AVRDUDE_PORT = com9

TARGET = CrazyCarMain

SRC = $(TARGET).c pwm.c adc.c zyklus.c global.c uart.c df.c EinAusgabeUSB.c myFunktions.c cc-lib.c

#include Makefile.mk
include MakefileW8-10.mk
