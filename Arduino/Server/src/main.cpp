#include <Arduino.h>
#include <serial_comm.hpp>
#include <try_throw_catch.h>
#include <debug_logging.hpp>

SerialCommProtocol SerialComm;

void setup()
{
    initLogging();
    SerialComm.begin();
};

void loop()
{
    TRY
    {
        SerialComm.recievePacket();
        switch (SerialComm.packetBuffer.packetType)
        {
        case Ping: 
            SerialComm.packetBuffer.packetType = Pong;
            SerialComm.sendPacket();
            break;
        }
    }
    ETRY
}
