#ifndef NO_RADIO
#ifndef _LORA_COMM_HPP_
#define _LORA_COMM_HPP_
#include "communication.hpp"

class LoRaCommProtocol : public ICommProtocol
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
};

#endif
#endif
