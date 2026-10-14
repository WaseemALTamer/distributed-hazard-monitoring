#ifndef CONFIG_H
#define CONFIG_H





namespace Config{

    // dynamic vars are keep inside if you want
    // check the Config.cpp file  for how  they
    // are handled 

    extern char name[64];
    //extern char token[32];


    extern char network_ssid[32];
    extern char network_password[64];

    extern char hotspot_ssid[32];
    extern char hotspot_password[64];

    extern int network_port;

    extern int broadcast_cooldown_duration_ms;

    void save();
    bool load();
    
    void init();
    void loop();
}




#endif