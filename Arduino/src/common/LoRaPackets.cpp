#include <LoRa.h>
#include <util/crc16.h>

#include "LoRaPackets.hpp"
#include "logging.hpp"

#define __STRING(s) #s
#define _STRING(s) __STRING(s)

IMUPacket::IMUPacket() : id(ID) {}
GPSPacket::GPSPacket() : id(ID) {}

IMUPacket::IMUPacket(float w, float x, float y, float z) : id(ID)
{
    data = {.w = w, .x = x, .y = y, .z = z};
}
#ifdef PACKET_ENCODER
GPSPacket::GPSPacket(TinyGPSPlus gps) : id(ID)
{
    data.valid = gps.location.isValid();
    data.lat = gps.location.lat();
    data.lng = gps.location.lng();
    data.alt = gps.altitude.meters();

    data.sats = gps.satellites.value();
}
#endif

#ifdef PACKET_DECODER
// https://arduinojson.org/v6/assistant/
StaticJsonDocument<IMUPacket::JSON_DOC_SIZE> IMUPacket::toJson()
{
    StaticJsonDocument<IMUPacket::JSON_DOC_SIZE> doc;

    doc["type"] = "IMU";
    doc["rssi"] = LoRaControl.lastRSSI;

    JsonObject _data = doc.createNestedObject("data");
    _data["w"] = data.w;
    _data["x"] = data.x;
    _data["y"] = data.y;
    _data["z"] = data.z;

    return doc;
}
StaticJsonDocument<GPSPacket::JSON_DOC_SIZE> GPSPacket::toJson()
{
    StaticJsonDocument<GPSPacket::JSON_DOC_SIZE> doc;

    doc["type"] = "GPS";
    doc["rssi"] = LoRaControl.lastRSSI;

    JsonObject _data = doc.createNestedObject("data");

    _data["valid"] = data.valid;
    _data["lat"] = data.lat;
    _data["lng"] = data.lng;
    _data["alt"] = data.alt;
    _data["sats"] = data.sats;

    return doc;
}

StaticJsonDocument<LossPacket::JSON_DOC_SIZE> LossPacket::toJson()
{
    StaticJsonDocument<LossPacket::JSON_DOC_SIZE> doc;

    doc["type"] = "LOSS";
    doc["rssi"] = LoRaControl.lastRSSI;

    JsonObject _data = doc.createNestedObject("data");
    _data["pct"] = pct;

    return doc;
}

LossPacket::LossPacket(float pct) : pct(pct) {}
#endif

_LoRaControl LoRaControl;

#ifdef PACKET_ENCODER
void _LoRaControl::_sendPacket(uint8_t packetID, const uint8_t *packet, uint8_t packetSize)
{
    LOGF("Sending packet with id ");
    LOG(packetID);
    LOGF("... ");
    unsigned long start = millis();

    buffer[0] = packetID;
    buffer[1] = packetNum++;

    bufferLength = PACKET_METADATA_SIZE;
    for (uint8_t i = 0; i < packetSize; i++)
    {
        buffer[bufferLength++] = packet[i];
    }

    uint16_t crc16 = calculateCRC();
    buffer[2] = crc16;
    buffer[3] = crc16 >> 8;

    LoRa.beginPacket();
    LoRa.write(buffer, bufferLength);
    LoRa.endPacket();

    LOG(millis() - start);
    delay(50); // prevent lost packets?
    LOGF("ms\n");
}
#endif

#ifdef PACKET_DECODER
int _LoRaControl::receivePacket()
{
    do
    {
        bufferLength = LoRa.parsePacket();
    } while (bufferLength == 0);

    lastRSSI = LoRa.packetRssi();

    LoRa.readBytes(buffer, bufferLength);

    if (bufferLength < PACKET_METADATA_SIZE)
    {
        LOGF("Too Small ");
        LOG(bufferLength);
        LOGF(", expected >= " _STRING(PACKET_METADATA_SIZE));
        return INVALID_PACKET;
    }
    uint8_t ID = buffer[0];
    packetNum = buffer[1];

    uint16_t actual_crc16 = buffer[2];
    actual_crc16 |= (uint16_t)(buffer[3]) << 8;

    uint16_t expected_crc16 = calculateCRC();

    if (actual_crc16 != expected_crc16)
    {
        LOGF("Invalid CRC, got 0x");
        LOG(actual_crc16, HEX);
        LOGF(", expected 0x");
        LOG(expected_crc16, HEX);
        LOGF(" ");
        return INVALID_PACKET;
    }
    return ID;
}
#endif

uint16_t _LoRaControl::calculateCRC()
{
    uint16_t crc16 = 0xffff;
    for (uint8_t i = PACKET_METADATA_SIZE; i < bufferLength; i++)
    {
        crc16 = _crc16_update(crc16, buffer[i]);
    }
    return crc16;
}

bool _LoRaControl::begin()
{
    LoRa.setPins(8, 4, 14);
    if (LoRa.begin(915E6))
    {
        LoRa.setSyncWord(0x6a);
        LoRa.setSpreadingFactor(8);
        LoRa.setSignalBandwidth(125E3);
        LoRa.setCodingRate4(3);
#ifdef PACKET_DECODER
        LoRa.receive();
#endif
        return true;
    }
    else
    {
        return false;
    }
}
