#include <Arduino.h>


#include "led_controller.h"

#include "sequence_manager.h"
#include "sequence_boot.h"








void setup() {

    Serial.begin(9600);

    // the order of the intiated systems are intentional

    LEDController::init();
    SequenceBoot::init();


}


void loop() {

    // the order of updating the systems are intentional

    LEDController::loop();
    SequenceManager::loop();
    

}