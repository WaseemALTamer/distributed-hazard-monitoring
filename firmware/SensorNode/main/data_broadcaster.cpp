#include <Arduino.h>

#include "device_status_manager.h"
#include "network_wifi.h"
#include "rtc_clock.h"
#include "protocol.h"
#include "config.h"

#include "sensor_manager.h"



namespace DataBroadcaster{


    int buffer_size = 256; // this value may need to be increased later on down the line

    
    bool broadcast_cooldown_passed() {
        // responsible for waiting until the next cool down time

        static unsigned long last_event_time = 0;
        unsigned long now = millis();

        if (now - last_event_time < Config::broadcast_cooldown_duration_ms) {
            return false; // ignore event
        }

        last_event_time = now;
        return true; // accept event
    }


    void init(){


    }


    void loop(){
        if(!broadcast_cooldown_passed()) return;


        Information info = {};
        info.status = static_cast<DeviceStatus>(DeviceStatusManager::current_state);
        info.temperature = SensorManager::temperature;
        info.humidity = SensorManager::humidity;
        info.carbon_dioxide = SensorManager::carbon_dioxide;
        info.natrual_gas = SensorManager::natrual_gas;
        info.carbon_monoxide = SensorManager::carbon_monoxide;
        info.smoke = SensorManager::smoke;


        Message message = {};
        snprintf(
            message.device_name,
            sizeof(message.device_name),
            "%s",
            Config::name
        );

        memcpy(
            message.mac_address,
            NetworkWifi::mac_address,
            sizeof(message.mac_address)
        );

        message.time_stamp = RTCClock::get_unix_time();
        message.is_encrypted = 0;
        message.content_type = CONTENT_TYPE_INFORMATION;
        message.content = nullptr;


        uint8_t info_buffer[128];
        size_t info_capacity = sizeof(info_buffer);

        uint8_t* info_end = information_encode(
            &info,
            info_buffer,
            &info_capacity
        );

        if (info_end == nullptr) {
            Serial.println("Failed to encode information payload");
            return;
        }


        const size_t encoded_info_size =
            static_cast<size_t>(info_end - info_buffer);


        uint8_t buffer[buffer_size];
        size_t remaining = sizeof(buffer);

        uint8_t* next = message_encode(
            &message,
            buffer,
            &remaining
        );

        if (next == nullptr) {
            Serial.println("Failed to encode message header");
            return;
        }

        // Ensure the Information payload fits.
        if (encoded_info_size > remaining) {
            Serial.println("Buffer too small for information payload");
            return;
        }

        // Append Information bytes immediately after Message bytes.
        memcpy(next, info_buffer, encoded_info_size);

        const size_t header_size = static_cast<size_t>(next - buffer);
        const size_t total_length = header_size + encoded_info_size;

        // Broadcast the complete packet.
        NetworkWifi::send(
            IPAddress(255, 255, 255, 255),
            Config::network_port,
            buffer,
            total_length
        );        
    }


}