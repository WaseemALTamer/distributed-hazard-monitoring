#include <Arduino.h>

#include "protocol.h"




namespace ProtocalEventHandler
{
    // this namespace will proccess each message at a time 
    // this namespace will not  work if  you are running a
    // RTOS 


    // this file will work in the following way:
    
    
    // meessage will be  taken and  dispatch_content is excauted
    // where content is message, then dispatch_content will call
    // it self  recursivelly as long  as the messages them selfs
    // contain a content_type, meaning that we recursively after
    // each message decode we trigger a event for the message


    // reserve space for all the message types
    Message message{};
    Command command{};
    WifiConnect wifi_connect{};
    Discovery discovery{};
    SetDeviceName set_device_name{};
    SetDeviceStatus set_device_status{};
    Information information{};


void dispatch_content(
    ContentType type,
    const uint8_t* buffer,
    size_t len,
    IPAddress senderIP,
    uint16_t senderPort,
    uint8_t depth = 0
) {
    if (buffer == nullptr || depth > 8) {
        return;
    }

    size_t remaining = len;
    const uint8_t* payload = nullptr;

    switch (type) {
        case CONTENT_TYPE_MESSAGE: {
            payload = message_decode(&message, buffer, &remaining);

            if (payload == nullptr) {
                return;
            }

            Serial.println("CONTENT_TYPE_MESSAGE");

            dispatch_content(
                message.content_type,
                payload,
                remaining,
                senderIP,
                senderPort,
                depth + 1
            );

            break;
        }

        case CONTENT_TYPE_COMMAND: {
            payload = command_decode(&command, buffer, &remaining);

            if (payload == nullptr) {
                return;
            }

            Serial.println("CONTENT_TYPE_COMMAND");

            dispatch_content(
                command.content_type,
                payload,
                remaining,
                senderIP,
                senderPort,
                depth + 1
            );

            break;
        }

        case CONTENT_TYPE_WIFI_CONNECT:
            if (wifi_connect_decode(&wifi_connect, buffer, &remaining) != nullptr) {
                Serial.println("CONTENT_TYPE_WIFI_CONNECT");
            }
            break;

        case CONTENT_TYPE_DISCOVERY:
            if (discovery_decode(&discovery, buffer, &remaining) != nullptr) {
                Serial.println("CONTENT_TYPE_DISCOVERY");
            }
            break;

        case CONTENT_TYPE_SET_DEVICE_NAME:
            if (set_device_name_decode(&set_device_name, buffer, &remaining) != nullptr) {
                Serial.println("CONTENT_TYPE_SET_DEVICE_NAME");
            }
            break;

        case CONTENT_TYPE_SET_DEVICE_STATUS:
            if (set_device_status_decode(&set_device_status, buffer, &remaining) != nullptr) {
                Serial.println("CONTENT_TYPE_SET_DEVICE_STATUS");
            }
            break;

        case CONTENT_TYPE_INFORMATION:
            if (information_decode(&information, buffer, &remaining) != nullptr) {
                Serial.println("CONTENT_TYPE_INFORMATION");
            }
            break;

        default:
            Serial.println("Unknown content type");
            break;
    }
}

    void handle_message(
        const uint8_t* buffer,
        size_t len,
        IPAddress senderIP,
        uint16_t senderPort
    ) {
        dispatch_content(
            CONTENT_TYPE_MESSAGE,
            buffer,
            len,
            senderIP,
            senderPort
        );
    }
}