#include "serial_comm.hpp"
#include "try_throw_catch.h"

void throwInvalidCharError(){
    logln();
    logln("[ERROR] Invalid Char");
    THROW(CommError);
}

void SerialCommProtocol::begin()
{
    COMM_SERIAL_STREAM.begin(COMM_SERIAL_BAUD);
};

// Bytes are send uppercase-hex-encoded
void SerialCommProtocol::sendByte(byte b)
{
    byte hn = b >> 4;
    byte ln = b & 0x0f;
    COMM_SERIAL_STREAM.write((hn > 9 ? ('A' - 10) : '0') + hn);
    COMM_SERIAL_STREAM.write((ln > 9 ? ('A' - 10) : '0') + ln);
};

byte SerialCommProtocol::readByte()
{
    log(' ');
    byte nybble1 = readNybble();
    byte nybble2 = readNybble();
    byte b = (nybble1 << 4) | nybble2;

    return b;
};


byte SerialCommProtocol::readNybble()
{
    unsigned long start = millis();
    while (!COMM_SERIAL_STREAM.available()) timeout(start);

    char nybble = COMM_SERIAL_STREAM.read();
    log(nybble);

    if (nybble < '0')
        throwInvalidCharError();
    else if (nybble <= '9')
        return nybble - '0';
    else if (nybble < 'A')
        throwInvalidCharError();
    else if (nybble <= 'F')
        return nybble - ('A' - 10);
    else
        throwInvalidCharError();
}

void SerialCommProtocol::startPacketRead() {
    unsigned long start = millis();
    while (COMM_SERIAL_STREAM.read() != '\n') ICommProtocol::timeout(start);
};
void SerialCommProtocol::endPacketRead() {};

void SerialCommProtocol::startPacketSend() {
    COMM_SERIAL_STREAM.print('\n');
};
void SerialCommProtocol::endPacketSend() {};
