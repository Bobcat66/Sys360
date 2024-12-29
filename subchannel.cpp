#include "subchannel.h"

/*-----------------------------------------------------------------*/
/* GLOBAL VARS                                                     */
/*-----------------------------------------------------------------*/

std::mutex command_mtx;

/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

subchannel::subchannel(const byte ID,std::shared_ptr<memory> coreptr)
:subchannelID(ID)
,workerThread{}{
    core = coreptr;
    threadActive = true;
    workerThread = std::thread{&subchannel::runThread,this};
}

subchannel::~subchannel() {
    threadActive = false;
    workerThread.join();
}

void subchannel::addDevice(byte devAddr,iodevice* devptr){
    std::lock_guard<std::mutex> subLock(subchannel_mtx);
    devices.insert(std::make_pair(devAddr,devptr));
}

int subchannel::startChannelProgram(byte devAddr, word memaddress, byte key){
    std::unique_lock<std::mutex> comLock(commandAccept_mtx);
    task = std::bind(&subchannel::runChannelProgram,this,devAddr,memaddress,key);
    acceptedCommand.wait(comLock);
    return commandAcceptCode.load();
}

int subchannel::haltChannelProgram(){
    if (!subchannel_busy) {return 1;}
    subchannel_busy = false;
    return 0;
}

doubleword subchannel::getCSW(){
    std::lock_guard<std::mutex> subchannelLock(subchannel_mtx);
    doubleword packedCSW = csw.key%(1<<4);
    packedCSW <<= 28;
    packedCSW |= csw.pc;
    packedCSW <<= 8;
    packedCSW |= csw.csc;
    packedCSW <<= 8;
    packedCSW |= csw.usc;
    packedCSW <<= 16;
    packedCSW |= csw.count;
    return packedCSW;
}



/*-----------------------------------------------------------------*/
/* PRIVATE                                                         */
/*-----------------------------------------------------------------*/

void subchannel::runThread() {
    while (threadActive) {
        if (task) {
            task.value()(); //Runs task
            task.reset(); //Resets task after it is finished
        }
    }
}

void subchannel::runChannelProgram(byte devaddr,word address,byte key){
    std::unique_lock<std::mutex> subchannelLock(subchannel_mtx);

    subchannelLock.lock();
    this->deviceID = devaddr;
    std::unique_lock<std::mutex> devLock(devices[deviceID]->mtx);
    csw = {0,0,0,0,0};
    subchannelLock.unlock();

    if (subchannel_busy){
        {   
            std::lock_guard<std::mutex> acceptGuard(commandAccept_mtx);
            commandAcceptCode.store(2);
        }
        acceptedCommand.notify_all();
        return;
    }

    if (devLock.try_lock() == 0){
        subchannelLock.lock();
        csw.key = key;
        csw.pc = address;
        subchannel_busy = true;
        {   
            std::lock_guard<std::mutex> acceptGuard(commandAccept_mtx);
            commandAcceptCode.store(0);
        }
        acceptedCommand.notify_all();
        subchannelLock.unlock();
        while (subchannel_busy) {
            cycle();
        }
        devLock.unlock();
    } else {
        csw.usc |= CTRBSY;
        commandAcceptCode.store(2);
        acceptedCommand.notify_all();
        return;
    }

    this->deviceID = NULL;
    subchannel_busy = false;
    pendingInterrupt = true;
    buffer.clear();
}

void subchannel::cycle() {
    std::lock_guard<std::mutex> subchannelLock(subchannel_mtx);
    doubleword ccw;
    byte opcode;
    word address;
    byte flags;
    halfword originalCount;
    bool noSuppressLen;
    iodevice *dev = devices[deviceID];
    std::unique_lock<std::mutex> memlock(core->mtx);

    //PROGRAM ERROR HANDLING
    try {
        ccw = core->getDoubleword(csw.pc,0);
    } catch (int e){
        csw.csc |= PRGRM;
        subchannel_busy = false;
        goto exit_cycle;
    }

    opcode = ccw / (1 << 56);
    address = (ccw % (1 << 56))/(1 << 32);
    flags = (ccw % (1 << 32))/(1 << 27);
    originalCount = ccw % (1 << 16);
    csw.count = originalCount;
    noSuppressLen = !((flags & SPLN_FLAG) && !(flags & CHDT_FLAG));

    //PROGRAM ERROR HANDLING
    if (!originalCount && !(opcode == 0b1000)){
        csw.csc |= PRGRM;
        subchannel_busy = false;
        goto exit_cycle;
    }

    //The device reads all available data into the datastream, regardless of how much data was requested by the channel. Count errors are handled by the channel
    switch (opcode % (1 << 4)){
        case 0b0000:
            //Invalid
            break;
        case 0b0100:
            //Sense
            buffer.clear();

            dev->acceptCommand(opcode);
            while (dev->service_in()) {
                buffer.push_back(dev->getByte());
                if (csw.count) {
                    csw.count--;
                } else {
                    if (noSuppressLen) {csw.csc |= INCLN;}
                    dev->halt();
                    break;
                }
            }

            if (!(flags & SKIP_FLAG)) {
                memlock.lock();
                for (int i = 0; i < originalCount; i++){
                    try {
                        core->writeByteNoSync(address+i,(byte)buffer.front(),csw.key);
                        buffer.pop_front();
                    } catch (int e) {
                        csw.csc |= PRTCT;
                        subchannel_busy = false;
                        memlock.unlock();
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            buffer.clear();
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
                        buffer.push_back((char)core->getByteNoSync(address+i,csw.key)); 
                    } catch (int e) {
                        csw.csc |= PRTCT;
                        memlock.unlock();
                        subchannel_busy = false;
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            dev->acceptCommand(opcode);
            while (dev->service_in()){
                dev->acceptByte(buffer.front());
                buffer.pop_front();
                if (csw.count) {
                    csw.count--;
                } else {
                    if (noSuppressLen){csw.csc |= INCLN;}
                    break;
                }
            }

            if (buffer.size() && noSuppressLen){csw.csc |= INCLN;}
            break;
        case 0b0010:
        case 0b1010:
        case 0b0110:
        case 0b1110:
            //Read
            buffer.clear();

            dev->acceptCommand(opcode);
            while (dev->service_in()) {
                buffer.push_back(dev->getByte());
                if (csw.count) {
                    csw.count--;
                } else {
                    if (noSuppressLen) {csw.csc |= INCLN;}
                    dev->halt();
                    break;
                }
            }

            if (!(flags & SKIP_FLAG)) {
                memlock.lock();
                for (int i = 0; i < originalCount; i++){
                    try {
                        core->writeByteNoSync(address+i,(byte)buffer.front(),csw.key);
                        buffer.pop_front();
                    } catch (int e) {
                        csw.csc |= PRTCT;
                        subchannel_busy = false;
                        memlock.unlock();
                        goto exit_cycle;
                    }
                }
                memlock.unlock();
            } else {
                goto exit_cycle;
            }

            buffer.clear();
            break;
        case 0b0011:
        case 0b1011:
        case 0b0111:
        case 0b1111:
            //Control
            break;
    }
    if (flags & PGCI_FLAG) {
        pendingInterrupt = true;
    }
    exit_cycle:
    csw.pc += 8;
}