#ifndef CPU_H
#define CPU_H

#include <memory>
#include <unordered_map>
#include "cpuhelpers.h"
#include "memory.h"
#include "channel.h"
#include <ostream>

//Sysmask channel bitmasks
#define CHANNEL0 0b10000000
#define CHANNEL1 0b01000000
#define CHANNEL2 0b00100000
#define CHANNEL3 0b00010000
#define CHANNEL4 0b00001000
#define CHANNEL5 0b00000100
#define OTHERCHN 0b00000010
#define EXTRNCHN 0b00000001
//Progmask channel bitmasks
#define FIP_OVERFLOW 0b1000 //Fixed-point overflow
#define DEC_OVERFLOW 0b0100 //Decimal overflow
#define EXP_UNDRFLOW 0b0010 //Exponent underflow
#define SIGNIFICANCE 0b0001 //Significance

struct ProgramStatusWord {
    unsigned int smsk : 8; //System Mask, Controls interruptions. Bits 0-5 enable channels 0-5, bit 6 enables all remaining channels, and bit 7 enables external interruptions
    unsigned int key : 4; //CPU protection key. This is compared with storage protection key. Setting this key to 0 gives unlimited access to memory
    unsigned int ascii : 1; //Controls whether or not the CPU will operate in ASCII mode
    unsigned int mchk : 1; //Enables machine-check interruption
    unsigned int wait : 1; //Enables wait state
    unsigned int pst : 1; //Sets problem state. When enabled, prevents the use of privileged instructions
    unsigned int ic : 16; //Interruption code
    unsigned int ilc : 2; //Instruction Length Code
    unsigned int cond : 2; //Condition code, see individual instructions for implementation
    unsigned int pmsk : 4; //Program mask
    unsigned int nxia : 24; //Next instruction address
};

struct Registers {
    word gen[16]; //General registers
    doubleword fp[4]; //Floating-point registers
};

class cpu;

class cpu {
    public:
    cpu(std::shared_ptr<memory> memptr,std::unordered_map<byte,instruction> &ISA);
    ~cpu();
    Registers rgstrs;
    std::shared_ptr<memory> core;
    /*Returns the byte stored at the address*/
    byte getByte(word address);
    /*Returns the halfword stored at the address.*/
    halfword getHalfword(word address);
    /*Returns the word stored at the address.*/
    word getWord(word address);
    /*Returns the doubleword stored at the address.*/
    doubleword getDoubleword(word address);
    /*Returns an address generated from X, B, and D*/
    word getAddr(uint8_t X, uint8_t B, uint16_t D);
    /*Writes byte to memory*/
    void writeByte(byte data, word address);
    /*Writes halfword to memory*/
    void writeHalfword(halfword data, word address);
    /*Writes fullword to memory*/
    void writeWord(word data, word address);
    /*Writes doubleword to memory*/
    void writeDoubleword(doubleword data, word address);

    void registerChannel(byte address, channel &newchannel);

    int64_t dec64ToInt(uint64_t dec);
    doubleword bin32toDec(word num);

    private:
    std::unordered_map<byte,instruction> ISA;
    std::unordered_map<byte,std::unique_ptr<channel>> channels;
    ProgramStatusWord psw;
    std::ostream cpulog;
    uint64_t absoluteCounter;
    bool verbose;
    void cycle();
    /*Reads byte from memory NO THREADLOCK*/
    byte getByteNoLock(word address);
    /*Reads halfword from memory NO THREADLOCK*/
    halfword getHalfwordNoLock(word address);
    /*Reads word from memory NO THREADLOCK*/
    word getWordNoLock(word address);
    /*Reads doubleword from memory NO THREADLOCK*/
    doubleword getDoublewordNoLock(word address);
    /*Writes byte to memory NO THREADLOCK*/
    void writeByteNoLock(byte data, word address);
    /*Writes halfword to memory NO THREADLOCK*/
    void writeHalfwordNoLock(halfword data, word address);
    /*Writes fullword to memory NO THREADLOCK*/
    void writeWordNoLock(word data, word address);
    /*Writes doubleword to memory NO THREADLOCK*/
    void writeDoublewordNoLock(doubleword data, word address);
    doubleword packPSW()

};



#endif