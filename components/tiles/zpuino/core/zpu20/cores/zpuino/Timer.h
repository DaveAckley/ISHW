#ifndef __TIMER_H__
#define __TIMER_H__

#include <zpuino-types.h>
#include <register.h>
#include <BaseDevice.h>

namespace ZPUino
{

    const int timerPrescalerDividers[8] = {
        1,2,4,8,16,64,256,1024
    };


    class PWM_class: public REGAccess
    {
    public:
        void setHigh(unsigned value);
        void setLow(unsigned value);
    private:
        register_t base;
    };

    /**
     * @brief Base class for any Timer implementation
     *
     * The TimerInstance_class provides low-level access to the
     * Timer configuration in hardware.
     */
    class TimerInstance_class: public REGAccess
    {
    public:
        /**
         * @brief begin using the Timer device.
         * Begin work on the specified instance. You should not use this function directly. Take a look at
         * Timer_class instead
         * @param base The base register to use
         * @param intline The interrupt line attached to this timer.
         */

        void begin(register_t base, int intline) {
            REGAccess::begin(base);
            m_intline=intline;
            /* Set up sane defaults */
            setUpDirection(true);
            setClearOnCompare(true);
        }
        /**
         * @brief Return the interrupt line attached to this timer.
         * @return The interrupt line number
         */
        int getInterruptLine() const {
            return m_intline;
        }
        /**
         * @brief Return whether this timer has prescaler support
         * @return true if prescaler is implemented, false otherwise
         */
        bool hasPrescaler() {
            return REG(128) & (1<<10);
        }
        /**
         * @brief Return whether this timer has TSC support (Time Stamp Counter)
         * @return true if TSC is implemented, false otherwise
         */
        bool hasTSC() {
            return REG(128) & (1<<8);
        }
        /**
         * @brief Return whether this timer has double buffering on the registers
         * @return true if double buffering is implemented, false otherwise
         */

        bool hasBuffers() {
            return REG(128) & (1<<9);
        }
        /**
         * @brief Return the number of PWM comparators in this timer
         * @return Number of comparators
         */
        int getNumberOfComparators() {
            return (REG(128)>>16) & 0xff;
        }
        /**
         * @brief Return the size, in bits, of this timer registers for counting and comparation purposes
         * @return The size in bits
         */
        int getSize() {
            return REG(128) & 0xff;
        }
        /**
         * @brief Set the current counter value of this timer.
         * @param value The new value for the counter.
         */
        void setCounter(unsigned value) {
            REG(ROFF_TMR0CNT)=value;
        }
        /**
         * @brief Set the current value of period comparator of this timer.
         * @param value The new value for the period comparator.
         */
        void setComparator(unsigned value) {
            REG(ROFF_TMR0CMP)=value;
        }
        /**
         * @brief Set count UP or count DOWN for this timer
         * @param value true to count up, false to count down
         */
        void setUpDirection(bool value) {
            REG(ROFF_TMR0CTL).bit(TCTLDIR) = value;
        }
        /**
         * @brief Set the clear-on-compare match for this timer.
         *
         * Set the clear-on-compare match for this timer. If enabled, this will
         * cause the counter to reset when it is equal to the period register.
         *
         * @param value true to enable clear on compare match, false to disable it
         */

        void setClearOnCompare(bool value) {
            REG(ROFF_TMR0CTL).bit(TCTLCCM) = value;
        }
        /**
         * @brief Set the period, in milisseconds, for a timer callback (periodic call)
         * @param ms the number of milisseconds requested.
         */
        int setPeriodMilliseconds(unsigned ms);
        /**
         * @brief Acknowledge a timer interrupt
         */
        void ackInterrupt()
        {
            REG(ROFF_TMR0CTL).clearBit(TCTLIF);
        }
        /**
         * @brief Set the timer clock prescaler
         */
        void setPrescaler(unsigned pres)
        {
            unsigned conf = REG(ROFF_TMR0CTL);
            conf &= 0xffff;
            conf |= (pres)<<16;
            REG(ROFF_TMR0CTL)=conf;
        }

        /**
         * @brief Start (enable) this timer
         */
        void start(bool interrupt_enabled) {
            REG(ROFF_TMR0CTL).setBit(TCTLENA);
            if (interrupt_enabled)
                REG(ROFF_TMR0CTL).setBit(TCTLIEN);
        }
        /**
         * @brief Stop (disable) this timer
         */
        void stop() {
            REG(ROFF_TMR0CTL).clearBit(TCTLENA);
        }
    private:
        //bool m_interruptAttached;
        int m_intline;
    };

    /**
     * Timer class implementation
     */
    class Timers_class: public BaseDevice
    {
    public:
        typedef int timerindex_t;
        struct TimerCallback {
            bool (*function)(void*);
            void *arg;
        };

        Timers_class();
        /**
         * @brief
         */
        void begin();
        void setPWMFrequency(long int hz);
        void setPWMOutputPin(int);
        void setPWMDuty(uint8_t val);

        int singleShot( int msec, void (*function)(void*), void *arg );
        int singleShot( int msec, void (*function)(void));
        int periodic( int msec, bool (*function)(void*), void *arg );
        int periodic( int msec, bool (*function)(void) );

        void cancel();
    private:
        int installTimerInterruptHandler(int line);
        static void timerInterruptHandler(void *arg);
        static void timerInterruptHandlerSS(void *arg);
        bool handleInterrupt();
    protected:
        void isAvailable(timerindex_t id);
        timerindex_t getFreeTimer();
        void releaseTimer(timerindex_t);
        TimerInstance_class *timer(timerindex_t id) { return &m_timers[id]; }
    protected:
        TimerInstance_class m_timers[2];
        TimerCallback m_cb[2];
        timerindex_t m_intrTimer, m_pwmTimer;
        unsigned m_timerlock;
    };
};

extern ZPUino::Timers_class Timers;
#endif
