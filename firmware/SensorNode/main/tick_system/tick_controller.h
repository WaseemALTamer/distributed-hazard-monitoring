#pragma once

/*
    TickController - Embedded / Arduino friendly tick system

    Features:
    - Fixed TPS (ticks per second)
    - Blocking tick timing (wait_for_tick)
    - Non-blocking elapsed timer
    - Smoothed TPS measurement
    - Portable time backend (micros by default)
*/

#include <Arduino.h>


class TickController {
public:
    explicit TickController(unsigned int target_tps = 60)
        : target_tps(target_tps),
          tick_interval_us(1000000.0f / target_tps),
          last_time_us(get_time_us()),
          elapsed_last_time_us(last_time_us),
          tps(0.0f),
          smoothing(0.1f)
    {}


    float wait_for_tick() {
        const unsigned long target_dt_us = (unsigned long)tick_interval_us;

        unsigned long now = get_time_us();
        unsigned long dt = now - last_time_us;

        while (dt < target_dt_us) {
            unsigned long remaining = target_dt_us - dt;

            // Yield CPU if enough time remains
            if (remaining > 2000) {
                delayMicroseconds((unsigned long)(remaining * 0.8f));
            }

            now = get_time_us();
            dt = now - last_time_us;
        }

        last_time_us = now;

        float dt_sec = dt / 1000000.0f;

        // Smoothed TPS calculation
        float instant_tps = (dt_sec > 0.0f) ? (1.0f / dt_sec) : 0.0f;
        tps += (instant_tps - tps) * smoothing;

        return dt_sec;
    }


    float get_elapsed() {
        unsigned long now = get_time_us();
        unsigned long dt = now - elapsed_last_time_us;
        elapsed_last_time_us = now;

        return dt / 1000000.0f;
    }

    float get_tps() const {
        return tps;
    }

    void set_target_tps(unsigned int new_tps) {
        target_tps = new_tps;
        tick_interval_us = 1000000.0f / target_tps;
    }

private:
    unsigned int target_tps;
    float tick_interval_us;

    unsigned long last_time_us;
    unsigned long elapsed_last_time_us;

    float tps;
    float smoothing;


    unsigned long get_time_us() {
#ifdef ARDUINO
        return micros();
#else
        return 0;
#endif
    }
};