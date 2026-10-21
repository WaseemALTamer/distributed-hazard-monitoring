#include <Arduino.h>



#include "device_status_manager.h"
#include "data_broadcaster.h"
#include "sequence_manager.h"
#include "led_controller.h"
#include "sensor_manager.h"
#include "sequence_boot.h"
#include "network_wifi.h"
#include "rtc_clock.h"
#include "protocol.h"
#include "buttons.h"
#include "config.h"














void setup() {

    Serial.begin(9600);

    // the order of the intiated systems are intentional

    Config::init();


    // set the device state
    DeviceStatusManager::init();
    DeviceStatusManager::set_status(DEVICE_STATUS_BOOTING);
    

    SensorManager::init();

    Buttons::init();

    

    RTCClock::init();
    LEDController::init();
    NetworkWifi::init();
    
    DataBroadcaster::init();

    

    
    
}


void loop() {

    // the order of updating the systems are intentional

    SensorManager::loop();

    Buttons::loop();
    RTCClock::loop();

    LEDController::loop();
    SequenceManager::loop();
    

    NetworkWifi::loop();

    DeviceStatusManager::loop();

    DataBroadcaster::loop();

}