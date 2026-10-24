#include <Arduino.h>
#include "constants.h"

#include "tick_system/tick_controller.h"
#include "sensor_environment.h"

// this file contains the Smoke and CO sensors if the sensor grow this files needs
// to me made into two smaller files one dedcated for smoke and one dedcated for CO



// below are information that i  found for  the online  about the  sensors on which
// levels are normal moderate or dangerous, again the  modules are prove of concept
// so the ranges may vary from a room to another room 

/**
 * ============================================================================
 * DFROBOT FERMION MEMS SENSOR MAPPING SPECIFICATIONS
 * ============================================================================
 * 
 * ----------------------------------------------------------------------------
 * 1. SMOKE / ETHANOL SENSOR (SEN0570)
 * ----------------------------------------------------------------------------
 * Air Quality Status     | Voltage Range  | Raw ADC Values | Approx. Gas Data
 * -----------------------|----------------|----------------|------------------
 * Normal / Clean Air     | 0.40V - 0.90V  | 496  - 1117    | 0 to 10 PPM
 * Low Fumes (Warning)    | 1.00V - 1.50V  | 1240 - 1860    | ~50 - 150 PPM
 * Moderate Smoke         | 1.60V - 2.20V  | 1985 - 2730    | ~200 - 500 PPM
 * Heavy Smoke (Danger)   | 2.30V - 3.30V  | 2850 - 4095    | ~600 - 1000+ PPM
 * 
 * ----------------------------------------------------------------------------
 * 2. CARBON MONOXIDE SENSOR (SEN0564)
 * ----------------------------------------------------------------------------
 * Air Quality Status     | Voltage Range  | Raw ADC Values | Approx. CO Data
 * -----------------------|----------------|----------------|------------------
 * Safe / Clean Air       | 0.40V - 0.90V  | 496  - 1117    | 0 to 5 PPM
 * Elevated CO (Unsafe)   | 1.00V - 1.60V  | 1240 - 1985    | ~50 - 400 PPM
 * High CO (Evacuate!)    | 1.70V - 2.50V  | 2110 - 3100    | ~500 - 2000 PPM
 * Extreme CO (Fatal)     | 2.60V - 3.30V  | 3225 - 4095    | ~3000 - 5000+ PPM
 * 
 * Note: These conversion approximations assume a natural clean-air baseline
 * average of 0.65V. Because MEMS chemistry sensors respond logarithmically,
 * a power-law curve (y = ax^b) is used to approximate the response curves 
 * outlined in the manufacturer's datasheets.
 * ============================================================================
 */









namespace SensorManager{






    TickController tick_controller;
    float current_delta_time;


    float temperature = 0;
    float humidity = 0;
    float carbon_dioxide = 0;
    float natrual_gas = 0;
    float carbon_monoxide = 0;
    float smoke = 0;



    // the get estimate functions needs to be verfied tested
    // and potentially rewritten to be more accurate

    float get_estimate_smoke_ppm(float voltSmoke) {

        if (voltSmoke < 0.0) voltSmoke = 0.0;
        float ppm = 0.0;
        if (voltSmoke <= 0.65) {
            ppm = (voltSmoke / 0.65) * 1.0;
        } else {
            ppm = 11.5 * pow((voltSmoke - 0.65), 1.45);
        }

        if (ppm > 1000.0) ppm = 1000.0;
        return ppm;
    }

    float get_estimate_co_ppm(float voltCO) {

        if (voltCO < 0.0) voltCO = 0.0;
        float ppm = 0.0;
        if (voltCO <= 0.65) {
            ppm = (voltCO / 0.65) * 0.5;
        } else {
            ppm = 14.8 * pow((voltCO - 0.65), 2.25);
        }
        if (ppm > 5000.0) ppm = 5000.0;
        return ppm;
    }




    void init(){
        analogSetAttenuation(ADC_11db);
        SensorEnvironment::init();
    }


    void loop(){

        // ensure that this loop only runs once every 0.5 seconds
        current_delta_time += tick_controller.get_elapsed();
        if (current_delta_time <= 0.5) return;
        current_delta_time = 0;

        int rawSmoke = analogRead(SENSOR_SMOKE_PIN);
        int rawCO = analogRead(SENSOR_CARBON_MONOXIDE_PIN);

        float voltSmoke = (rawSmoke / 4095.0) * 3.3;
        float voltCO = (rawCO / 4095.0) * 3.3;

        smoke = get_estimate_smoke_ppm(voltSmoke);
        carbon_monoxide = get_estimate_co_ppm(voltCO);


        SensorEnvironment::loop(); // run the sensor Enviroment loop to get the new data

        // copy the values fro the sensor now
        humidity = SensorEnvironment::humidity;
        temperature = SensorEnvironment::temperature;
        carbon_dioxide = SensorEnvironment::carbon_dioxide;
    }


}