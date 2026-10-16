#include <Arduino.h>

#include "sequence_manager.h"
#include "tick_system/transition_ease_in_out.h"
#include "led_controller.h"
#include "constants.h"
#include "config.h"





namespace SequenceNormal{



    void step_1(void* ctx){
        LEDController::transition_red_led(0, 1);
        LEDController::transition_green_led(1, 1);
        LEDController::transition_blue_led(0, 1);
    }


    void step_2(void* ctx){
        LEDController::transition_red_led(0, 1);
        LEDController::transition_green_led(0, 1);
        LEDController::transition_blue_led(0, 1);
    }
    
    




    void init(){

        SequenceManager::flush_steps(); // flush the steps before adding the new ones



        SequenceManager::sequence_steps[0].func = step_1;
        SequenceManager::sequence_steps[0].cooldown = 1;


        SequenceManager::sequence_steps[1].func = step_2;
        SequenceManager::sequence_steps[1].cooldown = 1;

        Config::broadcast_cooldown_duration_ms = BROADCAST_INTERVAL_NORMAL;

    }
}