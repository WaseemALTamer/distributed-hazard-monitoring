#ifndef DEVICE_STATUS_MANAGER_H
#define DEVICE_STATUS_MANAGER_H

#include <Arduino.h>
#include "protocol.h"

namespace DeviceStatusManager {

    void set_status(int status);

    void init();
    void loop();
}

#endif