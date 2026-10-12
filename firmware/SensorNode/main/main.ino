#include <Arduino.h>



#include "device_status_manager.h"
#include "sequence_manager.h"
#include "led_controller.h"
#include "sequence_boot.h"
#include "network_wifi.h"
#include "config.h"










void setup() {

    Serial.begin(9600);

    // the order of the intiated systems are intentional

    Config::init();
    DeviceStatusManager::init();
    

    LEDController::init();
    SequenceBoot::init();

    NetworkWifi::init();

    


}


void loop() {

    // the order of updating the systems are intentional

    LEDController::loop();
    SequenceManager::loop();

    NetworkWifi::loop();
    

}