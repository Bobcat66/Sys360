#include "subchannel.h"

void subchannel::runThread() {
    while (threadActive) {
        if (task) {
            task.value()(); //Runs task
            task.reset(); //Resets task after it is finished
        }
    }
}

void subchannel::runChannelProgram(byte devaddr,word address,byte key){
    this->deviceID = devaddr;
    subchannel_busy = true;
    csw = {key,0,0,0,0};
    std::unique_lock<std::mutex> devLock(devices[deviceID]->mtx);

    if (devLock.try_lock()){
        csw.pc = address;
        while (subchannel_busy) {
            cycle();
        }
        devLock.unlock();
    } else {
        csw.usc |= CTRBSY;
    }

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