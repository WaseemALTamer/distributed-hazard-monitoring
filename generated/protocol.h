/*
    THIS CODE IS GENERATED. DO NOT CHANGE.
    IF YOU WANT TO CHANGE IT, MODIFY THE PYTHON PROTOCOL
    AND REGENERATE THIS CODE.
*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

typedef enum ContentType {
    CONTENT_TYPE_NONE = 0,
    CONTENT_TYPE_MESSAGE = 1,
    CONTENT_TYPE_COMMAND = 2,
    CONTENT_TYPE_INFORMATION = 3,
    CONTENT_TYPE_DISCOVERY = 4,
    CONTENT_TYPE_WIFI_CONNECT = 5,
    CONTENT_TYPE_SET_DEVICE_STATUS = 6,
    CONTENT_TYPE_SET_DEVICE_NAME = 7,
    CONTENT_TYPE_SET_DATE_TIME = 8,
} ContentType;


typedef enum DeviceStatus {
    DEVICE_STATUS_NONE = 0,
    DEVICE_STATUS_BOOTING = 1,
    DEVICE_STATUS_NORMAL = 2,
    DEVICE_STATUS_NETWORK_ERROR = 3,
    DEVICE_STATUS_UNKNOWN_ERROR = 4,
    DEVICE_STATUS_SENSORS_ERROR = 5,
    DEVICE_STATUS_TH_SENSOR_ERROR = 6,
    DEVICE_STATUS_TEMPERATURE_ERROR = 7,
    DEVICE_STATUS_HUMIDITY_ERROR = 8,
    DEVICE_STATUS_CO2_ERROR = 9,
    DEVICE_STATUS_METHANE_SENSOR_ERROR = 10,
    DEVICE_STATUS_METHANE_DETECTED = 11,
    DEVICE_STATUS_METHANE_LEVEL_HIGH = 12,
    DEVICE_STATUS_SMOKE_SENSOR_ERROR = 13,
    DEVICE_STATUS_SMOKE_DETECTED = 14,
    DEVICE_STATUS_SMOKE_LEVEL_HIGH = 15,
    DEVICE_STATUS_WARNING = 16,
    DEVICE_STATUS_DANGER = 17,
} DeviceStatus;


typedef struct{
    uint64_t sequence_number;
    ContentType content_type;
    void *content;
} Command;

typedef struct{
} Discovery;

typedef struct{
    DeviceStatus status;
    float temperature;
    float humidity;
    float carbon_dioxide;
    float natrual_gas;
    float smoke;
} Information;

typedef struct{
    char device_name[64];
    uint8_t mac_address[6];
    double time_stamp;
    uint8_t is_encrypted;
    ContentType content_type;
    void *content;
} Message;

typedef struct{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} SetDateTime;

typedef struct{
    char device_name[64];
} SetDeviceName;

typedef struct{
    DeviceStatus status;
} SetDeviceStatus;

typedef struct{
    char ssid[32];
    char password[64];
} WifiConnect;


#ifdef __cplusplus
extern "C" {
#endif

uint8_t *command_encode(
    const Command *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *command_decode(
    Command *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *discovery_encode(
    const Discovery *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *discovery_decode(
    Discovery *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *information_encode(
    const Information *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *information_decode(
    Information *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *message_encode(
    const Message *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *message_decode(
    Message *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *set_date_time_encode(
    const SetDateTime *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *set_date_time_decode(
    SetDateTime *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *set_device_name_encode(
    const SetDeviceName *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *set_device_name_decode(
    SetDeviceName *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *set_device_status_encode(
    const SetDeviceStatus *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *set_device_status_decode(
    SetDeviceStatus *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);

uint8_t *wifi_connect_encode(
    const WifiConnect *msg,
    uint8_t *buffer,
    size_t *buffer_size
);

const uint8_t *wifi_connect_decode(
    WifiConnect *msg,
    const uint8_t *buffer,
    size_t *buffer_size
);


#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */
