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
#include <deque>
#include <iohelpers.h>
#include <future>

//NOTE: The 4 high-order bits of devaddr specifies the subchannel, while the 4 low-order bits specify the device

enum cmode {
    SELECTOR, // Only permits one channel program to run at a time
    MULTIPLEXER // Allows for multiple channel programs to run concurrently
};

class channel {
    public:
    
    channel(std::shared_ptr<memory> coreptr,const enum cmode cMode,const byte channelAddress);
    ~channel();
    void addDevice(int addr, iodevice* devptr);
    void startIO(byte devaddr);
    doubleword packCSW();
    std::optional<halfword> pendingInterrupts(); //Returns pending interruption code from a completed IO process, if one is available
    std::atomic_bool activeIO;
    private:
    void storeCSW(); //Stores the CSW at address 64 of main memory
    const byte channelAddress;
    const enum cmode channelMode;
    std::atomic<channelstatus> csw;
    std::atomic_int activeProcessCount = 0;
    std::shared_ptr<memory> coreptr;
    std::unordered_map<int,iodevice*> devices;
    halfword activeSubchannels; // A status byte indicating active subchannels
    std::vector<std::optional<IOInterrupt>> activeProcessQueue;
    //std::unordered_map<int,std::thread> deviceThreads;
    void fetchCAW();
    doubleword fetchCCW(); //Fetches channel command word
    void run(int devaddr,std::vector<std::optional<IOInterrupt>>::iterator interruptPtr); //Function that runs a channel program in a separate thread
    void cycle();
    std::mutex channel_mtx; //For protecting access to shared resources
    
};

#endif