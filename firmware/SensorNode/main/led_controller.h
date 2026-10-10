#pragma once


#include "tick_system/transition_ease_in_out.h"



namespace LEDController {


    extern EaseInOut led_red;
    extern EaseInOut led_green;
    extern EaseInOut led_blue;

    void init();
    void loop();

}