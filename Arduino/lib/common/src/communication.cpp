#include "communication.hpp"
#include "try_throw_catch.h"

void ICommProtocol::timeout(unsigned long start)
{
    if(millis() - start > COMM_TIMEOUT){
        logln("[ERROR] Timout");
        THROW(CommError);
    }
    delay(0);
}

void crc16_update(uint16_t &crc, uint8_t a)
{
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
        else
        crc = (crc >> 1);
    }
}

void Packet::calcCrc(){
    crc = 0xFFFF;
    crc16_update(crc, packetType);
    crc16_update(crc, length);
    for (uint8_t i = 0; i < length; i++)
    {
        crc16_update(crc, bytes[i]);
    }
}

void ICommProtocol::recievePacket()
{
    logln("Recieving Packet...");

    startPacketRead();
    log("Packet Type:");
    packetBuffer.packetType = (PacketType)readByte();
    logln();
    log("Length:");
    packetBuffer.length = readByte();
    logln();

    for (uint8_t i = 0; i < packetBuffer.length; i++)
    {
        packetBuffer.bytes[i] = readByte();
    }
    logln();
    log("crc:");
    byte crc1B = readByte();
    byte crc2B = readByte();
    uint16_t crcB = (crc1B << 8) | crc2B;
    packetBuffer.calcCrc();

    if(crcB == packetBuffer.crc){
        logln(" ✓");
    } else {
        logln(" X");
        logln("[ERROR] CRC Invalid, should be ");
        logln(packetBuffer.crc, HEX);
        endPacketRead();
        THROW(CommError);
    }
    endPacketRead();
};

// Packets are send with a CRC16 checksum and are seperated by a newline char
void ICommProtocol::sendPacket()
{
    logln("Sending Packet...");
    startPacketSend();
    sendByte(packetBuffer.packetType);
    sendByte(packetBuffer.length);
    for (uint8_t i = 0; i < packetBuffer.length; i++)
    {
        sendByte(packetBuffer.bytes[i]);
    }
    packetBuffer.calcCrc();
    sendByte(packetBuffer.crc >> 8);
    sendByte(packetBuffer.crc & 0xFF);
    endPacketSend();
};
