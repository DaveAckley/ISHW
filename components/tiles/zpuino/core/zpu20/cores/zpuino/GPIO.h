#ifndef __GPIO_h__
#define __GPIO_h__

#include <zpuino.h>
#include <zpuino-types.h>
#include "BaseDevice.h"

#define VENDOR_ZPUINO       0x08
#define PRODUCT_ZPUINO_GPIO 0x12

namespace ZPUino {

    class GPIO_class {
    public:
        GPIO_class(uint8_t instance=0xff): BaseDevice(instance) {
            deviceBegin(VENDOR_ZPUINO,PRODUCT_ZPUINO_GPIO);
        }
    };

    extern GPIO_class GPIO;
};

