#ifndef CHANNEL_H
#define CHANNEL_H

#include "helpers.h"
#include "memory.h"
#include "iodevice.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>

struct channelstatus {
    unsigned int key : 4; //Storage Protection Key
    unsigned int pc : 24; //Program counter
    unsigned int usc : 8; //Unit status conditions
    unsigned int csc : 8; //Channel status conditions
    unsigned int count : 16;
};

class channel {
    public:
    channel(std::shared_ptr<memory> coreptr);
    ~channel();
    void addDevice(int addr, iodevice* devptr);
    void startIO(int devaddr);
    word packCSW();
    private:
    int devaddr;
    channelstatus csw;
    std::shared_ptr<memory> coreptr;
    std::unordered_map<int,iodevice*> devices;
    char buffer[256];
    void readDevToBuf(int devaddr,int numchars);
    void writeBufToDev(int devaddr,int numchars);
    void writeBufToCore(word memaddr,int numbytes);
    void readCoreToBuf(word memaddr,int numbytes);
    void fetchCAW();
    doubleword fetchCCW(); //Fetches channel command word
    void run(); //Function that runs a channel program in a separate thread
    void cycle();
};
#endif