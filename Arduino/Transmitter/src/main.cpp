#include <Arduino.h>
#include <lora_comm.hpp>
#include <try_throw_catch.h>
#include <debug_logging.hpp>

LoRaCommProtocol LoRaComm;

void setup()
{
    initLogging();
    LoRaComm.begin();
};

void loop()
{
    TRY
    {
        LoRaComm.recievePacket();
        switch (LoRaComm.packetBuffer.packetType)
        {
        case Ping: 
            LoRaComm.packetBuffer.packetType = Pong;
            LoRaComm.sendPacket();
            break;
        }
    }
    ETRY
}
