#include <Arduino.h>
#include "buttons.h"
#include "constants.h"






namespace Buttons {

    // it is  unjustified to invest  in a making  a button class since 
    // we will  only have  two buttons  for the  whole  system and the
    // button press and  release are only going  to be  done  one at a
    // time, this file is not responsible for any button functionality
    // this file only provides events upon press and release


    // buttons pins are not exposed you can only change them
    // inside this file only





    // we start from <HIGH> as it is not pressed and when the value is <LOW> it is pressed
    // this hase been changed because now am using the arudino internal risistors
    int button_danger_last_state = HIGH;
    int button_normal_last_state = HIGH; 




    const unsigned long buttons_cooldown_ms = 30; // this will make sure that buttons have a cool down effect
                                                  // this helps with the internal bouncing effect


    // this is the callback events you simply just attack a function you want to run when
    // the event occures to any of these call events and they will be called for you

    typedef void (*ButtonCallback)(ButtonState state); // create the call back type




    ButtonCallback danger_callbacks[MAX_BUTTON_CALLBACKS];
    int danger_callback_count = 0;

    ButtonCallback normal_callbacks[MAX_BUTTON_CALLBACKS];
    int normal_callback_count = 0;







    bool button_event_cooldown_passed() {
        // this avoid internal bounce effect for the buttons

        static unsigned long last_event_time = 0;
        unsigned long now = millis();

        if (now - last_event_time < buttons_cooldown_ms) {
            return false; // ignore event
        }

        last_event_time = now;
        return true; // accept event
    }


    void on_button_danger_event(ButtonState state){
        // state 1 is released and state 0 is pressed

        // now we can loop through the array of callbacks and simply 
        // excuateeach function provided the function exists
        for (int i = 0; i < danger_callback_count; i++) {
            if (danger_callbacks[i] != nullptr) {
                danger_callbacks[i](state);
            }
        }
    }


    void on_button_normal_event(ButtonState state){
        // state 1 is released and state 0 is pressed

        // now we can loop through the array of callbacks and simply 
        // excuateeach function provided the function exists
        for (int i = 0; i < normal_callback_count; i++) {
            if (normal_callbacks[i] != nullptr) {
            normal_callbacks[i](state);
            }
        }
    }



    void init(){ 
        // we set  up the  buttons, this  function needs to 
        // be excuated at the start to set up the pins feel 
        // free to edit the button  pins before running the
        // init 

        pinMode (BUTTON_DANGER_PIN, INPUT_PULLUP); // we use the pull up risistor inside the ardino so less componenst on pcb
        pinMode (BUTTON_NORMAL_PIN, INPUT_PULLUP);
    }


    void loop(){
        // this function should be excuated inside the loop function

        // this will  check the buttons  weather it  is up  down right or left
        // each button will get its own event 0 means it is relased and 1 means 
        // it is pressed


        // this will work by reading the current state comparing it by 

        // button cooldown will collide  with each other but this will 
        // cause no issue to our embeded system it is  better to leave
        // it as it is or invest into making a button class that takes
        // a pin  which then  needs to  be looped  through  the button
        // manager and one universial  button event  where each button
        // holds a enum that reflect what button it is


        int button_danger_current_state = digitalRead(BUTTON_DANGER_PIN);
        int button_normal_current_state = digitalRead(BUTTON_NORMAL_PIN);


        if (button_danger_last_state != button_danger_current_state){
            button_danger_last_state = button_danger_current_state;
            if (button_event_cooldown_passed())
                on_button_danger_event(static_cast<ButtonState>(button_danger_current_state));
        }
        if (button_normal_last_state != button_normal_current_state){
            button_normal_last_state = button_normal_current_state;
            if (button_event_cooldown_passed())
                on_button_normal_event(static_cast<ButtonState>(button_normal_current_state));
        }
    }








    // register functions allows you to append a function for the event
    // so your function can  be excuated when the function is triggered

    void register_danger_button_callback(ButtonCallback callback) {
        if (danger_callback_count < MAX_BUTTON_CALLBACKS) {
            danger_callbacks[danger_callback_count++] = callback;
        }
    }

    void register_normal_button_callback(ButtonCallback callback) {
        if (normal_callback_count < MAX_BUTTON_CALLBACKS) {
            normal_callbacks[normal_callback_count++] = callback;
        }
    }


}