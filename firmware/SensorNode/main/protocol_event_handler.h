#pragma once
#include <Arduino.h>


namespace ProtocalEventHandler
{


    void handle_message(
        const uint8_t* buffer,
        size_t len,
        IPAddress senderIP,
        uint16_t senderPort
    );

}