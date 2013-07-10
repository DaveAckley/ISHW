#include "zpuino.h"
#include "zpuino-types.h"
#include "HardwareSerial.h"
/* Most stuff is in zpuino-accel.S */

#define ZPUINO_MAX_INTERRUPTS 16

struct interrupt_type_t {
    void(*func)(void*);
    void*arg;
};

static interrupt_type_t itable[ZPUINO_MAX_INTERRUPTS]={0};

int attachInterrupt(unsigned int line, void (*function)(void*), void *arg)
{
    if (line>=ZPUINO_MAX_INTERRUPTS)
        return -1;
    // cli();
    itable[line].func = function;
    itable[line].arg = arg;
    // sei();

    // Set mask
    INTRMASK |= _BV(line);

    return 0;
}

void attachInterrupt(unsigned int line, void (*function)(void), int mode)
{
    attachInterrupt(line, (void(*)(void*))function, NULL);
}

void detachInterrupt(unsigned int line)
{
    if (line>=ZPUINO_MAX_INTERRUPTS)
        return;

    INTRMASK &= ~_BV(line);

    itable[line].func = 0;
}

void _zpu_interrupt(unsigned int line)
{
    if (line>=ZPUINO_MAX_INTERRUPTS)
        return;
    if (itable[line].func)
        itable[line].func(itable[line].arg);
}
