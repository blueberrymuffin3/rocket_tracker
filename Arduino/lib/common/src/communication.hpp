#ifndef _COMMUNICATION_HPP_
#define _COMMUNICATION_HPP_

// #define DEBUG_LOGGING // TODO: Move to platformio.ini
#include <Arduino.h>
#include <debug_logging.hpp>

#define COMM_TIMEOUT 500

void crc16_update(uint16_t &crc, byte a);

enum PacketType : uint8_t
{
    Ping,
    Pong,
    RocketData
};

struct Packet
{
    struct RocketDataPacket
    {
        struct MPUData
        {
            float tempC;
            float angleX;
            float angleY;
            float angleZ;
            float accX;
            float accY;
            float accZ;
        };
        struct GPSData
        {
            // TODO: Implement GPS
        };

        MPUData mpuData;
        GPSData GPSData;
        unsigned long time;
    };

    PacketType packetType;
    uint8_t length;
    union {
        byte bytes[255];
        RocketDataPacket rocketData;
    };
    uint16_t crc;

    void calcCrc();
};

class ICommProtocol
{
public:
    Packet packetBuffer;

    static void timeout(unsigned long start);

    void sendPacket();
    void recievePacket();
    virtual void begin() = 0;

private:
    virtual void startPacketRead() = 0;
    virtual void endPacketRead() = 0;
    virtual void startPacketSend() = 0;
    virtual void endPacketSend() = 0;
    virtual void sendByte(byte b) = 0;
    virtual byte readByte() = 0;
};

#endif