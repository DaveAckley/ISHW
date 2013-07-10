#ifndef __SIGMADELTA_H__
#define __SIGMADELTA_H__

#include <zpuino.h>
#include <zpuino-types.h>
#include "BaseDevice.h"

#define VENDOR_ZPUINO       0x08
#define PRODUCT_ZPUINO_SIGMADELTA 0x14

namespace ZPUino {
};

typedef enum { CHANNEL_LEFT, CHANNEL_RIGHT } channel_t;
typedef enum { BIG_ENDIAN, LITTLE_ENDIAN } endian_t;

/**
 * SigmaDelta class.
 */

class SigmaDelta_class: public ZPUino::BaseDevice
{
private:
public:
    /**
     * @brief Construct a new SigmaDelta_class instance.
     * @param instance Then Nth Hardware SigmaDelta instance to attach to.
     */
    SigmaDelta_class(uint8_t instance=0xff): BaseDevice(instance), m_leftPin(-1), m_rightPin(-1) {}
    /**
     * @brief Start and initialize the SigmaDelta.
     *
     *
     */
    void begin();

    /**
     * @brief Start and initialize the SigmaDelta, using the defined pins as outputs.
     */
    void begin(int pin_left, int pin_right);
    /**
     * @brief Set SigmaDelta sample endianness.
     */
    void setEndianness(endian_t endianess);
    /**
     * @brief Set the output pin for a specified channel
     */
    int setChannelPin(channel_t channel, int pin);
    /**
     * @brief Set output pins for both channels
     */
    int setPins(int left_pin, int right_pin);
    /**
     * @brief Set the output value for a single channel
     */
    void setChannelValue(channel_t channel, uint16_t value);
    /**
     * @brief Set the output value for both channels, with individual values
     */
    void setValues(uint16_t value_left, uint16_t value_right);
    /**
     * @brief Set the output value for both channels using a composite value.
     */
    void setValues(uint32_t values);
    /**
     * @brief De-initialize the SigmaDelta and release device.
     */
    void end();
private:
    int m_leftPin, m_rightPin;
};

extern SigmaDelta_class SigmaDelta;
extern SigmaDelta_class SigmaDelta1;

#endif
