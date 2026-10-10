#include "tick_system/transition_ease_in_out.h"
#include "led_controller.h"
#include "constants.h"


namespace LEDController{

    // the scope of the project doesnt justify making a animation 
    // manager avoid doing it unless justified

    
    EaseInOut led_red;
    EaseInOut led_green;
    EaseInOut led_blue;


    void set_led_red(float value, void* ctx){
        
        int _value = (int)(255 * value);

        if (_value > 255) _value = 255;
        if (_value < 0) _value = 0;
        analogWrite(LED_RED_PIN, _value);
    }


    void set_led_green(float value, void* ctx){
        int _value = (int)(255 * value);

        if (_value > 255) _value = 255;
        if (_value < 0) _value = 0;
        analogWrite(LED_GREEN_PIN, _value);
    }


    void set_led_blue(float value, void* ctx){
        int _value = (int)(255 * value);

        if (_value > 255) _value = 255;
        if (_value < 0) _value = 0;
        analogWrite(LED_BLUE_PIN, _value);
    }



    void init(){

        // prepared the pins
        pinMode(LED_RED_PIN, OUTPUT);
        pinMode(LED_GREEN_PIN, OUTPUT);
        pinMode(LED_BLUE_PIN, OUTPUT);


        // this is not true analog this is PWM
        analogWrite(LED_RED_PIN, 0);
        analogWrite(LED_GREEN_PIN, 0);
        analogWrite(LED_BLUE_PIN, 0);


        led_red.add_callback(set_led_red);
        led_green.add_callback(set_led_green);
        led_blue.add_callback(set_led_blue);
    }
    


    void loop(){

        led_red.update();
        led_green.update();
        led_blue.update();

    }



}


