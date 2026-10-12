#ifndef DEVICE_STATUS_MANAGER_H
#define DEVICE_STATUS_MANAGER_H

#include <Arduino.h>
#include "protocol.h"

namespace DeviceStatusManager {

    void set_status(
        SetDeviceStatus content,
        IPAddress senderIP,
        uint16_t senderPort
    );

    void init();

}

#endif