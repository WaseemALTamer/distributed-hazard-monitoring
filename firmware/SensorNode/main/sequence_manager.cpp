#include <Arduino.h>


#include "tick_system/tick_controller.h"
#include "constants.h"
#include "config.h"



// this file will be responsible for excuating sequence of
// instructions, it will do it in  steps each step contain
// a waiting  period before  excuating the next extraction
// thisit will  aim to do  it  without  blocking  the main
// thread 





namespace SequenceManager{





    typedef void (*STEPFunction)(void* ctx);

    struct STEP {
        STEPFunction func;
        float cooldown; // this will tell the sequence to cool down
        void* context;
    };

    int current_step_index = 0;
    STEP sequence_steps[SEQUENCE_MAX_STEPS];


    TickController tick_controller;
    
    float remaining_cooldown = 0; 
    bool update_cooldown_state(){
        // return ture if we are in a cooldown state
        // and return false if we are  in  a none if
        // we are not in a cool down state note that
        // either run this function  inside the loop
        // on the time of adding  a cooldown time or
        // flush it when you start excuating the fun
        // and then start excuating this fun

        remaining_cooldown = remaining_cooldown - tick_controller.get_elapsed();
        

        if (remaining_cooldown <= 0){
            // dont let the cool down keep going below zero
            // it may create an overflow
            remaining_cooldown = 0; 
            return false;
        }else{
            return true;
        }
    }

    void flush_steps(){
        // reset runtime state
        current_step_index = 0;
        remaining_cooldown = 0;

        // clear all steps
        for (int i = 0; i < SEQUENCE_MAX_STEPS; i++){
            sequence_steps[i].func = NULL;
            sequence_steps[i].cooldown = 0.0f;
            sequence_steps[i].context = NULL;
        }
    }
    

    void test_step_1(void* ctx){

    }

    void test_step_2(void* ctx){

    }


    void init(){
        flush_steps(); // we flush it for grabage data

        /*

        sequence_steps[0].func = test_step_1;
        sequence_steps[0].cooldown = 1.0f;
        sequence_steps[0].context = NULL;

        sequence_steps[1].func = test_step_2;
        sequence_steps[1].cooldown = 1.0f;
        sequence_steps[1].context = NULL;
        
        */


    }


    void loop(){
        if (update_cooldown_state()){
            return;
        }

        if (current_step_index >= SEQUENCE_MAX_STEPS){
            current_step_index = 0;
        }

        STEP& current_step = sequence_steps[current_step_index];

        // now we need to check if the current step exist we also
        // loop back if we need and check again if the first step
        // also exist before we try to excuate it
        if (current_step.func == NULL){
            if (current_step_index == 0){ // check if we are on the first step
                return; // no sequence to excuate
            }

            // go to the first step
            current_step_index = 0;
            return; // exit the loop wait for the next loop to exuate the next function
        }

        current_step_index ++; // go to the next step in the next successful loop
                               // note the function that  will  be  excuated  has
                               // controll over the current_step_index                
        
        current_step.func(current_step.context); // excuate the function
        tick_controller.get_elapsed(); // flush current elapsed

        remaining_cooldown = current_step.cooldown; // apply the cool down
        
    }
}