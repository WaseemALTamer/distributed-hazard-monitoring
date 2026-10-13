#include <Preferences.h>
#include <Arduino.h>


#include "Config.h"
#include "protocol_event_handler.h"





namespace Config{

    Preferences preferences;

    char name[64];


    // network config
    char network_ssid[32];
    char network_password[64];


    char hotspot_ssid[32];
    char hotspot_password[64];


    int network_port = 39394;


    void save() {

        if (!preferences.begin("config", false)) {
            Serial.println("Failed to open configuration storage.");
            return;
        }

        preferences.putString("name", name);

        preferences.putString("net_ssid", network_ssid);
        preferences.putString("net_pass", network_password);

        preferences.putString("ap_ssid", hotspot_ssid);
        preferences.putString("ap_pass", hotspot_password);

        preferences.putInt("net_port", network_port);

        preferences.end();

        Serial.println("Configuration saved.");
    }


    bool load() {

        if (!preferences.begin("config", true)) {
            return false;
        }

        // Check whether a saved configuration exists.
        if (!preferences.isKey("name")) {
            preferences.end();
            return false;
        }


        preferences.getString("name", name, sizeof(name));

        preferences.getString(
            "network_ssid", network_ssid, sizeof(network_ssid));

        preferences.getString(
            "network_password", network_password, sizeof(network_password));

        preferences.getString(
            "hotspot_ssid", hotspot_ssid, sizeof(hotspot_ssid));

        preferences.getString(
            "hotspot_password", hotspot_password, sizeof(hotspot_password));

        network_port = preferences.getInt("network_port", 39394); // hard coded defult value change that please

        preferences.end();

        return true;
    }



    void on_network_wifi_connect(WifiConnect content, IPAddress senderIP, uint16_t senderPort){

        Serial.println("Hello world");

        strncpy(network_ssid, content.ssid, sizeof(network_ssid) - 1);
        strncpy(network_password, content.password, sizeof(network_password) - 1);

        network_ssid[sizeof(network_ssid) - 1] = '\0';
        network_password[sizeof(network_password) - 1] = '\0';

        save();
    }


    void init(){

        strncpy(name, "NODE-1", sizeof(name));
        name[sizeof(name) - 1] = '\0';

        // add the newtork data they are not changable for now at least
        strncpy(network_ssid, "Waseem", sizeof(network_ssid));
        strncpy(network_password, "Waseem16102005", sizeof(network_password));


        strncpy(hotspot_ssid, "ESP32-WASEEM", sizeof(hotspot_ssid));
        strncpy(hotspot_password, "", sizeof(hotspot_password));


        network_ssid[sizeof(network_ssid) - 1] = '\0';
        network_password[sizeof(network_password) - 1] = '\0';

        hotspot_ssid[sizeof(hotspot_ssid) - 1] = '\0';
        hotspot_password[sizeof(hotspot_password) - 1] = '\0';

        load();

        ProtocalEventHandler::register_wifi_connect_callback(on_network_wifi_connect);

    }




    void loop(){

    }

}



