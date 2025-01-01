#include "subchannel.h"
#include <iostream>

/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

subchannel::subchannel(const byte ID,std::shared_ptr<memory> coreptr)
:subchannelID(ID)
,workerThread{}{
    core = coreptr;
    threadActive = true;
    subchannel_busy = false;
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
    std::unique_lock<std::mutex> comLock(commandAccept_mtx,std::defer_lock);
    ////std::cout << "MEMADDR " << memaddress << std::endl;
    task = std::bind(&subchannel::runChannelProgram,this,devAddr,memaddress,key);
    while (!commandAcceptCode){
        acceptedCommand.wait(comLock);
    }
    return commandAcceptCode.value();
}

int subchannel::haltChannelProgram(){
    if (!subchannel_busy) {return 1;}
    subchannel_busy = false;
    return 0;
}

doubleword subchannel::getCSW(){
    //std::lock_guard<std::mutex> subchannelLock(subchannel_mtx);
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

byte subchannel::getDevID() {
    return deviceID.load();
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
    std::unique_lock<std::mutex> subchannelLock(subchannel_mtx,std::defer_lock);
    std::lock_guard<std::mutex> commandAcceptLock(commandAccept_mtx);
    subchannelLock.lock();
    this->deviceID = devaddr;
    std::unique_lock<std::mutex> devLock(devices[deviceID]->mtx,std::defer_lock);
    csw = {0,0,0,0,0};
    subchannelLock.unlock();

    if (subchannel_busy){
        commandAcceptCode = 2;
        acceptedCommand.notify_all();
        return;
    }

    if (devLock.try_lock()){
        subchannelLock.lock();
        csw.key = key;
        csw.pc = address;
        subchannel_busy = true;
        commandAcceptCode = 0;
        acceptedCommand.notify_all();
        subchannelLock.unlock();
        while (subchannel_busy) {
            cycle();
        }
        devLock.unlock();
    } else {
        csw.usc |= CTRBSY;
        commandAcceptCode = 2;
        acceptedCommand.notify_all();
        return;
    }

    subchannel_busy = false;
    pendingInterrupts++;
    buffer.clear();
}

void subchannel::cycle() {
    std::lock_guard<std::mutex> subchannelLock(subchannel_mtx);
    //std::cout << "CYCLE" << std::endl;
    doubleword ccw;
    byte opcode;
    word address;
    byte flags;
    halfword originalCount;
    bool noSuppressLen;
    iodevice *dev = devices[deviceID];
    std::unique_lock<std::mutex> memlock(core->mtx,std::defer_lock);
    //PROGRAM ERROR HANDLING
    try {
        //std::cout << "FETCHING CCW" << std::endl;
        //std::cout << csw.pc << std::endl;
        ccw = core->getDoubleword(csw.pc,0);
        //std::cout << "FETCHED CCW" << std::endl;
    } catch (int e){
        csw.csc |= PRGRM;
        subchannel_busy = false;
        goto exit_cycle;
    }

    opcode = ccw / ((doubleword)1 << 56);
    address = (ccw % ((doubleword)1 << 56))/(doubleword)((doubleword)1 << 32);
    flags = (ccw % ((doubleword)1 << 32))/((doubleword)1 << 27);
    originalCount = ccw % ((doubleword)1 << 16);
    csw.count = originalCount;
    noSuppressLen = !((flags & SPLN_FLAG) && !(flags & CHDT_FLAG));
    //std::cout << "LOADED CCW" << std::endl;
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
                buffer.push_back(dev->getChar());
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
                        //std::cout << i << std::endl;
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
                dev->acceptChar(buffer.front());
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
                buffer.push_back(dev->getChar());
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
        pendingInterrupts++;
    }

    exit_cycle:
    csw.pc += 8;
    if (!(flags & CHCM_FLAG)){
        subchannel_busy.store(false);
    }
}