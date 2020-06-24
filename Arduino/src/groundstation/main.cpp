#include <Arduino.h>

// #define DEBUG_STREAM Serial1

#include "../common/LoRaPackets.hpp"
#include "../common/logging.hpp"

void setup()
{
  BEGIN_LOG(115200);
  Serial1.begin(9600);

  while (!Serial && millis() < 5000)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  LOGF("Starting LoRa Radio...");
  LOG(LoRaControl.begin() ? F("OK\n") : F("ERROR\n"));
}

uint8_t lastPacketNumMSBs = 0x01;
uint8_t packetCount = 0;

void loop()
{
  LOGF("Recieving ");
  uint8_t packetID = LoRaControl.receivePacket();
  digitalWrite(LED_BUILTIN, HIGH);
  LOGF("rssi=");
  LOG(LoRaControl.lastRSSI);
  LOGF(" ");

  if (packetID == INVALID_PACKET)
  {
    LOGF("Invalid Packet\n");
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }

  switch (packetID)
  {
  case IMUPacket::ID:
  {
    IMUPacket packet = LoRaControl.getPacket<IMUPacket>();
    LOGF("IMUPacket(w=");
    LOG(packet.data.w);
    LOGF(", x=");
    LOG(packet.data.x);
    LOGF(", y=");
    LOG(packet.data.y);
    LOGF(", z=");
    LOG(packet.data.z);
    LOGF(")\n");
    serializeJson(packet.toJson(), Serial1);
    Serial1.write('\n');
    break;
  }

  case GPSPacket::ID:
  {
    GPSPacket packet = LoRaControl.getPacket<GPSPacket>();
    LOGF("GPSPacket(\n\tvalid=");
    LOG(packet.data.valid ? F("yes") : F("no"));
    LOGF(", lat=");
    LOG(packet.data.lat, 6);
    LOGF("\u00B0, lng=");
    LOG(packet.data.lng, 6);
    LOGF("\u00B0, alt=");
    LOG(packet.data.alt);
    LOGF("m, sats=");
    LOG(packet.data.sats);
    LOGF(")\n");
    serializeJson(packet.toJson(), Serial1);
    Serial1.write('\n');
    break;
  }

  case INVALID_PACKET:
  {
    break;
  }

  default:
  {
    LOGF("Unknown Packet(id=");
    LOG(packetID);
    LOGF(")\n");
    break;
  }
  }

  uint8_t packetNumMSBs = LoRaControl.packetNum & 0x40;
  if (packetNumMSBs != lastPacketNumMSBs)
  {
    LossPacket packet((64 - packetCount) / 64.0f);
    LOGF("LossPacket(pct=");
    LOG(packet.pct);
    LOGF(")\n");

    serializeJson(packet.toJson(), Serial1);
    Serial1.write('\n');

    packetCount = 0;
  }

  lastPacketNumMSBs = packetNumMSBs;
  packetCount++;

  digitalWrite(LED_BUILTIN, LOW);
}
