// this is a base <data provider> class, this class will not access any
// of the other, this class will provide time data and it will set an






#ifndef CLOCK_H
#define CLOCK_H



#include <Arduino.h>
#include <Wire.h>


#include <Rtc_Pcf8563.h>



namespace RTCClock {


    // data types 
    typedef void (*TimeCallback)(int seconds, int minutes, int hours);
    typedef void (*DateCallback)(int day, int month, int year);
    typedef void (*AlarmCallback)();

    extern int is_alarm_active; 

    void init(); // initation function

    void loop(); // loop this is required to be run

    // functions to get the time 
    byte get_seconds();
    byte get_mintues();
    byte get_hour();


    // functions to get the date
    byte get_day();
    byte get_month();
    byte get_year();


    uint32_t get_unix_time();


    // functions to get the alarm time that is already set
    byte get_alarm_mintues();
    byte get_alarm_hour();

    // functions to set the time, date and alarm
    void set_time(byte seconds, byte mintues, byte hours);
    void set_date(byte day, byte months, byte fullYear);
    void set_alarm(byte mintues, byte hours);

    void reset_alarm(); // this reset the alaram really import to run after the alarm is triggered


    // call back register function for the event system
    void register_time_callback(TimeCallback callback);
    void register_date_callback(DateCallback callback);
    void register_alarm_callback(AlarmCallback callback);

}

#endif