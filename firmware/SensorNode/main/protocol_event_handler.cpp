#include <Arduino.h>

#include "protocol.h"
#include "constants.h"

#include "protocol_event_handler.h"

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


    // current flow handel_message -> (dispatch_content -> event) -> (dispatch_content -> event) -> ... -> (dispatch_content -> event)
    // the flow will keep decoding the message until the message ends with content that doesnt hold greedy bytes

    


    // reserve space for all the message types
    Message message{};
    Command command{};
    WifiConnect wifi_connect{};
    Discovery discovery{};
    SetDeviceName set_device_name{};
    SetDeviceStatus set_device_status{};
    Information information{};
    SetDateTime set_date_time{};


    MessageCallback message_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int message_callback_count = 0;

    CommandCallback command_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int command_callback_count = 0;

    WifiConnectCallback wifi_connect_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int wifi_connect_callback_count = 0;

    DiscoveryCallback discovery_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int discovery_callback_count = 0;

    SetDeviceNameCallback set_device_name_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int set_device_name_callback_count = 0;

    SetDeviceStatusCallback set_device_status_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int set_device_status_callback_count = 0;

    InformationCallback information_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int information_callback_count = 0;

    SetDateTimeCallback set_date_time_callbacks[MAX_NETWORK_CALLBACKS] = {};
    int set_date_time_callback_count = 0;



    // registration functions
    bool register_message_callback(MessageCallback callback) {
        if (callback == nullptr ||
            message_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        message_callbacks[message_callback_count++] = callback;
        return true;
    }

    bool register_command_callback(CommandCallback callback) {
        if (callback == nullptr ||
            command_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        command_callbacks[command_callback_count++] = callback;
        return true;
    }

    bool register_wifi_connect_callback(WifiConnectCallback callback) {
        if (callback == nullptr ||
            wifi_connect_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        wifi_connect_callbacks[wifi_connect_callback_count++] = callback;
        return true;
    }

    bool register_discovery_callback(DiscoveryCallback callback) {
        if (callback == nullptr ||
            discovery_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        discovery_callbacks[discovery_callback_count++] = callback;
        return true;
    }

    bool register_set_device_name_callback(SetDeviceNameCallback callback) {
        if (callback == nullptr ||
            set_device_name_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        set_device_name_callbacks[set_device_name_callback_count++] = callback;
        return true;
    }

    bool register_set_device_status_callback(SetDeviceStatusCallback callback) {
        if (callback == nullptr ||
            set_device_status_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        set_device_status_callbacks[set_device_status_callback_count++] = callback;
        return true;
    }

    bool register_information_callback(InformationCallback callback) {
        if (callback == nullptr ||
            information_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        information_callbacks[information_callback_count++] = callback;
        return true;
    }


    bool register_set_date_time_callback(SetDateTimeCallback callback) {
        if (callback == nullptr ||
            set_date_time_callback_count >= MAX_NETWORK_CALLBACKS) return false;

        set_date_time_callbacks[set_date_time_callback_count++] = callback;
        return true;
    }



    // trigger functions
    void trigger_message_callbacks(Message content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < message_callback_count; i++)
            message_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_command_callbacks(Command content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < command_callback_count; i++)
            command_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_wifi_connect_callbacks(WifiConnect content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < wifi_connect_callback_count; i++)
            wifi_connect_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_discovery_callbacks(Discovery content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < discovery_callback_count; i++)
            discovery_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_set_device_name_callbacks(SetDeviceName content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < set_device_name_callback_count; i++)
            set_device_name_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_set_device_status_callbacks(SetDeviceStatus content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < set_device_status_callback_count; i++)
            set_device_status_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_information_callbacks(Information content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < information_callback_count; i++)
            information_callbacks[i](content, senderIP, senderPort);
    }

    void trigger_set_date_time_callbacks(SetDateTime content, IPAddress senderIP, uint16_t senderPort) {
        for (int i = 0; i < set_date_time_callback_count; i++)
            set_date_time_callbacks[i](content, senderIP, senderPort);
    }





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

                trigger_message_callbacks(message, senderIP, senderPort);

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

                trigger_command_callbacks(command, senderIP, senderPort);

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
                    trigger_wifi_connect_callbacks(wifi_connect, senderIP, senderPort);
                }
                break;

            case CONTENT_TYPE_DISCOVERY:
                if (discovery_decode(&discovery, buffer, &remaining) != nullptr) {
                    trigger_discovery_callbacks(discovery, senderIP, senderPort);
                }
                break;

            case CONTENT_TYPE_SET_DEVICE_NAME:
                if (set_device_name_decode(&set_device_name, buffer, &remaining) != nullptr) {
                    trigger_set_device_name_callbacks(set_device_name, senderIP, senderPort);
                }
                break;

            case CONTENT_TYPE_SET_DEVICE_STATUS:
                if (set_device_status_decode(&set_device_status, buffer, &remaining) != nullptr) {
                    trigger_set_device_status_callbacks(set_device_status, senderIP, senderPort);
                }
                break;

            case CONTENT_TYPE_INFORMATION:
                if (information_decode(&information, buffer, &remaining) != nullptr) {
                    trigger_information_callbacks(information, senderIP, senderPort);
                }
                break;

            case CONTENT_TYPE_SET_DATE_TIME:
                if (set_date_time_decode(&set_date_time, buffer, &remaining) != nullptr) {
                    trigger_set_date_time_callbacks(set_date_time, senderIP, senderPort);
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