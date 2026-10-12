#pragma once

#include <Arduino.h>


#include "tick_system/tick_controller.h"
#include "constants.h"
#include "config.h"

namespace SequenceManager
{

    typedef void (*STEPFunction)(void* ctx);
    struct STEP {
        STEPFunction func;
        float cooldown;
        void* context;
    };



    void init();
    void loop();

    void flush_steps();
    bool update_cooldown_state();



    extern int current_step_index;
    extern STEP sequence_steps[SEQUENCE_MAX_STEPS];

    extern TickController tick_controller;
    extern float remaining_cooldown;
}