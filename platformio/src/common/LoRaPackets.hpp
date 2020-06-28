#ifndef _LORA_PACKETS_HPP_
#define _LORA_PACKETS_HPP_

#define INVALID_PACKET ((uint8_t)(-1))
#define PACKET_METADATA_SIZE 5

#if ((!defined PACKET_ENCODER) && (!defined PACKET_DECODER)) || ((defined PACKET_ENCODER) && (defined PACKET_DECODER))
#error Either PACKET_ENCODER or PACKET_DECODER must be defined in build_flags
#endif

#include <Arduino.h>

#ifdef PACKET_ENCODER
#include <TinyGPS++.h>
#endif

#ifdef PACKET_DECODER
#include <ArduinoJson.h>
#endif

struct IMUPacket
{
    const static uint8_t ID = 1;
    uint8_t id;

#ifdef PACKET_DECODER
    const static size_t JSON_DOC_SIZE = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    StaticJsonDocument<JSON_DOC_SIZE> toJson();
#endif

    struct
    {
        float w;
        float x;
        float y;
        float z;
    } data;

    IMUPacket();
    IMUPacket(float w, float x, float y, float z);
};

struct GPSPacket
{
    const static uint8_t ID = 2;
    uint8_t id;

#ifdef PACKET_DECODER
    const static size_t JSON_DOC_SIZE = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    StaticJsonDocument<JSON_DOC_SIZE> toJson();
#endif

    // #ifdef
    struct
    {
        bool valid;
        double lat;
        double lng;
        double alt;

        uint16_t sats;
    } data;

    GPSPacket();
#ifdef PACKET_ENCODER
    GPSPacket(TinyGPSPlus gps);
#endif
};

#ifdef PACKET_DECODER
struct LossPacket
{
    float pct;

    const static size_t JSON_DOC_SIZE = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(1);
    StaticJsonDocument<JSON_DOC_SIZE> toJson();

    LossPacket(float pct);
};
#endif


class _LoRaControl
{
public:
    bool begin();

#ifdef PACKET_ENCODER
    template <class PacketType>
    void sendPacket(PacketType packet)
    {
        return _sendPacket(packet.id, (const uint8_t *)(&packet.data), sizeof(packet.data));
    }
#endif

#ifdef PACKET_DECODER
    int receivePacket();
    template <class PacketType>
    PacketType getPacket()
    {
        PacketType packet;
        memcpy((uint8_t *)(&packet.data), buffer + PACKET_METADATA_SIZE, sizeof(packet.data));
        return packet;
    }
#endif

    uint8_t buffer[0xFF];
    uint8_t bufferLength;
    uint8_t packetNum;

    int lastRSSI;

private:
    void _sendPacket(uint8_t packetID, const uint8_t *packet, uint8_t packetSize);
    uint16_t calculateCRC();
};

extern _LoRaControl LoRaControl;

#endif
