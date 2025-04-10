#ifndef __ZPUINO_H__
#define __ZPUINO_H__

#include "register.h"
#include "interrupt.h"

#define __ZPUINO__ 1

#ifndef _BV
#define _BV(x) (1<<(x))
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef volatile unsigned int* register_t;

extern void itoa(int value, char *dest, int base);
extern void utoa(unsigned int value, char *dest, int base);
extern char *ultoa ( unsigned long value, char * str, int base );
extern char *ltoa (long value, char * str, int base );

static inline __attribute__((always_inline)) register_t outputRegisterForPin(unsigned int pin)
{
	return &GPIODATA(pin/32);
}

static inline __attribute__((always_inline)) register_t inputRegisterForPin(unsigned int pin)
{
	return &GPIODATA(pin/32);
}

static inline __attribute__((always_inline)) register_t modeRegisterForPin(unsigned int pin)
{
	return &GPIOTRIS(pin/32);
}

static inline __attribute__((always_inline)) register_t PPSmodeRegisterForPin(unsigned int pin)
{
	return &GPIOPPSMODE(pin/32);
}

static inline __attribute__((always_inline)) unsigned int bitMaskForPin(unsigned int pin)
{
    return (1<<(pin%32));
}

static inline __attribute__((always_inline)) int digitalRead(unsigned int pin)
{
	return !!(*inputRegisterForPin(pin) & bitMaskForPin(pin));
}

static inline void __attribute__((always_inline)) outputPinForFunction(unsigned int pin,unsigned int function)
{
	GPIOPPSOUT(pin)=function;
}

static inline void __attribute__((always_inline)) inputPinForFunction(unsigned int pin,unsigned int function)
{
	GPIOPPSIN(function)=pin;
}

#ifdef ZPUINO_ACCEL

extern "C" void pinMode(unsigned int pin, int mode);
extern "C" void digitalWrite(unsigned int pin, int value);
extern "C" void pinModePPS(unsigned int pin, int value);

#else

static inline __attribute__((always_inline)) void digitalWrite(unsigned int pin, int value)
{
	if (value) {
		*outputRegisterForPin(pin) |= bitMaskForPin(pin);
	} else {
		*outputRegisterForPin(pin) &= ~bitMaskForPin(pin);
	}
}

static inline __attribute__((always_inline)) void pinMode(unsigned int pin, int mode)
{
	if (mode) {
		*modeRegisterForPin(pin) |= bitMaskForPin(pin);
	} else {
		*modeRegisterForPin(pin) &= ~bitMaskForPin(pin);
	}
}


static inline __attribute((always_inline)) void pinModePPS(int pin, int value)
{
	if (value) {
		*PPSmodeRegisterForPin(pin) |= bitMaskForPin(pin);
	} else {
		*PPSmodeRegisterForPin(pin) &= ~bitMaskForPin(pin);
	}
}


#endif


#endif
