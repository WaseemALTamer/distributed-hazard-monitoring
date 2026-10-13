// this is a base <data provider> class, this class will not access any
// of the other, this class will provide time data and it will set an


// i estimate this class to be taking 2% of the memeory but the libries
// we are importing wire and  the Rtc_Pcf8563 library we are taking 13%


// this module is not suitable after a centery or so long


#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <time.h>

#include "constants.h"
#include "protocol_event_handler.h"
#include "protocol.h"





namespace RTCClock{

    // the clock has to be connected to the I2C pins for arduino
    // nano thats <A5 -> SCL> and <A4 -> SOA>

    Rtc_Pcf8563 rtc; // this already know which pins are scl and soa through the wire library
                     // the hard bit now is connecting it


    // this is the alarm setting
    //const int int_pin = A3; // this pin will be used as a notifier for the alarm 
    //int int_pin_last_state = HIGH; // the pin start being at high

    // note that when  the alarm is activated it will  trigger only one event
    // and not a constant trigger of events please make sure your upper layer
    // works flawlessly

    // another event will be excuated every second to indecate a change in time
    // that event will provide the [seconds, mintues, hours]

    // another event  will be  excuated  every  day to  indecate a  change date
    // so you can update your date on display if you need to do so


    int current_seconds;
    int current_mintues;
    int current_hours;

    int current_day;
    int current_month;
    int current_year;


    uint32_t get_unix_time() {
        struct tm t = {};

        t.tm_sec  = current_seconds;
        t.tm_min  = current_mintues;
        t.tm_hour = current_hours;
        t.tm_mday = current_day;
        t.tm_mon  = current_month - 1;
        t.tm_year = current_year + 100;

        time_t timestamp = mktime(&t);

        return (uint32_t)timestamp;
    }



    int latest_seconds = 0; // this will be used to track a change in seconds to trigger an event

    int latest_day = 0; // this will be used to track a change in the day to trigger a date event


    int loop_cooldown_duration_ms = 500; // we dont excuate the loop all the time, because the
                                         // time clock  module will  update only every  second
                                         // and the getting second and  day time  is expinsive
                                         // it will distrub the flow of the main loop


    // incase you cant wait for the event that detect a change of date or  time then  excuate the
    // get_seconds, get_mintues ... get_year function to get  the data  that you  need this class
    // will provide a wrapper to those  functions so  do not  re intiate  the Rtc_Pcf8563 library
    // please =) 


    // events

    const int MAX_CALLBACKS = 1; // each array can only hold 5 callbacks if you try to add 
                                 // more than 5 callbacks then what  comes after 5 will be
                                 // ignored

    typedef void (*TimeCallback)(int seconds, int mintues, int hours); // this will include seconds mintues and hours note
                                                                       // that this is aiming to  be triggered by when the
                                                                       // seconds change
           
                                                                       
    TimeCallback time_callbacks[MAX_CALLBACKS];
    int time_callbacks_count = 0;


    typedef void (*DateCallback)(int day, int month, int year); // this will include day month and year note that this aims
                                                                // to be triggered on a  change of  day, note  that the two
                                                                // <typedef>  can  be  one, i just  left it  like that  for
                                                                // readablity, if we are low on memeory thats an option


    DateCallback date_callbacks[MAX_CALLBACKS];
    int date_callbacks_count = 0;




    typedef void (*AlarmCallback)(); // the alarm event doesnt give you any data back it just triggers
                                     // the registred callbacks, if you  want to display the time then
                                     // you can just get them using this get functions


    AlarmCallback alarm_callbacks[MAX_CALLBACKS];
    int alarm_callbacks_count = 0;



    int is_alarm_active = 0; // this will tell you if the alaram is avtive if it is active then
                             // this will be set to  1 and  if it  is not  then this  will be 0
                             // please do not over right this var, but for safty it is isolated
                             // if you over right it you will end up with less information 


    // event trigger functions
    void on_time_event(byte seconds, byte mintues, byte hours) {

        // loop through the rigistered functions and excuate them
        for (int i = 0; i < time_callbacks_count; i++) { 
            if (time_callbacks[i] != nullptr) {
                time_callbacks[i](seconds, mintues, hours);
            }
        }
    }


    void on_date_event(byte day, byte months, byte year) {   

        // loop through the rigistered functions and excuate them
        for (int i = 0; i < date_callbacks_count; i++) {
            if (date_callbacks[i] != nullptr) {
                date_callbacks[i](day, months, year);
            }
        }
    }


    void on_alarm_event() {   

        // loop through the rigistered functions and excuate them
        for (int i = 0; i < alarm_callbacks_count; i++) {
            if (alarm_callbacks[i] != nullptr) {
                alarm_callbacks[i]();
            }
        }
    }



    void register_time_callback(TimeCallback callback) {
        if (time_callbacks_count < MAX_CALLBACKS) {
            time_callbacks[time_callbacks_count++] = callback;
        }
    }


    void register_date_callback(DateCallback callback) {
        if (date_callbacks_count < MAX_CALLBACKS) {
            date_callbacks[date_callbacks_count++] = callback;
        }
    }


    void register_alarm_callback(AlarmCallback callback) {
        if (alarm_callbacks_count < MAX_CALLBACKS) {
            alarm_callbacks[alarm_callbacks_count++] = callback;
        }
    }



    // Those function are wrapped functions from the library please use those
    // and not the library functions please this makes  the library changable
    // later on down the line, as the  module that am  using is  not the same
    // one as in the labs

    // get functions they are wrapps

    // time
    byte get_seconds(){
        return rtc.getSecond();
    }

    byte get_mintues(){
        return rtc.getMinute();
    }

    byte get_hour(){
        return rtc.getHour();
    }

    // date
    byte get_day(){
        return rtc.getDay();
    }

    byte get_month(){
        return rtc.getMonth();
    }

    byte get_year(){
        return rtc.getYear();
    }

    // alarm

    // time
    byte get_alarm_mintues(){
        return rtc.getAlarmMinute();
    }

    byte get_alarm_hour(){
        return rtc.getAlarmHour();
    }



    // set functions -> those will be used to set the current time and 
    // to set an alarm time,  again they  are wrapers functions to the
    // rtc library


    // note that those functions will not trigger any events
    // those functions will do what they say and thats it no
    // more no less, if you want  to update something update
    // your self through the upper layers please dont change
    // those base layers and keep them as they are


    void set_time(byte hours, byte mintues, byte seconds){
        // this is a simple files that sets the time for you

        rtc.setTime(
            hours, 
            mintues, 
            seconds
        );

    }

    void set_date(byte day, byte months, uint32_t fullYear){
        // this code is cabable of tracking week days but we are not
        // going to do that from  the start  but we  can always come
        // back to make it better

        byte year = fullYear % 100;
        byte century = (fullYear >= 2000);

        //Serial.println(fullYear);
        //Serial.println(year);
        //Serial.println(century);

        rtc.setDate(
            day,
            0,
            months,
            century,
            year
        );
    }

    void set_alarm(byte hours, byte mintues){
        // the clock is cabable of making alarams for certain day of the week
        // or certain days of the month but we are not  giong to support that
        // in our project lets stick to only mintues and hours every day  


        rtc.setAlarm(
            mintues,  
            hours, 
            RTCC_NO_ALARM, // day 
            RTCC_NO_ALARM // weekday
        );
    }


    void reset_alarm(){
        // this will simply  rest the alarm so its ready for next
        // day or whever your  alarm is, make sure  that the this
        // is run this after each alarm take event this is a base
        // class am not going to put the code here

        // updated this does nothing now
        
        //rtc.resetAlarm();
    }





    bool loop_cooldown_passed() {
        // this avoid internal bounce effect for the buttons

        static unsigned long last_event_time = 0;
        unsigned long now = millis();

        if (now - last_event_time < loop_cooldown_duration_ms) {
            return false; // ignore event
        }

        last_event_time = now;
        return true; // accept event
    }


    void call_back_test(int seconds, int mintues, int hours){
        
        Serial.printf("%d", get_unix_time());
        Serial.printf("        ");
        Serial.printf("%d", current_day);
        Serial.printf("/");
        Serial.printf("%d", current_month);
        Serial.printf("/");
        Serial.printf("%d", current_year);
        Serial.printf("        ");

        Serial.printf("%d", hours);
        Serial.printf(":");
        Serial.printf("%d", mintues);
        Serial.printf(":");
        Serial.printf("%d", seconds);
        Serial.println(" ");
    }


    void on_network_date_time_sync_request(SetDateTime content, IPAddress senderIP, uint16_t senderPort){
        set_time(content.hours, content.minutes, content.seconds);
        set_date(content.day, content.month, content.year);
    }


    void init() {
        Wire.begin(SDA_PIN, SCL_PIN); // we start the I2C communication
        //pinMode (int_pin, INPUT_PULLUP); // set up the pin so we can read the input from it

        // register the call backs
        ProtocalEventHandler::register_set_date_time_callback(on_network_date_time_sync_request);
        //register_time_callback(call_back_test);
    }


    void loop() {


        if (!loop_cooldown_passed()){
            // we dont want the loop to be excuated every single loop
            // this will be very  ineffecent for  the  arduino to run
            // we aim to run it less than a second but still  in that
            // good enough that we can detect events


            return;
        }
        /*
        int int_current_state = digitalRead(int_pin);
        if (int_current_state != int_pin_last_state){
            // we detected change check if an alaram is triggered
            // this happens when our current state is on LOW

            if (int_current_state == LOW){
                // trigger the alarm event
                on_alarm_event();
                is_alarm_active = 1; // tell the other parts of the code we are active
            }if (int_current_state == HIGH){
                is_alarm_active = 0; // tell the other parts of the code we are inactive
            }


          int_pin_last_state  = int_current_state;
        }
        */





        // this detects the change in seconds and trigger an event
        current_seconds = get_seconds(); // note that am using the wrapped functions as well
        current_mintues = get_mintues();
        current_hours = get_hour();


        if (latest_seconds != current_seconds){
            latest_seconds = current_seconds;

            // we trigger time event here
            
            on_time_event(
                current_seconds, // we already have the seconds from before
                current_mintues, // we get the mintues
                current_hours // we get the year
            );


        }

        // we manually check for the alarm, this  is done this way
        // so i can save the int pin and use it for something else
        byte alarm_mintues = get_alarm_mintues();
        byte alarm_hours = get_alarm_hour();

        if (current_mintues == alarm_mintues &&
            current_hours == alarm_hours &&
            is_alarm_active == 0)
            {
                is_alarm_active = 1; // turn the alarm on
                on_alarm_event();
            }
            
            if(current_mintues != alarm_mintues ||
                current_hours != alarm_hours)
                {

                if (is_alarm_active == 1){ // combine both of the if statments togather later on
                    is_alarm_active = 0; // turn the alram off
                }
            }


        current_day = get_day(); // note that am using the wrapped functions as well
        if (latest_day != current_day){
            latest_day = current_day;

            // we trigger date event here

            current_month = get_month();
            current_year = get_year();

            on_date_event(
                current_day, // we already have the day from befor 
                current_month, // we get the year 
                current_year // we get the hour
            );



        }


    }





}