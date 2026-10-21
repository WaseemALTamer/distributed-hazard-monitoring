#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "constants.h"

namespace SensorManager {


    extern float temperature;
    extern float humidity;
    extern float carbon_dioxide;
    extern float natrual_gas;
    extern float carbon_monoxide;
    extern float smoke;



    void init();
    void loop();

}

#endif // SENSOR_MANAGER_H