#include <Arduino.h>
#include <ESP.h>



#include "sequence_manager.h"
#include "tick_system/transition_ease_in_out.h"
#include "led_controller.h"
#include "constants.h"
#include "config.h"





namespace SequenceReeboot{



    void step_1(void* ctx){
        LEDController::transition_red_led(0, 0.2);
        LEDController::transition_green_led(0, 0.2);
        LEDController::transition_blue_led(1, 0.2);
    }

    void step_2(void* ctx){
        LEDController::transition_blue_led(0, 0.2);
    }

    void step_3(void* ctx){
        LEDController::transition_blue_led(1, 0.2);
    }

    void step_4(void* ctx){
        LEDController::transition_blue_led(0, 0.2);
    }

    void step_5(void* ctx){
        LEDController::transition_blue_led(1, 0.2);
    }

    void step_6(void* ctx){
        LEDController::transition_blue_led(0, 0.2);
    }




    void step_7(void* ctx){
        ESP.restart(); // reboot device to
    }
    




    void init(){

        SequenceManager::flush_steps(); // flush the steps before adding the new ones


        SequenceManager::sequence_steps[0].func = step_1;
        SequenceManager::sequence_steps[0].cooldown = 0.2;

        SequenceManager::sequence_steps[1].func = step_2;
        SequenceManager::sequence_steps[1].cooldown = 0.2;

        SequenceManager::sequence_steps[2].func = step_3;
        SequenceManager::sequence_steps[2].cooldown = 0.2;

        SequenceManager::sequence_steps[3].func = step_4;
        SequenceManager::sequence_steps[3].cooldown = 0.2;

        SequenceManager::sequence_steps[4].func = step_5;
        SequenceManager::sequence_steps[4].cooldown = 0.2;

        SequenceManager::sequence_steps[5].func = step_6;
        SequenceManager::sequence_steps[5].cooldown = 0.2;

        SequenceManager::sequence_steps[6].func = step_7;
        SequenceManager::sequence_steps[6].cooldown = 0.2;



    }
}