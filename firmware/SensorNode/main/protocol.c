
/*
    THIS CODE IS GENEARTED DO NOT CHANGE, IF YOU WANT TO CHANGE LOOK THROUGH THE
    PYTHON MIRROR OF THE PROTOCAL AND REGENERATE THE CODE WITH THE GENERATOR
*/

#include "protocol.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdio.h>



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

    if (buffer[offset] > 8)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

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

    memcpy(&buffer[offset], &msg->natrual_gas, 4);
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

    memcpy(&msg->natrual_gas, &buffer[offset], 4);
    offset += 4;

    memcpy(&msg->smoke, &buffer[offset], 4);
    offset += 4;

    *buffer_size -= offset;
    return buffer + offset;
}

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

    if (buffer[offset] > 8)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

uint8_t *set_date_time_encode(const SetDateTime *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 6)
        return NULL;

    size_t offset = 0;

    buffer[offset++] = msg->seconds;

    buffer[offset++] = msg->minutes;

    buffer[offset++] = msg->hours;

    buffer[offset++] = msg->day;

    buffer[offset++] = msg->month;

    memcpy(&buffer[offset], &msg->year, 4);
    offset += 4;

    *buffer_size -= offset;
    return buffer + offset;
}

const uint8_t *set_date_time_decode(SetDateTime *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 6)
        return NULL;

    size_t offset = 0;

    msg->seconds = buffer[offset++];

    msg->minutes = buffer[offset++];

    msg->hours = buffer[offset++];

    msg->day = buffer[offset++];

    msg->month = buffer[offset++];

    memcpy(&msg->year, &buffer[offset], 4);
    offset += 4;

    *buffer_size -= offset;
    return buffer + offset;
}

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

