#include <WiFiUdp.h>
#include <WiFi.h>

#include <Arduino.h>
#include "config.h"


#include "protocol_event_handler.h"



namespace NetworkWifi{

    
    WiFiUDP udp;
    bool udp_started = false;

    bool is_connected = false;
    bool hotspot_active = false;
    int loop_cooldown_duration_ms = 3000;


    unsigned long last_reconnect_attempt = 0;

    uint8_t mac_address[6];



    bool loop_cooldown_passed() {
        // this will let the wifi connection to try after 3 seconds
        // of not being able to connect

        static unsigned long last_event_time = 0;
        unsigned long now = millis();

        if (now - last_event_time < loop_cooldown_duration_ms) {
            return false; // ignore event
        }

        last_event_time = now;
        return true; // accept event
    }

    void start_hotspot() {
        if (hotspot_active) return;


        WiFi.mode(WIFI_AP);
        WiFi.setSleep(false);

        bool success = WiFi.softAP(
            Config::hotspot_ssid,
            Config::hotspot_password,
            6,
            false,
            4
        );

        if (success) {
            hotspot_active = true;
        } else {
            Serial.println("Failed to start hotspot");
        }
    }

    void stop_hotspot() {
        if (!hotspot_active) return;

        Serial.println("Stopping fallback hotspot...");

        WiFi.softAPdisconnect(true);
        hotspot_active = false;
    }



    void send(IPAddress targetIP, uint16_t targetPort, const uint8_t* payload, size_t length) {
        if (!is_connected) return;

        udp.beginPacket(targetIP, targetPort);
        udp.write(payload, length);
        udp.endPacket();
    }



    void init(){
        
        
        
        WiFi.mode(WIFI_STA);
        WiFi.STA.begin(); // for some reason i need to began the STA wifi to be able to get the mac address

        WiFi.macAddress(mac_address);


        WiFi.setAutoReconnect(true);        
        WiFi.begin(Config::network_ssid, Config::network_password);
    }

 
    void loop(){

        wl_status_t status = WiFi.status();

        if (status == WL_CONNECTED) {

            if (WiFi.SSID() != Config::network_ssid) { // check if the wifi we are connected to is the correct wifi
                is_connected = false;
                WiFi.disconnect();
                return;
            }


            if (!is_connected){
                Serial.println(WiFi.localIP());
                is_connected = true;
                stop_hotspot();
            }

            //if (is_connected) {
                //stop_hotspot();
            //}

        }else{
            is_connected = false;
            start_hotspot();
            if (loop_cooldown_passed()){
                WiFi.begin(Config::network_ssid, Config::network_password);
            }
        }

        if (!udp_started) {
            if (udp.begin(Config::network_port)) {
                udp_started = true;
            }
        }


        // now we can handel reading messages
        int packetSize = udp.parsePacket();
        if (packetSize > 0) {
            IPAddress senderIP = udp.remoteIP();
            int senderPort = udp.remotePort();

            uint8_t buffer[2048];
            int len = udp.read(buffer, 2048); // limit it to 2048

            ProtocalEventHandler::handle_message(buffer, len, senderIP, senderPort);
            //MessageManager::decode_message(buffer, len, senderIP, senderPort);
        }

    }
}