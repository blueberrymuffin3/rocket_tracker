#ifndef _SERIAL_COMM_HPP_
#define _SERIAL_COMM_HPP_
#include "communication.hpp"

#define COMM_SERIAL_BAUD 57600
// #define COMM_SERIAL_BAUD 115200

#ifndef COMM_SERIAL_STREAM
#define COMM_SERIAL_STREAM Serial1
#endif

class SerialCommProtocol : public ICommProtocol
{
public:
    virtual void begin() override;

private:
    virtual void startPacketRead() override;
    virtual void endPacketRead() override;
    virtual void startPacketSend() override;
    virtual void endPacketSend() override;
    virtual void sendByte(byte b) override;
    virtual byte readByte() override;
    byte readNybble();
};

#endif