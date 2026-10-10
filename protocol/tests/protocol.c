
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
    CONTENT_TYPE_ONLINE = 5,
    CONTENT_TYPE_WIFI_CONNECT = 6,
} ContentType;


typedef struct{
    uint64_t sequence_number;
    ContentType content_type;
    void *content;
} Command;

uint8_t *Command_encode(const Command *msg, uint8_t *buffer, size_t *buffer_size)
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

const uint8_t *Command_decode(Command *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 9)
        return NULL;

    size_t offset = 0;

    memcpy(&msg->sequence_number, &buffer[offset], 8);
    offset += 8;

    if (buffer[offset] > 6)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    uint8_t version;
    char device_name[64];
    uint8_t mac_address[6];
    double time_stamp;
    uint8_t is_encrypted;
    ContentType content_type;
    void *content;
} Message;


uint8_t *Message_encode(const Message *msg, uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 81)
        return NULL;

    size_t offset = 0;

    buffer[offset++] = msg->version;

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

const uint8_t *Message_decode(Message *msg, const uint8_t *buffer, size_t *buffer_size)
{
    if (*buffer_size < 81)
        return NULL;

    size_t offset = 0;

    msg->version = buffer[offset++];

    memcpy(msg->device_name, &buffer[offset], 64);
    msg->device_name[63] = '\0';
    offset += 64;

    memcpy(msg->mac_address, &buffer[offset], 6);
    offset += 6;

    memcpy(&msg->time_stamp, &buffer[offset], 8);
    offset += 8;

    msg->is_encrypted = buffer[offset++];

    if (buffer[offset] > 6)
        return NULL;
    msg->content_type = (ContentType)buffer[offset++];

    *buffer_size -= offset;
    return buffer + offset;
}

typedef struct{
    char ssid[32];
    char password[64];
} WifiConnect;

uint8_t *Wifi_Connect_encode(const WifiConnect *msg, uint8_t *buffer, size_t *buffer_size)
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

const uint8_t *Wifi_Connect_decode(WifiConnect *msg, const uint8_t *buffer, size_t *buffer_size)
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



int main(void){




    Message m;

    m.version = 1;
    snprintf(m.device_name, sizeof(m.device_name), "%s", "Waseem AL Tamer");
    memcpy(m.mac_address, (uint8_t[]){0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01}, 6);
    m.time_stamp = 3000.0;
    m.content_type = CONTENT_TYPE_COMMAND;

    Command c;
    c.sequence_number = 0;
    c.content_type = CONTENT_TYPE_WIFI_CONNECT;


    WifiConnect w;
    snprintf(w.ssid, sizeof(w.ssid), "%s", "waseem");
    snprintf(w.password, sizeof(w.password), "%s", "waseem2005");


    uint8_t buffer[sizeof(m) + sizeof(c) + sizeof(w)];
    size_t space = sizeof(buffer);
    uint8_t *next = Message_encode(&m, buffer, &space);
    next = Command_encode(&c, next, &space);
    next = Wifi_Connect_encode(&w, next, &space);

    


    printf("data = bytes([");
    for (size_t i = 0; i < sizeof(buffer) - space; i++) {
        printf("0x%02X", buffer[i]);

        if (i + 1 < sizeof(buffer) - space)
            printf(", ");
    }
    printf("])\n");
    


    return 0;
}