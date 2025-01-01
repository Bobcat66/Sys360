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
    clockUnit.start();
    absoluteCounter = 0;
}

cpu::~cpu(){
    //std::cout << "\nDESTRUCTING CPU\n";
    clockUnit.stop();
}

byte cpu::getByte(word address){
    return core->getByte(address,psw.key);
}

void cpu::setVerbose(bool enabled){
    this->verbose = enabled;
}

void cpu::test(int cycles){
    std::cout << "---------START---------" << std::endl;
    for (int i = 0; i < cycles; i++) {
        cycle();
        //std::cout << "ABCYCLE " << absoluteCounter << ": " << getWord(72) << std::endl;
    }
    std::cout << "\n----------END----------\n";
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
            case INTERRUPT:
                outputLog << "------------INTERRUPTION------------" << std::endl;
                break;
        }
        outputLog << std::dec << clockUnit.gettime() << " MICROSECONDS" << std::endl;
    }
}

halfword cpu::getHalfword(word address){
    return core->getHalfword(address,psw.key);
}

word cpu::getWord(word address){
    return core->getWord(address,psw.key);
}

doubleword cpu::getDoubleword(word address){
    return core->getDoubleword(address,psw.key);
}

void cpu::writeByte(byte data, word address){
    if (verbose) {outputLog << "Writing byte " << (int)data << " at " << address << std::endl;}
    core->writeByte(address,data,psw.key);
}

void cpu::writeHalfword(halfword data, word address){
    if (verbose) {outputLog << "Writing halfword " << data << " at " << address << std::endl;}
    core->writeHalfword(address,data,psw.key);
}

void cpu::writeWord(word data, word address){
    if (verbose) {outputLog << "Writing word " << data << " at " << address << std::endl;}
    core->writeWord(address,data,psw.key);
}

void cpu::writeDoubleword(doubleword data, word address){
    if (verbose) {outputLog << "Writing doubleword " << (int)data << " at " << address << std::endl;}
    core->writeDoubleword(address,data,psw.key);
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
    if (verbose) {
        outputLog << "Starting IO on device " << (int)channel << " " << (int)subchannel << " " << (int)device << std::endl;
    }
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
                for (int i = 0; i < 16; i++) {
                    outputLog << "REGISTER " << i << ": " << rgstrs.gen[i] << std::endl;
                }
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