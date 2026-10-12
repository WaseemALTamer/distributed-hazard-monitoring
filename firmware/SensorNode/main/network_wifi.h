#pragma once

#include <WiFiUdp.h>
#include <WiFi.h>


namespace NetworkWifi
{
    // expose the init function so anybody can
    // excuate it and add the schedule it 
    
    void send(IPAddress targetIP, uint16_t targetPort, const uint8_t* payload, size_t length);

    void init();
    void loop();

}