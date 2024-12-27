#ifndef CHANNEL_H
#define CHANNEL_H

#include "helpers.h"
#include "memory.h"
#include "iodevice.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <atomic>
#include <optional>
#include <thread>
#include <future>

//NOTE: The 4 high-order bits of devaddr specifies the subchannel, while the 4 low-order bits specify the device

struct channelstatus {
    unsigned int key : 4; //Storage Protection Key
    unsigned int pc : 24; //Program counter
    unsigned int usc : 8; //Unit status conditions
    unsigned int csc : 8; //Channel status conditions
    unsigned int count : 16;
};

enum cmode {
    SELECTOR, // Only permits one channel program to run at a time
    MULTIPLEXER // Allows for multiple channel programs to run concurrently
};

class channel {
    public:
    
    channel(std::shared_ptr<memory> coreptr,const enum cmode cMode);
    ~channel();
    void addDevice(int addr, iodevice* devptr);
    void startIO(byte devaddr);
    word packCSW();
    std::optional<byte> pendingInterrupts(); //Returns pending interruption code from a completed IO process, if one is available
    std::atomic_bool activeIO;
    private:
    const enum cmode channelMode;
    std::atomic<channelstatus> csw;
    std::atomic_int activeProcessCount = 0;
    std::shared_ptr<memory> coreptr;
    std::unordered_map<int,iodevice*> devices;
    std::unordered_map<int,std::future<int>> activeProcessFutures;
    //std::unordered_map<int,std::thread> deviceThreads;
    void fetchCAW();
    doubleword fetchCCW(); //Fetches channel command word
    int run(int devaddr); //Function that runs a channel program in a separate thread
    void cycle();
    std::mutex channel_mtx; //For protecting access to shared resources
};

#endif