#pragma once


#include "tick_system/transition_ease_in_out.h"



namespace LEDController {


    extern EaseInOut led_red;
    extern EaseInOut led_green;
    extern EaseInOut led_blue;


    extern float led_red_current_value;
    extern float led_green_current_value;
    extern float led_blue_current_value;


    void set_led_red(float value, void* ctx);
    void set_led_green(float value, void* ctx);
    void set_led_blue(float value, void* ctx);

    void transition_red_led(float value, float duration);
    void transition_green_led(float value, float duration);
    void transition_blue_led(float value, float duration);



    void init();
    void loop();

}