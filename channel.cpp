#include <mutex>
#include <thread>
#include <functional>
#include "channel.h"

#define CDMASK   0b10000
#define CCMASK   0b01000
#define SLIMASK  0b00100
#define SKIPMASK 0b00010
#define PCIMASK  0b00001

//Thread-local resources
static thread_local int thread_devaddr = -1; //Device address of the current thread
static thread_local bool thread_active = false; //A flag that determines whether or not the current thread is actively performing an IO operation
static thread_local std::vector<char> thread_buffer;
static thread_local channelstatus thread_csw;

channel::channel(std::shared_ptr<memory> coreptr,const cmode cMode)
    :channelMode(cMode) {
    this->coreptr = coreptr;
    csw = {0,0,0,0,0};
}

void channel::addDevice(int addr, iodevice* devptr){
    devices[addr] = devptr;
}

void channel::fetchCAW(){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    thread_csw.key = coreptr->getByte((word)72,0);
    thread_csw.pc = ((word)(coreptr->getByte((word)74,0)) << 16) + ((word)(coreptr->getByte((word)75,0)) << 8) + ((word)(coreptr->getByte((word)76,0)));
}

doubleword channel::fetchCCW(){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    doubleword ccw = 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc,0)) << 56) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+1,0)) << 48) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+2,0)) << 40) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+3,0)) << 32) +
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+4,0)) << 24) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+5,0)) << 16) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+6,0)) << 8) + 
    ((doubleword)(coreptr->getByte((word)thread_csw.pc+7,0)));
    thread_csw.pc += 8;
    return ccw;
}

std::optional<byte> channel::pendingInterrupts(){
    std::lock_guard<std::mutex> channelLock(channel_mtx);
    std::optional<byte> interruptCode;
    for (auto& [key,value] : activeProcessFutures){
        if (value.valid()) {
            interruptCode = value.get();
            activeProcessFutures.erase(key);
            return interruptCode;
        }
    }
    return interruptCode;
}

void channel::cycle(){
    if (thread_devaddr == -1){
        throw std::runtime_error("Error: channel::cycle was called in an inactive thread");
    }
    doubleword ccw = fetchCCW();
    byte opcode = ccw / (1 << 56);
    word address = (ccw % (1 << 56))/(1 << 32);
    byte flags = (ccw % (1 << 32))/(1 << 27);
    halfword count = ccw % (1 << 16);

    bool chainData = flags & CDMASK;
    bool chainCommand = flags & CCMASK;
    bool suppressLengthError = flags & SLIMASK;
    bool skip = flags & SKIPMASK;
    bool progInterrupt = flags & PCIMASK;

    //The device reads all available data into the datastream, regardless of how much data was requested by the channel. Count errors are handled by the channel
    switch (opcode % (1 << 4)){
        case 0b0000:
            //Invalid
            break;
        case 0b0100:
            //Sense
            {
                char c;
                devices[thread_devaddr]->acceptCommand(opcode);
                thread_buffer.clear();
                while (c) {
                    *(devices[thread_devaddr]->getDatastreamPtr()) >> c;
                    thread_buffer.push_back(c);
                }
                if (thread_buffer.size() > count){
                    
                }
                if (!skip) {

                }
            }
            break;
        case 0b1000:
            //Transfer in channel
            break;
        case 0b1100:
            //Read backwards
            break;
        case 0b0001:
        case 0b1001:
        case 0b0101:
        case 0b1101:
            //Write
            break;
        case 0b0010:
        case 0b1010:
        case 0b0110:
        case 0b1110:
            //Read
            {
                char c;
                devices[thread_devaddr]->acceptCommand(opcode);
                thread_buffer.clear();
                while (c) {
                    *(devices[thread_devaddr]->getDatastreamPtr()) >> c;
                    thread_buffer.push_back(c);
                }
                if (thread_buffer.size() > count){
                    
                }
                if (!skip) {

                }
            }
            break;
        case 0b0011:
        case 0b1011:
        case 0b0111:
        case 0b1111:
            //Control
            break;
    }
    csw.store(thread_csw);
}

//TODO: Add interruptions and exception handling
int channel::run(int devaddr){
    activeProcessCount++;
    //Thread-local initialization
    thread_devaddr = devaddr;
    thread_active = true;
    thread_buffer.reserve(4096); //Each thread has a buffer of 4 KiB
    std::lock_guard<std::mutex> devLock(devices[thread_devaddr]->mtx); //Locks the device, so no other thread can access it
    fetchCAW();
    while (thread_active) {
        cycle();
    }
    //Thread-local cleanup
    thread_devaddr = -1; //-1 is a placeholder value
    thread_active = false; //Not technically needed, but kept to make everything more explicit. Chances are the compiler will just optimize it anyways
    thread_buffer.clear();
    thread_csw = {0,0,0,0,0};

    activeProcessCount--;
    return 0;
}

void channel::startIO(byte devaddr){
    std::lock_guard<std::mutex> channelLock(channel_mtx);
    if (channelMode == SELECTOR && activeProcessCount >= 1){return;} //Concurrent operation is disabled in selector mode
    if (activeProcessFutures.count(devaddr/(1<<4))) {return;} //Operation is disabled if the specified subchannel is already working
    std::future<int> ioFuture = std::async(std::launch::async,std::bind(&channel::run,this,devaddr));
    activeProcessFutures.insert(std::make_pair(devaddr/(1<<4),ioFuture));
}



