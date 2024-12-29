#ifndef SUBCHANNEL_H
#define SUBCHANNEL_H

#include "helpers.h"
#include <atomic>
#include <mutex>
#include <iodevice.h>
#include <unordered_map>
#include <functional>
#include <optional>
#include <deque>
#include "iohelpers.h"
#include <memory>
#include "memory.h"
#include <thread>

//TODO: Implement custom thread pool
class subchannel {
    public:
    subchannel(const byte ID,std::shared_ptr<memory> coreptr);
    ~subchannel();
    void addDevice(byte devAddr,iodevice* devptr);
    int startChannelProgram(byte devAddr,word address,byte key);
    int haltChannelProgram();
    doubleword getCSW();
    const byte subchannelID;
    std::atomic_bool pendingInterrupt;
    std::atomic_bool threadActive;
    private:
    std::thread workerThread;
    channelstatus csw;
    std::deque<char> buffer;
    std::shared_ptr<memory> core;
    byte deviceID;
    std::unordered_map<byte,iodevice*> devices;
    std::optional<std::function<void()>> task;
    std::atomic_bool subchannel_busy;
    std::mutex subchannel_mtx;
    void runThread();
    void runChannelProgram(byte devaddr,word address,byte key);
    void cycle();
};
#endif