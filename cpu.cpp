#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <csetjmp>
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
#define FP_OVERFLOW 0b1000 //Fixed-point overflow
#define DEC_OVERFLOW 0b0100 //Decimal overflow
#define EXP_UNDRFLOW 0b0010 //Exponent underflow
#define SIGNIFICANCE 0b0001 //Significance

//Instruction field load macros

#define LOAD_RR_FIELDS uint8_t R1 = leftNibble(b1); uint8_t R2 = rightNibble(b1);
#define LOAD_RX_FIELDS byte R1 = leftNibble(b1); byte X1 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);
#define LOAD_RS_FIELDS byte R1 = leftNibble(b1); byte R3 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);

using std::cin;
using std::cout;

using doubleword = uint64_t;
using word = uint32_t;
using halfword = uint16_t;
using byte = uint8_t;

//Most negative constexprs
constexpr doubleword MAXNEG_64 =  0b1000000000000000000000000000000000000000000000000000000000000000; //Most negative int64, expressed as an unsigned int64
constexpr word MAXNEG_32 =        0b10000000000000000000000000000000; //Most negative int32, expressed as an unsigned int32
constexpr halfword MAXNEG_16 =    0b1000000000000000; //Most negative int16, expressed as an unsigned int16
constexpr byte MAXNEG_8 =         0b10000000; //Most negative int8, expressed as an unsigned int8
//Program buffer for handling interrupts
jmp_buf env;

typedef struct ProgramStatusWordStruct {
    unsigned int smsk : 8; //System Mask, Controls interruptions. Bits 0-5 enable channels 0-5, bit 6 enables all remaining channels, and bit 7 enables external interruptions
    unsigned int key : 4; //CPU protection key. The IBM System/360 implemented an early form of ECC Memory, and used this register for it. However, this System/360 Emulator relies on the host system's memory, which almost certainly is not ECC (unless the host system happens to be an enterprise-grade server or workstation), so ECC-related functionality has been omitted. However, in the name of authenticity, this register has been left in the code. Think of it as something of an easter egg.
    unsigned int ascii : 1; //Controls whether or not the CPU will operate in ASCII mode
    unsigned int mchk : 1; //Enables machine-check interruption
    unsigned int wait : 1; //Enables wait state
    unsigned int pst : 1; //Sets problem state. When enabled, prevents the use of privileged instructions
    unsigned int ic : 16; //Interruption code
    unsigned int ilc : 2; //Instruction Length Code
    unsigned int cond : 2; //Condition code, see individual instructions for implementation
    unsigned int pmsk : 4; //Program mask
    unsigned int nxia : 24; //Next instruction address
} ProgramStatusWord;

struct Registers {
    word gen[16]; //General registers
    doubleword fp[4]; //Floating-point registers
    ProgramStatusWord psw; //Program status word
};

inline uint8_t leftNibble(byte in){return in/16;}
inline uint8_t rightNibble(byte in){return in%16;}

inline byte leftByte(halfword in){return (byte)in/(1 << 8);}
inline byte rightByte(halfword in){return (byte)in%(1 << 8);}

inline halfword leftHalfword(word in){return (halfword)in/(1<<16);}
inline halfword rightHalfword(word in){return (halfword)in%(1<<16);}

inline word leftWord(doubleword in){return (word)in/(1<<32);}
inline word rightWord(doubleword in){return (word)in%(1<<32);}

inline halfword displacement(halfword in){return in%(1<<12);}
inline uint8_t baseRegister(halfword in){return in/(1<<12);}

//Checks for overflow in addition between two integers
inline bool chkOverflow(word add1, word add2){return ((add1 & MAXNEG_32) != (add2 & MAXNEG_32)) && (((add1 + add2) & MAXNEG_32) != (add1 & MAXNEG_32));}

//Sign extension functions

inline word signex16_32(halfword in){return static_cast<word>(static_cast<int16_t>(in));}

enum InstructionSet {
    STD, //Standard instruction set architecture (ISA), includes fixed-point arithmetic
    SCI, //Scientific ISA, Superset of the Standard ISA that adds floating-point arithmetic
    COM, //Commercial ISA, Superset of the Standard ISA that adds decimal arithmetic
    UNI //Universal ISA, Superset of the Standard ISA that combines the features from both the Commercial and Scientific ISAs
};

typedef void (*op)(byte,halfword,halfword);

class cpu 
{
    public:
    cpu(int memSize, enum InstructionSet instructionSet){
        ISA = instructionSet;
        coreSize = memSize;
        core = new byte[memSize];
        usedOpLists.push_back(&stdOps);
        switch(ISA){
            case SCI:
                usedOpLists.push_back(&sciOps);
                break;
            case COM:
                usedOpLists.push_back(&comOps);
                break;
            case UNI:
                usedOpLists.push_back(&sciOps);
                usedOpLists.push_back(&comOps);
                break;
        }
    }

    private:

    enum InstructionSet ISA;
    int coreSize = 0;
    byte *core;
    std::vector<std::unordered_map<byte,op>*> usedOpLists;
    bool interruptFlag = false; //Flag that indicates whether or not the program has been interrupted

    Registers rgstrs = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0}
    };

    word packPSW(){
        word packpsw = rgstrs.psw.smsk;
        packpsw <<= 4;

    }
    void interrupt(int interruptionCode){
        //Add some extra functionality
        longjmp(env,interruptionCode);
    }

    /*Returns the byte stored at the address. WARNING: possibly memory-unsafe*/
    inline byte getByte(word address){
        if (address >= coreSize){interrupt(0x05);}
        return *(core + address);
    }
    /*Returns the halfword stored at the address. WARNING: possibly memory-unsafe*/
    inline halfword getHalfword(word address){
        if (address % 2 != 0){interrupt(0x06);}
        return (halfword)(getByte(address) << 8) + getByte(address + 1);
    }
    /*Returns the word stored at the address. WARNING: possibly memory-unsafe*/
    inline word getWord(word address){
        if (address % 4 != 0){interrupt(0x06);}
        return (word)(getHalfword(address) << 16) + getHalfword(address + 2);
    }
    /*Returns the doubleword stored at the address. WARNING: possibbly memory-unsafe*/
    inline doubleword getDoubleword(word address){
        if (address % 8 != 0){interrupt(0x06);}
        return (doubleword)(getWord(address) << 32) + getWord(address + 4);
    }

    /*Returns an address generated from X, B, and D*/
    inline word getAddr(uint8_t X, uint8_t B, uint16_t D){
        return (rgstrs.gen[X] + rgstrs.gen[B] + D)%(1<<24);
    }

    //INSTRUCTION SET

    //STD Load operations
    /*Loads R2 register into R1 register. Only b1 is used. RR*/
    void LR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        rgstrs.gen[R1] = rgstrs.gen[R2];
    }
    /*Loads word from memory into R1 register. b1 and word1 are used. RX*/
    void L(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word address = getAddr(X1,B2,D2);
        rgstrs.gen[R1] = getWord(address);
    }

    /*Loads halfword from memory into R1 register. B1 and word1 are used. RX*/
    void LH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word address = getAddr(X1,B2,D2);
        rgstrs.gen[R1] = signex16_32(getHalfword(address));
    }
    /*Loads R2 register into R1 register. The value is used to set the condition register. Only b1 is used. RR*/
    void LTR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        rgstrs.gen[R1] = rgstrs.gen[R2];
        if (rgstrs.gen[R1] == 0){
            rgstrs.psw.cond = 0;
        } else if (static_cast<int32_t>(rgstrs.gen[R1]) < 0){
            rgstrs.psw.cond = 1;
        } else {
            rgstrs.psw.cond = 2;
        }
    }

    /*Loads the two's complement of the R2 Register into the R1 Register. Only b1 is used. RR*/
    void LCR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (rgstrs.gen[R2] == MAXNEG_32 && rgstrs.psw.pmsk & FP_OVERFLOW){
            //Raises FP overflow interrupt if the most negative number is complemented
            interrupt(0x08);
        }
        rgstrs.gen[R1] = ~rgstrs.gen[R2] + 1;
    }

    /*Loads the absolute value of the R2 register into the R1 Register. Only b1 is used. RR*/
    void LPR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (rgstrs.gen[R2] == MAXNEG_32 && rgstrs.psw.pmsk & FP_OVERFLOW){
            //Raises FP overflow interrupt if the most negative number is complemented
            interrupt(0x08);
        }
        if (rgstrs.gen[R2] & MAXNEG_32){
            rgstrs.gen[R1] = ~rgstrs.gen[R2] + 1;
        } else {
            rgstrs.gen[R1] = rgstrs.gen[R2];
        }
    }
    /*Loads the two's complement of the absolute value of the R2 register into the R1 register. Only b1 is used. RR*/
    void LNR(byte b1, halfword word1 = 0, halfword word2 = 0){
        LOAD_RR_FIELDS
        if (!(rgstrs.gen[R2] & MAXNEG_32)){
            rgstrs.gen[R1] = ~rgstrs.gen[R2] + 1;
        } else {
            rgstrs.gen[R1] = rgstrs.gen[R2];
        }
    }

    /*The set of general registers starting with the register
    specified by R1 and ending with the register specified
    by R3 is loaded from the locations designated by the
    second opcrand address.
    The storage area from which the contents of the
    general registers are obtained starts at the location
    designated by the second operand address and continues through as many words as needed. The general
    registers are loaded in the ascending order of their
    addresses, starting with the register specified by R1
    and continuing up to and including the register specified by R3, with register 0 following register 15.
    RS, Only b1 and word1 are used*/
    void LM(byte b1, halfword word1, halfword word2){
        LOAD_RS_FIELDS
        word address = getAddr(0,B2,D2);
        do {
            rgstrs.gen[R1] = getWord(address);
            address += 4;
            R1 = ((R1 + 1) % 16);
        } while (R1 != ((R3 + 1) % 16));
    }

    //STD Add operations

    /*Adds R2 register to R1 register. Result is stored in R1. Only b1 is used. RR*/
    void AR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        word sum = rgstrs.gen[R1] + rgstrs.gen[R2];
        if (chkOverflow(rgstrs.gen[R1],rgstrs.gen[R2])){
            rgstrs.psw.cond = 3;
            if (rgstrs.psw.pmsk & FP_OVERFLOW){
                interrupt(0x08);
            }
        } else if (sum == 0){
            rgstrs.psw.cond = 0;
        } else if (sum > 0) {
            rgstrs.psw.cond = 2;
        } else {
            rgstrs.psw.cond = 1;
        }
        rgstrs.gen[R1] = sum;
    }

    /*Adds word from memory address to R1 register. b1 and word1 are used. RX*/
    void A(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word address = getAddr(X1,B2,D2);
        word add1 = getWord(address);
        word add2 = rgstrs.gen[R1];
        word sum = add1 + add2;
        if (chkOverflow(add1,add2)){
            rgstrs.psw.cond = 3;
            if (rgstrs.psw.pmsk & FP_OVERFLOW){interrupt(0x08);}
        } else if (sum == 0){
            rgstrs.psw.cond = 0;
        } else if (rgstrs.gen[R1] > 0) {
            rgstrs.psw.cond = 2;
        } else {
            rgstrs.psw.cond = 1;
        }
        rgstrs.gen[R1] = sum;
    }

    /*Adds halfword from memory address to R1 register. b1 and word1 are used. RX*/
    void AH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word address = getAddr(X1,B2,D2);
        word add1 = signex16_32(getHalfword(address));
        word add2 = rgstrs.gen[R1];
        word sum = add1 + add2;
        if (chkOverflow(add1,add2)){
            rgstrs.psw.cond = 3;
            if (rgstrs.psw.pmsk & FP_OVERFLOW){interrupt(0x08);}
        } else if (sum == 0){
            rgstrs.psw.cond = 0;
        } else if (rgstrs.gen[R1] > 0) {
            rgstrs.psw.cond = 2;
        } else {
            rgstrs.psw.cond = 1;
        }
        rgstrs.gen[R1] = sum;
    }

    //STD Operation List
    std::unordered_map<byte,op> stdOps;
    //SCI Operation List
    std::unordered_map<byte,op> sciOps;
    //COM Operation List
    std::unordered_map<byte,op> comOps;

    op getOperation(byte opcode){
        for (std::unordered_map<byte,op>* opList : usedOpLists){
            try {
                return opList->at(opcode);
            } catch (std::out_of_range) {
                continue;
            }
        }
        //The operation not being found is undefined behavior
        cout << "UNDEFINED BEHAVIOR ERROR: INSTRUCTION " << opcode << " IS NOT DEFINED UNDER THE CURRENT ISA" << std::endl;
    }

    /*Performs one CPU cycle*/
    void cycle(){
        int interruptionCode = setjmp(env);
        if (interruptionCode == 0){
            //Normal functioning
        } else {
            //Interruption handling here
        }
    }
};