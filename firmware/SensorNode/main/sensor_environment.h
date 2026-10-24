#ifndef SENSORENVIRONMENT_H
#define SENSORENVIRONMENT_H

#include <Arduino.h>
#include "constants.h"

namespace SensorEnvironment {


    extern float humidity;
    extern float temperature;
    extern float carbon_dioxide;
    extern int16_t module_error;


    void init();
    void loop();

}

#endif // SENSORENVIRONMENT_H