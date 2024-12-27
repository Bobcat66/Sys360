#include "cpu.h"
#include "cpuhelpers.h"

cpu::cpu(std::shared_ptr<memory> memptr,std::unordered_map<byte,instruction> &ISARef,std::ostream& outputLogRef)
    :outputLog(outputLogRef),ISA(ISARef){
    core = memptr;
    psw = {0,0,0,0,0,0,0,0,0,0,0};
    rgstrs = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0}
    };
}

byte cpu::getByte(word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Reading byte from " << std::hex << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime();
    }
    return getByteNoLock(address);
}

void cpu::setMode(enum CPUMode newMode){
    mode = newMode;
    if (verbose) {
        switch (mode)  {
            case CONTROL:
                outputLog << "------------CONTROL UNIT------------" << std::endl;
            case EXECUTION:
                outputLog << "------------EXECUTE UNIT------------" << std::endl;
            case MEMORY:
                outputLog << "------------MEMMGMT UNIT------------" << std::endl;
            case INTERRUPT:
                outputLog << "------------INTERRUPTION------------" << std::endl;
        }
        outputLog << std::dec << clockUnit.gettime() << " MICROSECONDS" << std::endl;
    }
}

halfword cpu::getHalfword(word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Reading halfword from " << std::hex << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    return getHalfwordNoLock(address);
}

word cpu::getWord(word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Reading fullword from " << std::hex << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    return getWordNoLock(address);
}

doubleword cpu::getDoubleword(word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Reading doubleword from " << std::hex << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    return getDoublewordNoLock(address);
}

void cpu::writeByte(byte data, word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Writing byte " << std::hex << data << " to " << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    writeByteNoLock(data,address);
}

void cpu::writeHalfword(halfword data, word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Writing halfword " << std::hex << data << " to " << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    writeHalfwordNoLock(data,address);
}

void cpu::writeWord(word data, word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Writing fullword " << std::hex << data << " to " << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    writeWordNoLock(data,address);
}

void cpu::writeDoubleword(doubleword data, word address){
    setMode(MEMORY);
    if (verbose) {
        outputLog << "Writing doubleword " << std::hex << data << " to " << address << std::endl;
    }
    std::lock_guard<std::mutex> memlock(core->mtx);
    if (verbose) {
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    writeDoublewordNoLock(data,address);
}

void cpu::registerChannel(byte address, channel &newChannel){
    channels[address] = std::make_unique<channel>(newChannel);
}

doubleword cpu::packPSW(){
    word packpsw = (word) psw.smsk;
    packpsw <<= 4;
    packpsw += psw.key;
    packpsw <<= 1;
    packpsw += psw.ascii;
    packpsw <<= 1;
    packpsw += psw.mchk;
    packpsw <<= 1;
    packpsw += psw.wait;
    packpsw <<= 1;
    packpsw += psw.pst;
    packpsw <<= 16;
    packpsw += psw.ic;
    packpsw <<= 2;
    packpsw += psw.ilc;
    packpsw <<= 2;
    packpsw += psw.cond;
    packpsw <<= 4;
    packpsw += psw.pmsk;
    packpsw <<= 24;
    packpsw += psw.nxia;
    return packpsw;
}

/* PRIVATE */

void cpu::cycle(){
    setMode(CONTROL);
    try {
        byte opcode = getByte(psw.nxia);
        try {
            instruction currentInstruction = ISA[opcode]; 
            if (verbose) {
                outputLog << "Cycle " << absoluteCounter << std::endl;
                outputLog << "PSW: " << std::hex << packPSW() << std::endl;
                outputLog << "Fetched: " << currentInstruction.name << std::endl;
            }
            //Check for privileged instruction
            if (psw.pst == 1 && psw.pst != currentInstruction.pst){
                throw 0x02;
            }
            doubleword oldIA = psw.nxia;
            psw.nxia += (2*psw.ilc); //Increases instruction counter, can be overridden by the actual operation
            switch (psw.ilc){
                case 1:
                    (currentInstruction.run)(this,getByte(oldIA + 1),0,0);
                    break;
                case 2:
                    (currentInstruction.run)(this,getByte(oldIA + 1),getHalfword(oldIA+2),0);
                    break;
                case 3:
                    (currentInstruction.run)(this,getByte(oldIA + 1),getHalfword(oldIA+2),getHalfword(oldIA+4));
                    break;
            }
        } catch (std::out_of_range) {
            throw 0x01;
        };
        absoluteCounter++;
   } catch (int interruptionCode) {
        //ALL Interruption handling here
        setMode(INTERRUPT);
        
   }
}