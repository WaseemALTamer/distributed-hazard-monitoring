#include <Preferences.h>
#include <Arduino.h>


#include "Config.h"
#include "protocol_event_handler.h"


// unless you establish a strong connection saving and data will feel jankey


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

        // clear existing values before writing the new ones
        preferences.remove("name");
        preferences.remove("net_ssid");
        preferences.remove("net_password");
        preferences.remove("ap_ssid");
        preferences.remove("ap_password");
        preferences.remove("ap_port");

        // write the current configuration.
        preferences.putString("name", name);
        preferences.putString("net_ssid", network_ssid);
        preferences.putString("net_password", network_password);
        preferences.putString("ap_ssid", hotspot_ssid);
        preferences.putString("ap_password", hotspot_password);
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
            "net_ssid", network_ssid, sizeof(network_ssid));

        preferences.getString(
            "net_password", network_password, sizeof(network_password));

        preferences.getString(
            "ap_ssid", hotspot_ssid, sizeof(hotspot_ssid));

        preferences.getString(
            "ap_password", hotspot_password, sizeof(hotspot_password));

        network_port = preferences.getInt("net_port", 39394); // hard coded defult value change that please

        preferences.end();

        return true;
    }


    void on_network_set_device_name(SetDeviceName content, IPAddress senderIP, uint16_t senderPort){
        strncpy(name, content.device_name, sizeof(name));
        strncpy(hotspot_ssid, content.device_name, sizeof(hotspot_ssid));

        name[sizeof(name) - 1] = '\0';
        name[sizeof(name) - 1] = '\0';

        save();
    }


    void on_network_wifi_connect(WifiConnect content, IPAddress senderIP, uint16_t senderPort){
        strncpy(network_ssid, content.ssid, sizeof(network_ssid) - 1);
        strncpy(network_password, content.password, sizeof(network_password) - 1);

        network_ssid[sizeof(network_ssid) - 1] = '\0';
        network_password[sizeof(network_password) - 1] = '\0';


        //Serial.printf("WiFi SSID: '%s'\n", network_ssid);
        //Serial.printf("WiFi Password: '%s'\n", network_password);

        save();
    }


    void init() {
        // Default values used only if nothing has been saved.
        strncpy(name, "NODE-1", sizeof(name));
        name[sizeof(name) - 1] = '\0';

        network_ssid[0] = '\0';
        network_password[0] = '\0';

        hotspot_ssid[0] = '\0';
        hotspot_password[0] = '\0';

        network_port = 39394;

        if (load()) {
            Serial.println("Configuration loaded.");
        } else {
            Serial.println("No saved configuration found; using defaults.");
        }

        //Serial.printf("Device name: %s\n", name);
        //Serial.printf("WiFi SSID: '%s'\n", network_ssid);
        //Serial.printf("WiFi Password: '%s'\n", network_password);
        

        ProtocalEventHandler::register_wifi_connect_callback(
            on_network_wifi_connect
        );

        ProtocalEventHandler::register_set_device_name_callback(
            on_network_set_device_name
        );
    }




    void loop(){

    }

}



