#pragma once

#include "tick_controller.h"

#define TRANSITION_MAX_CALLBACKS 1

typedef void (*TransitionCallback)(float value, void* ctx);

struct TransitionCallbackEntry {
    TransitionCallback func;
    void* context;
};

class Uniform {
public:
    Uniform(){



    }


    

    void update() {
        if (!is_running) return;

        float elapsed_time = tick.get_elapsed(); // seconds

        if (is_reversed) {
            duration_timestamp -= elapsed_time;
        } else {
            duration_timestamp += elapsed_time;
        }

        // Clamp + stop at bounds
        if (duration_timestamp >= duration) {
            duration_timestamp = duration;
            current_value = ending_value;
            is_running = false;
        }

        if (duration_timestamp <= 0.0f) {
            duration_timestamp = 0.0f;
            current_value = starting_value;
            is_running = false;
        }

        // Linear interpolation
        float t = (duration > 0.0f) ? (duration_timestamp / duration) : 0.0f;

        current_value = starting_value +
                        t * (ending_value - starting_value);

        // Call callbacks
        for (int i = 0; i < callback_count; i++) {
            if (callbacks[i].func) {
                callbacks[i].func(current_value, 
                                  callbacks[i].context);
            }
        }
    }


    void start_transition() {
        is_reversed = false;
        is_running = true;
        tick.get_elapsed(); // flush timer
    }


    void reverse_transition() {
        is_reversed = true;
        is_running = true;
        tick.get_elapsed(); // flush timer
    }


    void stop_transition() {
        is_running = false;
    }


    void reset_transition() {
        duration_timestamp = 0.0f;
        current_value = starting_value;
    }


    void add_callback(TransitionCallback function, void* context = nullptr) {
        if (callback_count < TRANSITION_MAX_CALLBACKS) {
            callbacks[callback_count++] = {function, context};
        }
    }


    float starting_value = 0.0f;
    float ending_value = 1.0f;
    float duration = 1.0f;

    float duration_timestamp = 0.0f;
    float current_value = 0.0f;

    bool is_reversed = false;
    bool is_running = false;

private:
    TickController tick;
    TransitionCallbackEntry callbacks[TRANSITION_MAX_CALLBACKS];
    int callback_count = 0;
};