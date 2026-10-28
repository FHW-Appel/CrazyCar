AVRDUDE_PORT = com9

TARGET = CrazyCarMain

SRC = $(TARGET).c pwm.c adc.c zyklus.c global.c uart.c df.c EinAusgabeUSB.c myFunktions.c cc-lib.c

# Check if we are on Windows or not and include corresponding Makefile
ifeq ($(OS),Windows_NT)
  include MakefileW8-10.mk
else
  include Makefile.mk
endif