
/*
    THIS CODE IS GENEARTED DO NOT CHANGE, IF YOU WANT TO CHANGE LOOK THROUGH THE
    PYTHON MIRROR OF THE PROTOCAL AND REGENERATE THE CODE WITH THE GENERATOR
*/


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdio.h>



typedef enum ContentType {
    CONTENT_TYPE_NONE = 0,
    CONTENT_TYPE_MESSAGE = 1,
    CONTENT_TYPE_COMMAND = 2,
    CONTENT_TYPE_INFORMATION = 3,
    CONTENT_TYPE_DISCOVERY = 4,
    CONTENT_TYPE_WIFI_CONNECT = 5,
    CONTENT_TYPE_SET_DEVICE_STATUS = 6,
    CONTENT_TYPE_SET_DEVICE_NAME = 7,
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

uint8_t *command_encode(const Command *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 9)
        return NULL;

    size_t offset = 0;

    memcpy(&buffer[offset], &msg->sequence_number, 8);
    offset += 8;

    buffer[offset++] = (uint8_t)msg->content_type;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *command_decode(Command *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 9)
        return NULL;

    size_t offset = 0;

    memcpy(&msg->sequence_number, &buffer[offset], 8);
    offset += 8;

    if (buffer[offset] > 7)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
} Discovery;

uint8_t *discovery_encode(const Discovery *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 0)
        return NULL;

    size_t offset = 0;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *discovery_decode(Discovery *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 0)
        return NULL;

    size_t offset = 0;

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    DeviceStatus status;
    float temperature;
    float humidity;
    float carbon_dioxide;
    float methane;
    float smoke;
} Information;

uint8_t *information_encode(const Information *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 21)
        return NULL;

    size_t offset = 0;

    buffer[offset++] = (uint8_t)msg->status;

    memcpy(&buffer[offset], &msg->temperature, 4);
    offset += 4;

    memcpy(&buffer[offset], &msg->humidity, 4);
    offset += 4;

    memcpy(&buffer[offset], &msg->carbon_dioxide, 4);
    offset += 4;

    memcpy(&buffer[offset], &msg->methane, 4);
    offset += 4;

    memcpy(&buffer[offset], &msg->smoke, 4);
    offset += 4;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *information_decode(Information *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 21)
        return NULL;

    size_t offset = 0;

    if (buffer[offset] > 17)
        return NULL;
    msg->status = (DeviceStatus)buffer[offset++];

    memcpy(&msg->temperature, &buffer[offset], 4);
    offset += 4;

    memcpy(&msg->humidity, &buffer[offset], 4);
    offset += 4;

    memcpy(&msg->carbon_dioxide, &buffer[offset], 4);
    offset += 4;

    memcpy(&msg->methane, &buffer[offset], 4);
    offset += 4;

    memcpy(&msg->smoke, &buffer[offset], 4);
    offset += 4;

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    char device_name[64];
    uint8_t mac_address[6];
    double time_stamp;
    uint8_t is_encrypted;
    ContentType content_type;
    void *content;
} Message;

uint8_t *message_encode(const Message *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 80)
        return NULL;

    size_t offset = 0;

    memset(&buffer[offset], 0, 64);
    strncpy((char *)&buffer[offset], msg->device_name, 63);
    offset += 64;

    memcpy(&buffer[offset], msg->mac_address, 6);
    offset += 6;

    memcpy(&buffer[offset], &msg->time_stamp, 8);
    offset += 8;

    buffer[offset++] = msg->is_encrypted;

    buffer[offset++] = (uint8_t)msg->content_type;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *message_decode(Message *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 80)
        return NULL;

    size_t offset = 0;

    memcpy(msg->device_name, &buffer[offset], 64);
    msg->device_name[63] = '\0';
    offset += 64;

    memcpy(msg->mac_address, &buffer[offset], 6);
    offset += 6;

    memcpy(&msg->time_stamp, &buffer[offset], 8);
    offset += 8;

    msg->is_encrypted = buffer[offset++];

    if (buffer[offset] > 7)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    char device_name[64];
} SetDeviceName;

uint8_t *set_device_name_encode(const SetDeviceName *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 64)
        return NULL;

    size_t offset = 0;

    memset(&buffer[offset], 0, 64);
    strncpy((char *)&buffer[offset], msg->device_name, 63);
    offset += 64;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *set_device_name_decode(SetDeviceName *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 64)
        return NULL;

    size_t offset = 0;

    memcpy(msg->device_name, &buffer[offset], 64);
    msg->device_name[63] = '\0';
    offset += 64;

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    DeviceStatus status;
} SetDeviceStatus;

uint8_t *set_device_status_encode(const SetDeviceStatus *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 1)
        return NULL;

    size_t offset = 0;

    buffer[offset++] = (uint8_t)msg->status;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *set_device_status_decode(SetDeviceStatus *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 1)
        return NULL;

    size_t offset = 0;

    if (buffer[offset] > 17)
        return NULL;
    msg->status = (DeviceStatus)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    char ssid[32];
    char password[64];
} WifiConnect;

uint8_t *wifi_connect_encode(const WifiConnect *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 96)
        return NULL;

    size_t offset = 0;

    memset(&buffer[offset], 0, 32);
    strncpy((char *)&buffer[offset], msg->ssid, 31);
    offset += 32;

    memset(&buffer[offset], 0, 64);
    strncpy((char *)&buffer[offset], msg->password, 63);
    offset += 64;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *wifi_connect_decode(WifiConnect *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 96)
        return NULL;

    size_t offset = 0;

    memcpy(msg->ssid, &buffer[offset], 32);
    msg->ssid[31] = '\0';
    offset += 32;

    memcpy(msg->password, &buffer[offset], 64);
    msg->password[63] = '\0';
    offset += 64;

    *buffer_size -= offset;
    return buffer + offset;
}

