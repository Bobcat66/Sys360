#include "cpu.h"
#include "cpuhelpers.h"

cpu::cpu(std::shared_ptr<memory> memptr,std::unordered_map<byte,instruction> &ISA){
    core = memptr;
    this->ISA = ISA;
    psw = {0,0,0,0,0,0,0,0,0,0,0};
    rgstrs = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0}
    };
}

byte cpu::getByte(word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getByteNoLock(address);
}

halfword cpu::getHalfword(word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getHalfwordNoLock(address);
}

word cpu::getWord(word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getWordNoLock(address);
}

doubleword cpu::getDoubleword(word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getDoublewordNoLock(address);
}

void cpu::writeByte(byte data, word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeByteNoLock(data,address);
}

void cpu::writeHalfword(halfword data, word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeHalfwordNoLock(data,address);
}

void cpu::writeWord(word data, word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeWordNoLock(data,address);
}

void cpu::writeDoubleword(doubleword data, word address){
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeDoublewordNoLock(data,address);
}

/* PRIVATE */

void cpu::cycle(){
   try {
        byte opcode = getByte(psw.nxia);
        try {
            instruction currentInstruction = ISA[opcode]; 
            if (verbose) {
                cpulog << "------------------------------------------------" << std::endl;
                cpulog << "Cycle " << absoluteCounter << std::endl;
                cpulog << "PSW: " << std::hex << packPSW() << std::endl;
                cpulog << "Executing: " << currentInstruction.name << std::endl;
                cpulog << "------------------------------------------------" << std::endl;
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
   }
}