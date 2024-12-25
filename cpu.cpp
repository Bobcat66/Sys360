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
    std::lock_guard<std::mutex> memlock(core->mtx);
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
        outputLog << clockUnit.gettime() << " MICROSECONDS" << std::endl;
    }
}

halfword cpu::getHalfword(word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getHalfwordNoLock(address);
}

word cpu::getWord(word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getWordNoLock(address);
}

doubleword cpu::getDoubleword(word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    return getDoublewordNoLock(address);
}

void cpu::writeByte(byte data, word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeByteNoLock(data,address);
}

void cpu::writeHalfword(halfword data, word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeHalfwordNoLock(data,address);
}

void cpu::writeWord(word data, word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeWordNoLock(data,address);
}

void cpu::writeDoubleword(doubleword data, word address){
    setMode(MEMORY);
    std::lock_guard<std::mutex> memlock(core->mtx);
    writeDoublewordNoLock(data,address);
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