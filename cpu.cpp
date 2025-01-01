#include "cpu.h"
#include "cpuhelpers.h"
#include <iostream>

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
        outputLog << "Memory Lock Acquired at " << std::dec << clockUnit.gettime() << std::endl;
    }
    return core->getByteNoSync(address,psw.key);
}

void cpu::setVerbose(bool enabled){
    this->verbose = enabled;
}

void cpu::test(int cycles){
    std::cout << "TESTING" << std::endl;
    for (int i = 0; i < cycles; i++) {
        cycle();
    }
}

void cpu::setMode(enum CPUMode newMode){
    mode = newMode;
    if (verbose) {
        switch (mode)  {
            case CONTROL:
                outputLog << "------------CONTROL UNIT------------" << std::endl;
                break;
            case EXECUTION:
                outputLog << "------------EXECUTE UNIT------------" << std::endl;
                break;
            case MEMORY:
                outputLog << "------------MEMMGMT UNIT------------" << std::endl;
                break;
            case INTERRUPT:
                outputLog << "------------INTERRUPTION------------" << std::endl;
                break;
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
    return core->getHalfwordNoSync(address,psw.key);
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
    return core->getWordNoSync(address,psw.key);
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
    return core->getDoublewordNoSync(address,psw.key);
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
    core->writeByteNoSync(data,address,psw.key);
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
    core->writeHalfwordNoSync(data,address,psw.key);
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
    core->writeWordNoSync(data,address,psw.key);
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
    core->writeDoublewordNoSync(data,address,psw.key);
}

void cpu::registerChannel(byte address, channel &newChannel){
    channels[address] = std::make_unique<channel>(newChannel);
}

doubleword cpu::packPSW(){
    doubleword packpsw = (doubleword) psw.smsk;
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

void cpu::setAddr(word address) {
    psw.nxia = address;
}

int cpu::startIO(byte channel, byte subchannel, byte device) {
    return channels[channel]->startIO({subchannel,device});
}

int cpu::haltIO(byte channel, byte subchannel, byte device) {
    return channels[channel]->haltIO({subchannel,device});
}

byte cpu::getPmsk() {
    return psw.pmsk;
}

byte cpu::getCond() {
    return psw.cond;
}

word cpu::getAddr(uint8_t X, uint8_t B, uint16_t D){
    return (rgstrs.gen[X] + rgstrs.gen[B] + D) % (1<<24);
}

/* PRIVATE */

void cpu::cycle(){
    try {
        byte opcode = getByte(psw.nxia);
        setMode(CONTROL);
        try {
            instruction currentInstruction = ISA[opcode]; 
            if (verbose) {
                outputLog << "Cycle " << absoluteCounter << std::endl;
                outputLog << "PSW: " << std::hex << packPSW() << std::endl;
                outputLog << "Fetched: " << currentInstruction.name << "(" << std::hex << (int)opcode << ")" << std::endl;
            }
            //Check for privileged instruction
            if (psw.pst == 1 && psw.pst != currentInstruction.pst){
                throw 0x02;
            }
            doubleword oldIA = psw.nxia;
            psw.ilc = currentInstruction.ilc;
            psw.nxia += (2*psw.ilc); //Increases instruction counter, can be overridden by the actual operation
            std::optional<int> retCode;
            byte b1 = getByte(oldIA + 1);
            halfword word1 = getHalfword(oldIA+2);
            halfword word2 = getHalfword(oldIA+4);
            setMode(EXECUTION);
            switch (psw.ilc){
                case 1:
                    retCode = (currentInstruction.run)(this,b1,0,0);
                    break;
                case 2:
                    retCode = (currentInstruction.run)(this,b1,word1,0);
                    break;
                case 3:
                    retCode = (currentInstruction.run)(this,b1,word1,word2);
                    break;
            }
            setMode(CONTROL);
            if (retCode.has_value()) {
                psw.cond = retCode.value();
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