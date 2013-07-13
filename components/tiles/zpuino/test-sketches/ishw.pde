#include <BaseDevice.h>

using namespace ZPUino;

static unsigned int mytxbuf[2] = { 0xdeadbeef, 0xcafecafe };

class ISHW_class: public BaseDevice
{
public:
    ISHW_class(): BaseDevice(1)
    {
    }
    static const int STATUS_CLOCK_DETECTED = (1<<2);
    static const int STATUS_FRAME_DETECTED = (1<<3);
    static const int STATUS_INTX = (1<<4);

    int begin() {
        if (deviceBegin(0x05, 0x01)!=0) {
            Serial.println("Cannot find device");
            return -1;
        }
        rxhwbuf=0;
        rxcbuf=7;
        // TODO: reset buffers on hw
        attachInterrupt(getSlot(),&interrupt,(void*)this);
        sei();
        return 0;
    }
    void setConfig(unsigned v)
    {
        REG(0)=v;
    }

    unsigned getStatus()
    {
        return REG(1);
    }

    void ackInterrupt()
    {
        REG(1) = 1;
    }
    void setRXBuffer(int index, void *address) {
        REG(8+index) = (unsigned)address;
    }
    void setRXBufferSize(unsigned size) {
        //REG(8) = size;
    }
    unsigned getInterruptRXBuffer()
    {
        return REG(2);
    }

    void transmit(unsigned *buffer, size_t sizewords)
    {
        REG(2) = (unsigned)buffer;
        REG(3) = sizewords-1;
    }
    void handleInterrupt()
    {
        rxhwbuf=getInterruptRXBuffer();
        ackInterrupt();
    }

    static void interrupt(void*ptr)
    {
        ISHW_class *me = (ISHW_class*)ptr;
        me->handleInterrupt();
    }

    void check()
    {
        if (rxhwbuf!=rxcbuf) {
            rxcbuf++;
            rxcbuf&=7;
            if (rxhwbuf!=rxcbuf) {
                // Buffer overrun.
                Serial.print("O");
            }
            Serial.print(".");
            transmit(mytxbuf, sizeof(mytxbuf)/sizeof(unsigned));
        }
    }

    volatile unsigned rxhwbuf;
    unsigned rxcbuf;
};


ISHW_class ISHW;

unsigned char  mybuffers[8][256] __attribute__((aligned(256)));

void setup()
{
    Serial.begin(115200);
    if (ISHW.begin()!=0) {
        Serial.println("Aborting");
        for(;;);
    }
    int i;
    for (i=0;i<8;i++)
        ISHW.setRXBuffer(i,&mybuffers[i]);

    ISHW.setConfig(0x3);

}

void loop()
{
    ISHW.check();
    unsigned status = ISHW.getStatus();
    Serial.print("Status: clock ");
    Serial.print( (status & ISHW_class::STATUS_CLOCK_DETECTED) ? "OK": "FAIL");
    Serial.print(", frame ");
    Serial.print( (status & ISHW_class::STATUS_FRAME_DETECTED) ? "OK": "FAIL");
    Serial.println("");
}
