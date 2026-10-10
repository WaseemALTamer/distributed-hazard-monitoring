#include <Arduino.h>

#include "sequence_manager.h"
#include "tick_system/transition_ease_in_out.h"
#include "led_controller.h"
#include "sequence_normal.h"


// this file is responsible for the boot sequence
// it aims to play a  one  time sequence and this
// file should be taken  as an  example of how to
// use the SequenceManager



// note  that this  sequence is  not a system the
// init function  will not be excuated during the 
// entry point  of this  application  the init is
// there so  it  adds the  sequence  function  to
// SequenceMnager nothing more nothing less


namespace SequenceBoot{


    // in the next following  steps we aim to make
    // 1 wave pattern that goes from green to blue
    // then it goes red then it goes black 



    void step_1(void* ctx){


        LEDController::led_red.start_transition();
    }


    void step_2(void* ctx){

        LEDController::led_red.reverse_transition();

        LEDController::led_green.start_transition();
        

    }


    void step_3(void* ctx){

        LEDController::led_green.reverse_transition();

        LEDController::led_blue.start_transition();
    }


    void step_4(void* ctx){
        LEDController::led_blue.reverse_transition();
    }


    void step_5(void* ctx){
        SequenceNormal::init();
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


        SequenceManager::sequence_steps[2].func = step_3;
        SequenceManager::sequence_steps[2].cooldown = 1;


        SequenceManager::sequence_steps[3].func = step_4;
        SequenceManager::sequence_steps[3].cooldown = 1;

        SequenceManager::sequence_steps[4].func = step_5;
        SequenceManager::sequence_steps[4].cooldown = 1;


    }
}