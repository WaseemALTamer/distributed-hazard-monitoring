#include <Arduino.h>
#include "Config.h"





namespace Config{


    char name[64];
    //char token[32];


    float transition_speed = 0.3f; // transition speed for sequence effects
    float brightness = 0.15f;
    


    // network config
    char network_ssid[32];
    char network_password[64];

    int network_port = 39394;



    void init(){
        //strncpy(name, "New Name", sizeof(name));
        strncpy(name, "NODE-1", sizeof(name));
        name[sizeof(name) - 1] = '\0';

        // add the newtork data they are not changable for now at least
        strncpy(network_ssid, "", sizeof(network_ssid));
        strncpy(network_password, "", sizeof(network_password));

        // adding the null treminator just in case 
        network_ssid[sizeof(network_ssid) - 1] = '\0';
        network_password[sizeof(network_password) - 1] = '\0';


        // this will handel loading on the data that you
        // have stored later on
    }




    void loop(){

    }

}



