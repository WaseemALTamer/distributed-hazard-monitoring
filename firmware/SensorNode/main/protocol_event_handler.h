#pragma once
#include <Arduino.h>
#include "constants.h"

namespace ProtocalEventHandler
{


    typedef void (*MessageCallback)(Message content, IPAddress senderIP, uint16_t senderPort);
    extern MessageCallback message_callbacks[MAX_NETWORK_CALLBACKS];
    extern int message_callback_count;

    typedef void (*CommandCallback)(Command content, IPAddress senderIP, uint16_t senderPort);
    extern CommandCallback command_callbacks[MAX_NETWORK_CALLBACKS];
    extern int command_callback_count;

    typedef void (*WifiConnectCallback)(WifiConnect content, IPAddress senderIP, uint16_t senderPort);
    extern WifiConnectCallback wifi_connect_callbacks[MAX_NETWORK_CALLBACKS];
    extern int wifi_connect_callback_count;

    typedef void (*DiscoveryCallback)(Discovery content, IPAddress senderIP, uint16_t senderPort);
    extern DiscoveryCallback discovery_callbacks[MAX_NETWORK_CALLBACKS];
    extern int discovery_callback_count;

    typedef void (*SetDeviceNameCallback)(SetDeviceName content, IPAddress senderIP, uint16_t senderPort);
    extern SetDeviceNameCallback set_device_name_callbacks[MAX_NETWORK_CALLBACKS];
    extern int set_device_name_callback_count;

    typedef void (*SetDeviceStatusCallback)(SetDeviceStatus content, IPAddress senderIP, uint16_t senderPort);
    extern SetDeviceStatusCallback set_device_status_callbacks[MAX_NETWORK_CALLBACKS];
    extern int set_device_status_callback_count;

    typedef void (*InformationCallback)(Information content, IPAddress senderIP, uint16_t senderPort);
    extern InformationCallback information_callbacks[MAX_NETWORK_CALLBACKS];
    extern int information_callback_count;

    bool register_message_callback(MessageCallback callback);
    bool register_command_callback(CommandCallback callback);
    bool register_wifi_connect_callback(WifiConnectCallback callback);
    bool register_discovery_callback(DiscoveryCallback callback);
    bool register_set_device_name_callback(SetDeviceNameCallback callback);
    bool register_set_device_status_callback(SetDeviceStatusCallback callback);
    bool register_information_callback(InformationCallback callback);

    void trigger_message_callbacks(Message content);
    void trigger_command_callbacks(Command content);
    void trigger_wifi_connect_callbacks(WifiConnect content);
    void trigger_discovery_callbacks(Discovery content);
    void trigger_set_device_name_callbacks(SetDeviceName content);
    void trigger_set_device_status_callbacks(SetDeviceStatus content);
    void trigger_information_callbacks(Information content);

    void handle_message(
        const uint8_t* buffer,
        size_t len,
        IPAddress senderIP,
        uint16_t senderPort
    );

}