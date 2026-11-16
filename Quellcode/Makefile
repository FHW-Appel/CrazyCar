AVRDUDE_PORT = com3

TARGET = CrazyCarMain

SRC = $(TARGET).c pwm.c adc.c zyklus.c global.c uart.c df.c EinAusgabeUSB.c myFunktions.c cc-lib.c

# Check if we are on Windows or not and define variables
ifeq ($(OS),Windows_NT)
  AVRDUDE_PROGRAMMER = avr910
else
  AVRDUDE_PROGRAMMER = stk500v1
  AVRDUDE_PORT := $(shell ../.vscode/find_serial_port.sh)

endif

# Include Makefile that works on both Windows and Linux
include Makefile.mk