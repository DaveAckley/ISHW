#ifndef __ZPUINO_INTERRUPT_H__
#define __ZPUINO_INTERRUPT_H__

#include "register.h"

static __attribute__((always_inline)) inline void sei()
{
    INTRCTL=1;
}

static __attribute__((always_inline)) inline void cli()
{
    INTRCTL=0;
}

extern void attachInterrupt(unsigned int, void (*)(void), int mode=0);
extern void detachInterrupt(unsigned int);
extern int attachInterrupt(unsigned int line, void (*function)(void*), void *arg);


#endif
