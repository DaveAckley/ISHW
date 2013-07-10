#include "SigmaDelta.h"
#include "DeviceRegistry.h"

using namespace ZPUino;

void SigmaDelta_class::begin()
{
    deviceBegin(VENDOR_ZPUINO, PRODUCT_ZPUINO_SIGMADELTA);
}
void SigmaDelta_class::begin(int pin_left, int pin_right)
{
    begin();
    if (!isError()) {
        setChannelPin(CHANNEL_LEFT, pin_left);
        setChannelPin(CHANNEL_RIGHT, pin_right);
    }
}

void SigmaDelta_class::setEndianness(endian_t endianess)
{
    if (!isError()) {
        if (LITTLE_ENDIAN==endianess)
            REG(0).setBit(SDLE);
        else
            REG(0).clearBit(SDLE);
    }
}

int SigmaDelta_class::setPins(int left_pin, int right_pin)
{
    setChannelPin(CHANNEL_LEFT, left_pin);
    setChannelPin(CHANNEL_RIGHT, right_pin);
}

int SigmaDelta_class::setChannelPin(channel_t channel, int pin)
{
    int *i_pin = (channel == CHANNEL_LEFT ? &m_leftPin: &m_rightPin);

    if (!isError()) {
        if (*i_pin>=0)
            pinModePPS(*i_pin, LOW);

        int ppspin = DeviceRegistry::getPPSOutputPin( getSlot(), channel==CHANNEL_LEFT ? 0: 1);
        if (ppspin<0)
            return -1;
        pinModePPS( pin, HIGH );
        outputPinForFunction( pin, ppspin );

        *i_pin = pin;
    }
    return 0;
}

void SigmaDelta_class::setChannelValue(channel_t channel, uint16_t value)
{
    uint32_t rv = REG(1);
    if (channel==CHANNEL_LEFT) {
        rv&=0xffff;
        rv|=((uint32_t)value)<<16;
    } else {
        rv&=0xffff0000;
        rv|=value&0xffff;
    }
    setValues(rv);
}

void SigmaDelta_class::setValues(uint16_t value_left, uint16_t value_right)
{
    setValues(((uint32_t)value_left)<<16 | value_right);
}
void SigmaDelta_class::setValues(uint32_t values)
{
    REG(1)=values;
}

void SigmaDelta_class::end()
{
    if (m_leftPin>=0)
        pinModePPS(m_leftPin, LOW);

    if (m_rightPin>=0)
        pinModePPS(m_leftPin, LOW);
}

SigmaDelta_class SigmaDelta;
SigmaDelta_class SigmaDelta1;

