// this class require the Uniform class as it is build on top of it


#pragma once

#include <math.h>
#include "transition_uniform.h"


// this is here for testing perposes
inline float fast_powf(float base, float exp){
    // Handle edge cases
    if (exp == 0.0f) return 1.0f;
    if (exp == 1.0f) return base;

    // Fast path
    if (exp == 2.0f) return base * base;


    // Approximation using exp/log faster than powf
    return expf(exp * logf(base));
}


class EaseInOut {
public:
    EaseInOut(){
        // Configure underlying uniform transition (0 → 1)
        uniform.starting_value = 0.0f;
        uniform.ending_value = 1.0f;

        uniform.add_callback(on_uniform_update, this);
    }

    

    void start_transition() {
        uniform.duration = duration;
        uniform.start_transition();
    }

    void reverse_transition() {
        uniform.duration = duration;
        uniform.reverse_transition();
    }

    void stop_transition() {
        uniform.stop_transition();
    }

    void reset_transition() {
        uniform.reset_transition();
    }

    void update() {
        uniform.update();
    }

    Uniform uniform;
    
    float starting_value = 0.0f;
    float ending_value = 1.0f;
    float current_value = 0.0f;
    float duration = 1.0f;

    float damping = 2.0f;


    TransitionCallbackEntry callbacks[TRANSITION_MAX_CALLBACKS];
    int callback_count = 0;

    void add_callback(TransitionCallback function, void* context = nullptr) {
        if (callback_count < TRANSITION_MAX_CALLBACKS) {
            callbacks[callback_count++] = {function, context};
        }
    }

private:

    static void on_uniform_update(float value, void* ctx) {
        
        EaseInOut* self = (EaseInOut*)ctx;
        if (!self) return;
        
        float eased_value;

        if (value <= 0.5f) {
            eased_value = 0.5f * fast_powf(2.0f * value, self->damping);
        } else {
            eased_value = 1.0f - 0.5f * fast_powf(2.0f * (1.0f - value), self->damping);
        }

        self->current_value = self->starting_value +
                        (self->ending_value - self->starting_value) * eased_value;

        // Trigger callbacks
        for (int i = 0; i < self->callback_count; i++) {
            if (self->callbacks[i].func) {
                self->callbacks[i].func(self->current_value,
                                        self->callbacks[i].context);
            }
        }
    }
};