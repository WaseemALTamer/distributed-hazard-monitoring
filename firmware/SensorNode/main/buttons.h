#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>



typedef enum ButtonState {
    BUTTON_STATE_PRESSED = 0,
    BUTTON_STATE_RELEASED = 1,
} ButtonState;



namespace Buttons {



    // Callback type for button events
    typedef void (*ButtonCallback)(ButtonState state);


    // Initializes the button pins (call in setup)
    void init();

    // Call this in your main loop to update button states
    void loop();

    // Register callback functions for each button
    void register_danger_button_callback(ButtonCallback callback);
    void register_normal_button_callback(ButtonCallback callback);
}

#endif
