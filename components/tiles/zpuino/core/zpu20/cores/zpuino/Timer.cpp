#include <Timer.h>
#include <register.h>
#include <zpuino.h>

#define VENDOR_ZPUINO       0x08
#define PRODUCT_ZPUINO_TIMERS 0x13

namespace ZPUino {

    Timers_class::Timers_class(): BaseDevice(1), m_intrTimer(-1), m_pwmTimer(-1), m_timerlock(0)
    {
    }

    void Timers_class::begin()
    {
        if (deviceBegin(VENDOR_ZPUINO, PRODUCT_ZPUINO_TIMERS)==0) {
            m_timers[0].begin( getBaseRegister(), getSlot() );
            m_timers[1].begin( getBaseRegister() + 64, getSlot()+1);
        }
    }

    int TimerInstance_class::setPeriodMilliseconds(unsigned ms) {
        /* Shifting more than 31 is not valid */
        unsigned size=getSize();
        unsigned maxvalue = size>=32 ? -1: (1<<size)-1;
        unsigned cmp=0,pres=0;
        if (hasPrescaler()) {
            int i;
            for (i=0;i<8;i++) {
                unsigned long count = CLK_FREQ/(timerPrescalerDividers[i]*1000); /* This is one milissecond */
                cmp = (count * ms)-1;
                pres=i;
                if (cmp<maxvalue)
                    break;
                cmp=0;
            }
        } else {
            unsigned long count = CLK_FREQ; /* This is one second */
            cmp = count/ms;
            if (cmp>maxvalue)
                cmp=0;
        }
        if (cmp==0)
            return -1;

        setPrescaler(pres);
        setComparator(cmp);
        setCounter(0);
        setUpDirection(true);
        return 0;
    }

#if 0
    void TimerClass::setPWMDuty(uint8_t val)
    {
        long cmp = count;
        cmp *= val;
        cmp >>= 8;
        //	TMR1OCR=cmp;
    }

    void TimerClass::setPWMFrequency(long int hz)
    {
        count =  (CLK_FREQ / hz) -1 ;
        TMR1CNT=0;
        TMR1CMP=count;
        //  TMR1OCR=count>>1;
        TMR1CTL &= ~(BIT(TCTLCP0)|BIT(TCTLCP1)|BIT(TCTLCP2));
        //TMR1CTL |= (i<<TCTLCP0);
    }

    void TimerClass::setPWMOutputPin(int pin)
    {
        pinMode(pin,OUTPUT);
        pinModePPS(pin,HIGH);
        outputPinForFunction( pin, IOPIN_TIMER1_OC);
    }
#endif

    int Timers_class::singleShot( int msec, void (*function)(void*), void *arg )
    {
        /* get a free timer */
        timerindex_t tmr = getFreeTimer();

        if (tmr<0)
            return -1;

        m_intrTimer = tmr;
        if (attachInterrupt(timer(tmr)->getInterruptLine(), &timerInterruptHandlerSS, (void*)this)) {
            releaseTimer(m_intrTimer);
            m_intrTimer = -1;
            return -2;
        }
        if (timer(tmr)->setPeriodMilliseconds(msec)<0) {
            detachInterrupt(timer(tmr)->getInterruptLine());
            releaseTimer(m_intrTimer);
            m_intrTimer = -1;
            return -3;
        }
        m_cb[tmr].function = (bool(*)(void*))function;
        m_cb[tmr].arg = arg;
        timer(tmr)->start(true);
        sei();
        return tmr;
    }

    int Timers_class::singleShot( int msec, void (*function)(void))
    {
        return singleShot(msec, (void(*)(void*))function, 0);
    }

    int Timers_class::periodic( int msec, bool (*function)(void*), void *arg )
    {
        timerindex_t tmr = getFreeTimer();

        if (tmr<0)
            return -1;

        m_intrTimer = tmr;
        if (attachInterrupt(timer(tmr)->getInterruptLine(), &timerInterruptHandler, (void*)this)<0) {
            releaseTimer(m_intrTimer);
            m_intrTimer = -1;
            return -2;
        }
        if (timer(tmr)->setPeriodMilliseconds(msec)<0) {
            detachInterrupt(timer(tmr)->getInterruptLine());
            releaseTimer(m_intrTimer);
            m_intrTimer = -1;
            return -3;
        }
        m_cb[tmr].function = function;
        m_cb[tmr].arg = arg;
        timer(tmr)->start(true);
        sei();
        return tmr;
    }

    int Timers_class::periodic( int msec, bool (*function)(void) )
    {
        return periodic(msec, (bool(*)(void*))function, 0);
    }

    void Timers_class::cancel()
    {
        if (m_intrTimer<0)
            return;
        timer(m_intrTimer)->stop();
        detachInterrupt(timer(m_intrTimer)->getInterruptLine());
        releaseTimer(m_intrTimer);
        m_intrTimer=-1;
    }


    void Timers_class::timerInterruptHandler(void *arg)
    {
        Timers_class *hclass = static_cast<Timers_class*>(arg);
        if (!hclass->handleInterrupt()) {
            hclass->timer(hclass->m_intrTimer)->stop();
            detachInterrupt(hclass->timer(hclass->m_intrTimer)->getInterruptLine());
            hclass->releaseTimer(hclass->m_intrTimer);
            hclass->m_intrTimer=-1;
        }
        hclass->timer(hclass->m_intrTimer)->ackInterrupt();
    }

    void Timers_class::timerInterruptHandlerSS(void *arg)
    {
        Timers_class *hclass = static_cast<Timers_class*>(arg);
        hclass->timer(hclass->m_intrTimer)->stop();
        hclass->handleInterrupt();
        detachInterrupt(hclass->timer(hclass->m_intrTimer)->getInterruptLine());
        hclass->m_intrTimer=-1;
        // Ack interrupt
        hclass->timer(hclass->m_intrTimer)->ackInterrupt();

    }

    bool Timers_class::handleInterrupt()
    {
        if (m_intrTimer<0)
            return false;
        if (m_cb[m_intrTimer].function!=0) {
            return m_cb[m_intrTimer].function(m_cb[m_intrTimer].arg);
        }
        return false;
    }

    Timers_class::timerindex_t Timers_class::getFreeTimer()
    {
        if (!(m_timerlock&1)) {
            m_timerlock|=1;
            return 0;
        }
        if (!(m_timerlock&2)) {
            m_timerlock|=2;
            return 1;
        }
        return -1;
    }
    void Timers_class::releaseTimer(timerindex_t idx)
    {
        m_timerlock &= ~(1<<idx);
    }

};

ZPUino::Timers_class Timers;
