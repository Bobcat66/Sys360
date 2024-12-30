#include <mutex>
#include <thread>
#include <deque>
#include <functional>
#include "channel.h"
#include "iohelpers.h"

//TODO: Reimplement the Asynchronous IO using a thread pool instead of std::async

/*-----------------------------------------------------------------*/
/* MACROS & CONSTEXPR VALUES                                       */
/*-----------------------------------------------------------------*/

constexpr halfword 
subchannelMasks[] = {
    0b1000000000000000, //Subchannel 0
    0b0100000000000000, //Subchannel 1
    0b0010000000000000, //Subchannel 2
    0b0001000000000000, //Subchannel 3
    0b0000100000000000, //Subchannel 4
    0b0000010000000000, //Subchannel 5
    0b0000001000000000, //Subchannel 6
    0b0000000100000000, //Subchannel 7
    0b0000000010000000, //Subchannel 8
    0b0000000001000000, //Subchannel 9
    0b0000000000100000, //Subchannel 10
    0b0000000000010000, //Subchannel 11
    0b0000000000001000, //Subchannel 12
    0b0000000000000100, //Subchannel 13
    0b0000000000000010, //Subchannel 14
    0b0000000000000001  //Subchannel 15
};

/*-----------------------------------------------------------------*/
/* THREAD-LOCAL RESOURCES                                          */
/*-----------------------------------------------------------------*/

static thread_local int thread_devaddr = -1; //Device address of the current thread
static thread_local bool thread_active = false; //A flag that determines whether or not the current thread is actively performing an IO operation
static thread_local std::deque<char> thread_buffer;
static thread_local channelstatus thread_csw;
static thread_local std::vector<std::optional<IOInterrupt>>::iterator thread_processState;

/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

channel::channel(std::shared_ptr<memory> coreptr,const cmode cMode,const byte address)
    :channelMode(cMode),channelAddress(address){
    this->coreptr = coreptr;
    csw = {0,0,0,0,0};
    activeProcessQueue.reserve(16); //Preallocates enough memory to store 16 active processes, 
}

void channel::addDevice(int addr, iodevice* devptr){
    devices.insert(std::make_pair(addr,devptr));
}

std::optional<halfword> channel::pendingInterrupts(){
    std::lock_guard<std::mutex> channelLock(channel_mtx);
    std::optional<halfword> interruptCode;
    for (int i = 0; i < activeProcessQueue.size(); i++){
        auto interrupt = activeProcessQueue[i];
        if (interrupt) {
            bool isFinished = interrupt.value().finished;
            interruptCode = interrupt.value().address; //During IO interrupts, the System/360 stores the address of the IO channel and device in the PSW
            interruptCode.reset(); 
            if (isFinished) {activeProcessQueue.erase(activeProcessQueue.begin() + i);}
            return interruptCode;
        }
    }
    return interruptCode;
}

void channel::startIO(byte devaddr){
    std::lock_guard<std::mutex> channelLock(channel_mtx);
    std::optional<IOInterrupt> processStatus;
    activeProcessQueue.push_back(processStatus);
    std::async(std::launch::async,std::bind(&channel::run,this,devaddr,--activeProcessQueue.end()));
    //activeProcessFutures.insert(std::make_pair(devaddr/(1<<4),ioFuture));
}

/*-----------------------------------------------------------------*/
/* PRIVATE                                                         */
/*-----------------------------------------------------------------*/

void channel::storeCSW(){
    coreptr->writeDoubleword(64,packCSW(),0);
}
void channel::fetchCAW(){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    thread_csw.key = (coreptr->getByteNoSync((word)72,0))/(1<<4);
    thread_csw.pc = ((word)(coreptr->getByteNoSync((word)74,0)) << 16) + ((word)(coreptr->getByteNoSync((word)75,0)) << 8) + ((word)(coreptr->getByteNoSync((word)76,0)));
}

void channel::cycle(){
    if (thread_devaddr == -1){throw std::runtime_error("Error: channel::cycle was called in an inactive thread");}
    doubleword ccw;
    byte opcode;
    word address;
    byte flags;
    halfword originalCount;
    bool noSuppressLen;
    iodevice *dev = devices[thread_devaddr];
    std::unique_lock<std::mutex> memlock(coreptr->mtx);

    //PROGRAM ERROR HANDLING
    try {
        ccw = coreptr->getDoubleword(thread_csw.pc,0);
    } catch (int e){
        thread_csw.csc |= PRGRM;
        thread_active = false;
        goto exit_cycle;
    }

    opcode = ccw / (1 << 56);
    address = (ccw % (1 << 56))/(1 << 32);
    flags = (ccw % (1 << 32))/(1 << 27);
    originalCount = ccw % (1 << 16);
    thread_csw.count = originalCount;
    noSuppressLen = !((flags & SPLN_FLAG) && !(flags & CHDT_FLAG));

    //PROGRAM ERROR HANDLING
    if (!originalCount && !(opcode == 0b1000)){
        thread_csw.csc |= PRGRM;
        thread_active = false;
        goto exit_cycle;
    }

    //The device reads all available data into the datastream, regardless of how much data was requested by the channel. Count errors are handled by the channel
    switch (opcode % (1 << 4)){
        case 0b0000:
            //Invalid
            break;
        case 0b0100:
            //Sense
            thread_buffer.clear();

            dev->acceptCommand(opcode);
            while (dev->service_in()) {
                thread_buffer.push_back(dev->getByte());
                if (thread_csw.count) {
                    thread_csw.count--;
                } else {
                    if (noSuppressLen) {thread_csw.csc |= INCLN;}
                    dev->halt();
                    break;
                }
            }

            if (!(flags & SKIP_FLAG)) {
                memlock.lock();
                for (int i = 0; i < originalCount; i++){
                    try {
                        coreptr->writeByteNoSync(address+i,(byte)thread_buffer.front(),thread_csw.key);
                        thread_buffer.pop_front();
                    } catch (int e) {
                        thread_csw.csc |= PRTCT;
                        thread_active = false;
                        memlock.unlock();
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            thread_buffer.clear();
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
            if (!(flags & SKIP_FLAG)){
                memlock.lock();
                for (int i = 0; i < originalCount; i++){
                    try {
                        thread_buffer.push_back((char)coreptr->getByteNoSync(address+i,thread_csw.key)); 
                    } catch (int e) {
                        thread_csw.csc |= PRTCT;
                        memlock.unlock();
                        thread_active = false;
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            dev->acceptCommand(opcode);
            while (dev->service_in()){
                dev->acceptByte(thread_buffer.front());
                thread_buffer.pop_front();
                if (thread_csw.count) {
                    thread_csw.count--;
                } else {
                    if (noSuppressLen){thread_csw.csc |= INCLN;}
                    break;
                }
            }

            if (thread_buffer.size() && noSuppressLen){thread_csw.csc |= INCLN;}
            break;
        case 0b0010:
        case 0b1010:
        case 0b0110:
        case 0b1110:
            //Read
            thread_buffer.clear();

            dev->acceptCommand(opcode);
            while (dev->service_in()) {
                thread_buffer.push_back(dev->getByte());
                if (thread_csw.count) {
                    thread_csw.count--;
                } else {
                    if (noSuppressLen) {thread_csw.csc |= INCLN;}
                    dev->halt();
                    break;
                }
            }

            if (!(flags & SKIP_FLAG)) {
                memlock.lock();
                for (int i = 0; i < originalCount; i++){
                    try {
                        coreptr->writeByteNoSync(address+i,(byte)thread_buffer.front(),thread_csw.key);
                        thread_buffer.pop_front();
                    } catch (int e) {
                        thread_csw.csc |= PRTCT;
                        thread_active = false;
                        memlock.unlock();
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            thread_buffer.clear();
            break;
        case 0b0011:
        case 0b1011:
        case 0b0111:
        case 0b1111:
            //Control
            break;
    }
    if (flags & PGCI_FLAG) {
        std::unique_lock<std::mutex> channelLock(channel_mtx);
        channelLock.lock();
        *thread_processState = {false,(halfword)(((halfword)channelAddress << 8) | thread_devaddr)};
        csw.store(thread_csw);
        storeCSW();
        channelLock.unlock();
    }
    exit_cycle:
    thread_csw.pc += 8;
    csw.store(thread_csw);
}

//TODO: Add interruptions and exception handling
void channel::run(int devaddr,std::vector<std::optional<IOInterrupt>>::iterator interruptPtr){
    activeProcessCount++;
    //Thread-local initialization
    thread_processState = interruptPtr;
    thread_devaddr = devaddr;
    thread_active = true;
    thread_csw = {0,0,0,0,0};
    std::unique_lock<std::mutex> devLock(devices[thread_devaddr]->mtx); //Locks the device, so no other thread can access it

    //Checks if the device is available
    if ((channelMode == SELECTOR && activeProcessCount >= 1) || (activeSubchannels & subchannelMasks[devaddr/(1<<4)])){
        thread_csw.usc |= CTRBSY;
        goto endthread;
    } //Concurrent operation is disabled in selector mode
    
    activeSubchannels |= subchannelMasks[devaddr/(1<<4)];

    if (devLock.try_lock()){
        fetchCAW();
        while (thread_active) {
            cycle();
        }
        devLock.unlock();
    } else {
        thread_csw.usc |= CTRBSY;
        goto endthread;
    }
    endthread:
    std::unique_lock<std::mutex> channelLock(channel_mtx);
    channelLock.lock();
    *thread_processState = {true,(halfword)(((halfword) channelAddress << 8) | thread_devaddr)};
    csw.store(thread_csw);
    storeCSW();
    channelLock.unlock();
    //Thread-local cleanup
    thread_devaddr = -1; //-1 is a placeholder value
    thread_active = false; //Not technically needed, but kept to make everything more explicit. Chances are the compiler will just optimize it anyways
    thread_buffer.clear(); 
    thread_csw = {0,0,0,0,0};
    activeProcessCount--;
    return;
}



