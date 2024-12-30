#ifndef SUBCHANNEL_H
#define SUBCHANNEL_H

#include "helpers.h"
#include <atomic>
#include <mutex>
#include "iodevice.h"
#include <unordered_map>
#include <functional>
#include <optional>
#include <deque>
#include "iohelpers.h"
#include <memory>
#include "memory.h"
#include <thread>
#include <unordered_set>
#include <condition_variable>

//TODO: Implement custom thread pool
class subchannel {
    public:
    subchannel(const byte ID,std::shared_ptr<memory> coreptr);
    subchannel(subchannel&& other);
    ~subchannel();
    void addDevice(byte devAddr,iodevice* devptr);
    int startChannelProgram(byte devAddr,word address,byte key);
    int haltChannelProgram();
    doubleword getCSW(); //Returns packed CSW. WARNING: NOT THREAD-SAFE, USE WITH CAUTION
    byte getDevID();
    const byte subchannelID;
    std::atomic_int pendingInterrupts{0};
    std::atomic_bool threadActive;
    std::mutex subchannel_mtx;
    private:
    std::condition_variable acceptedCommand;
    std::optional<int> commandAcceptCode;
    std::thread workerThread;
    channelstatus csw;
    std::deque<char> buffer;
    std::shared_ptr<memory> core;
    std::atomic_uchar deviceID;
    std::unordered_map<byte,iodevice*> devices;
    std::optional<std::function<void()>> task;
    std::atomic_bool subchannel_busy;
    std::mutex commandAccept_mtx;
    void runThread();
    void runChannelProgram(byte devaddr,word address,byte key);
    void cycle();
};
#endif