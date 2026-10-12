#include <Arduino.h>

#include "sequence_manager.h"
#include "tick_system/transition_ease_in_out.h"
#include "led_controller.h"






namespace SequenceDanger{



    void step_1(void* ctx){
        LEDController::led_red.start_transition();
        LEDController::led_green.reverse_transition();
        LEDController::led_blue.reverse_transition();
    }


    void step_2(void* ctx){
        LEDController::led_red.reverse_transition();
        LEDController::led_green.reverse_transition();
        LEDController::led_blue.reverse_transition();
    }
    




    void init(){

        SequenceManager::flush_steps(); // flush the steps before adding the new ones

        LEDController::led_red.starting_value = 0;
        LEDController::led_red.ending_value = 1;
        LEDController::led_red.duration = 1;


        LEDController::led_red.starting_value = 0;
        LEDController::led_red.ending_value = 1;
        LEDController::led_red.duration = 1;


        LEDController::led_red.starting_value = 0;
        LEDController::led_red.ending_value = 1;
        LEDController::led_red.duration = 1;




        SequenceManager::sequence_steps[0].func = step_1;
        SequenceManager::sequence_steps[0].cooldown = 1;


        SequenceManager::sequence_steps[1].func = step_2;
        SequenceManager::sequence_steps[1].cooldown = 1;

    }
}