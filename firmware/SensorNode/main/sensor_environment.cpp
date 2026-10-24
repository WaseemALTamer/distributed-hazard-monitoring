#include <SparkFun_SCD4x_Arduino_Library.h>
#include <Arduino.h>



#include "constants.h"






namespace SensorEnvironment {


    SCD4x mySensor;

    float humidity = 0;
    float temperature = 0;
    float carbon_dioxide = 0;
    int16_t module_error = 0;





    void init(){
        Wire.begin(SDA_PIN, SCL_PIN);
        mySensor.begin();

        mySensor.startPeriodicMeasurement();
    }


    void loop(){

        if (mySensor.readMeasurement()) {
            carbon_dioxide = (float)mySensor.getCO2();
            temperature = mySensor.getTemperature();
            humidity = mySensor.getHumidity();
        }


    }

}
