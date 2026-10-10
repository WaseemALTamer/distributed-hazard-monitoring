#ifndef CONFIG_H
#define CONFIG_H


// constants  are keept  outside of  the LEDs they
// are also global so dont bother using the Config
// namespace when trying to access them

#define LED_DATA_PIN 5
#define LED_MAX_CALLBACKS 1
#define SEQUENCE_MAX_STEPS 50


#define SDA_PIN 21
#define SCL_PIN 22



namespace Config{

    // dynamic vars are keep inside if you want
    // check the Config.cpp file  for how  they
    // are handled 

    extern char name[64];
    //extern char token[32];

    extern float transition_speed;

    extern float brightness;


    extern char network_ssid[32];
    extern char network_password[64];

    extern int network_port;


    void init();
    void loop();
}




#endif