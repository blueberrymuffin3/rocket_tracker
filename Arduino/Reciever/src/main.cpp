#include <Arduino.h>
#include <serial_comm.hpp>
#include <lora_comm.hpp>
#include <try_throw_catch.h>
#include <debug_logging.hpp>

SerialCommProtocol SerialComm;
LoRaCommProtocol LoRaComm;

void setup() {
    initLogging();
    SerialComm.begin();
    LoRaComm.begin();
};

void ping(ICommProtocol &Comm, const char *name) {
    TRY {
        log("Pinging ");
        logln(name);
        unsigned long start = micros();
        Comm.packetBuffer.packetType = Ping;
        Comm.packetBuffer.length = 32;
        for (uint8_t i = 0; i < Comm.packetBuffer.length; i++)
        {
            Comm.packetBuffer.bytes[i] = i;
        }
        
        Comm.sendPacket();
        logln("Waiting for a response..");
        Comm.recievePacket();

        if(Comm.packetBuffer.packetType != Pong){
            log("[ERROR] Unexpexted Packet type ");
            logln(Comm.packetBuffer.packetType, HEX);
            THROW(CommError);
        }

        unsigned long stop = micros();

        log(name);
        log(" ping is ");
        log(stop - start);
        logln("μs");
    } CATCH(CommError) {
        logln("[ERROR] CommError");
    } ETRY
};

void loop() {
    logln();
    logln();
    logln();
    ping(SerialComm, "SerialComm");
    ping(LoRaComm, "LoRaComm");
    delay(5000);
}
