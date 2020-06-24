#ifndef NO_RADIO
#include "lora_comm.hpp"
#include "try_throw_catch.h"
#include <LoRa.h>

#define SS 8
#define RST 4
#define INT 7
#define FREQ 915e6

void LoRaCommProtocol::begin(){
    pinMode(RST, OUTPUT);
    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(10);
    digitalWrite(RST, HIGH);
    delay(10);

    logln("LoRa intiializing...");
    LoRa.setPins(SS, RST, INT);
    if (!LoRa.begin(FREQ)) {
        logln("[ERROR] LoRa init failed. Check your connections.");
        while (true);
    }
    // Uncomment for better range but slower speed
    /*
    logln("Setting SpreadingFactor to SF10");
    LoRa.setSpreadingFactor(10); // (7 - 12, bigger is better range)
    logln("Setting Bandwidth to 10.4khz");
    LoRa.setSignalBandwidth(10.4e3); // (smaller is better range)
    */
    
    logln("LoRa Init Success");
}

void LoRaCommProtocol::startPacketRead(){
    unsigned long start = millis();
    while(LoRa.parsePacket() == 0) ICommProtocol::timeout(start);
}

void LoRaCommProtocol::endPacketRead(){
    while(LoRa.available()) LoRa.read();
}

void LoRaCommProtocol::startPacketSend(){
    LoRa.beginPacket();
}

void LoRaCommProtocol::endPacketSend(){
    LoRa.endPacket();
}

void LoRaCommProtocol::sendByte(byte b){
    LoRa.write(b);
}

byte LoRaCommProtocol::readByte(){
    int b = LoRa.read();
    if(b < 0) {
        logln("[ERROR] No byte to read");
        THROW(CommError);
    }
    #ifdef DEBUG_LOGGING
    byte high = (b & 0xF0) >> 4;
    byte low = b & 0x0F;
    log(' ');
    log(high, HEX);
    log(low, HEX);
    #endif
    return b;
}
#endif
