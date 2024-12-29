#ifndef CHANNEL_H
#define CHANNEL_H

#include "iohelpers.h"
#include "helpers.h"
#include "memory.h"
#include <memory>
#include <unordered_map>
#include "subchannel.h"
#include <optional>
#include "iodevice.h"

class channel {
    public:
    channel(std::shared_ptr<memory> memPtr,const byte channelAddress);
    ~channel();
    void addSubchannel(byte subchannelID);
    void addDevice(deviceAddress devaddr,iodevice* devptr); //4 high order bits designate the subchannel, 4 low order bits designate the device ID
    int startIO(deviceAddress devaddr); //Starts the given subchannel, returns a condition code to be stored in the psw
    int haltIO(deviceAddress devaddr); //Immediately halts the given subchannel
    std::optional<halfword> getPendingInterrupts(); //Returns interruption codes if there are any pending interrupts
    const byte channelID;
    private:
    channelstatus csw;
    void storeCSW(); //Stores CSW at address 64 in memory
    word fetchCAW(); //Retrieves CAW from address 72 in memory
    std::shared_ptr<memory> core;
    std::unordered_map<byte,subchannel*> subchannels;
};

#endif