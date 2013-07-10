#include "BaseDevice.h"

namespace ZPUino {
    int BaseDevice::deviceBegin(uint8_t vendor, uint8_t product) {
        uint8_t slot;
        slot = DeviceRegistry::scanDevice(vendor,product,m_instance);
        if (slot==NO_DEVICE) {
            /* Uuups */
            m_slot=0xff;
            return -1;
        }
        m_slot=slot;
        m_baseaddress = (register_t)IO_SLOT(m_slot);
        return 0;
    }
};
