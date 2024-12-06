#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <csetjmp>
#include <cmath>
 
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

//Checks for overflow in addition between two 32 bit integers
inline bool chkOverflow(word add1, word add2){return ((add1 & MAXNEG_32) != (add2 & MAXNEG_32)) && (((add1 + add2) & MAXNEG_32) != (add1 & MAXNEG_32));}

//Checks for carry in logical addition between two 32 bit unsigned integers
inline bool chkCarry(word add1, word add2){return add1 > (INT32_MAX - add2);}

//Sign extension functions

inline word signex16_32(halfword in){return static_cast<word>(static_cast<int16_t>(in));}

enum InstructionSet {
    STD, //Standard instruction set architecture (ISA), includes fixed-point arithmetic
    SCI, //Scientific ISA, Superset of the Standard ISA that adds floating-point arithmetic
    COM, //Commercial ISA, Superset of the Standard ISA that adds decimal arithmetic
    UNI //Universal ISA, Superset of the Standard ISA that combines the features from both the Commercial and Scientific ISAs
};

typedef void (cpu::*op)(byte,halfword,halfword);

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
        generateOpLens();
        generateStdOps();
    }

    private:

    bool runAuto = false; //Flag to check whether the CPU is running in auto mode
    enum InstructionSet ISA;
    int coreSize = 0;
    byte *core;
    std::vector<std::unordered_map<byte,op>*> usedOpLists;
    bool interruptFlag = false; //Flag that indicates whether or not the program has been interrupted

    Registers rgstrs = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0}
    };

    ProgramStatusWord psw = {0,0,0,0,0,0,0,0,0,0,0};

    word packPSW(){
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

    /*Returns the byte stored at the address. WARNING: possibly memory-unsafe*/
    inline byte getByte(word address){
        if (address >= coreSize){throw 0x05;}
        return *(core + address);
    }
    /*Returns the halfword stored at the address. WARNING: possibly memory-unsafe*/
    inline halfword getHalfword(word address){
        if (address % 2 != 0){throw 0x06;}
        return (halfword)(getByte(address) << 8) + getByte(address + 1);
    }
    /*Returns the word stored at the address. WARNING: possibly memory-unsafe*/
    inline word getWord(word address){
        if (address % 4 != 0){throw 0x06;}
        return (word)(getHalfword(address) << 16) + getHalfword(address + 2);
    }
    /*Returns the doubleword stored at the address. WARNING: possibly memory-unsafe*/
    inline doubleword getDoubleword(word address){
        if (address % 8 != 0){throw 0x06;}
        return (doubleword)(getWord(address) << 32) + getWord(address + 4);
    }

    /*Returns an address generated from X, B, and D*/
    inline word getAddr(uint8_t X, uint8_t B, uint16_t D){
        return (rgstrs.gen[X] + rgstrs.gen[B] + D) % (1<<24);
    }

    /*Writes byte to memory*/
    inline void writeByte(byte data, word address){
        if (address >= coreSize){throw 0x05;}
        *(core + address) = data;
    }

    /*Writes halfword to memory*/
    inline void writeHalfword(halfword data, word address){
        if (address % 2 != 0){throw 0x06;}
        writeByte(leftByte(data),address);
        writeByte(rightByte(data),address+1);
    }

    /*Writes fullword to memory*/
    inline void writeWord(word data, word address){
        if (address % 4 != 0){throw 0x06;}
        writeHalfword(leftHalfword(data),address);
        writeHalfword(rightHalfword(data),address + 2);
    }

    /*Writes doubleword to memory*/
    inline void writeDoubleword(doubleword data, word address){
        if (address % 8 != 0){throw 0x06;}
        writeWord(leftWord(data),address);
        writeWord(rightWord(data),address + 4);
    }

    /*Converts a 64-bit packed decimal into a signed integer*/
    int64_t dec64ToInt(uint64_t dec){
        char sign = dec % (1 << 4);
        dec /= (1 << 4);
        char digits[15];
        uint64_t outint = 0;
        for (int i = 0; i < 15; i++){
            digits[i] = dec % (1 << 4);
            if (digits[i] > 9){throw 0x07;} //Raise Data exception if invalid digit
            dec /= (1 << 4);
        }
        for (int i = 0; i < 15; i++){
            outint += digits[i] * std::pow(10,i);
        }
        switch (sign){
            case 0b1010:
            case 0b1100:
            case 0b1110:
            case 0b1111:
                break;
            case 0b1011:
            case 0b1101:
                outint *= -1;
                break;
            default:
                //Raise data exception if invalid digit
                throw 0x07;
        }
        return outint;
    }

    doubleword bin32toDec(word num){
        char numSign;
        if (psw.ascii) {
            numSign = num & MAXNEG_32 ? 0b1010 : 0b1011;
        } else {
            numSign = num & MAXNEG_32 ? 0b1100 : 0b1101;
        }
        word mag = num & MAXNEG_32 ? ~num + 1 : num;
        doubleword out = 0;
        while (mag > 0){
            out <<= 4;
            int magmod = (int)pow(10,(int)log10(mag));
            out += (mag / magmod);
            mag %= magmod;
        }
        out <<= 4;
        out += numSign;
        return out;
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
            psw.cond = 0;
        } else if (static_cast<int32_t>(rgstrs.gen[R1]) < 0){
            psw.cond = 1;
        } else {
            psw.cond = 2;
        }
    }

    /*Loads the two's complement of the R2 Register into the R1 Register. Only b1 is used. RR*/
    void LCR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (rgstrs.gen[R2] == MAXNEG_32 && psw.pmsk & FP_OVERFLOW){
            //Raises FP overflow interrupt if the most negative number is complemented
            throw 0x08;
        }
        rgstrs.gen[R1] = ~rgstrs.gen[R2] + 1;
    }

    /*Loads the absolute value of the R2 register into the R1 Register. Only b1 is used. RR*/
    void LPR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (rgstrs.gen[R2] == MAXNEG_32 && psw.pmsk & FP_OVERFLOW){
            //Raises FP overflow interrupt if the most negative number is complemented
            throw 0x08;
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
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){
                throw 0x08;
            }
        } else if (sum == 0){
            psw.cond = 0;
        } else if (sum & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
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
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){throw 0x08;}
        } else if (sum == 0){
            psw.cond = 0;
        } else if (sum & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
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
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){throw 0x08;}
        } else if (sum == 0){
            psw.cond = 0;
        } else if (sum & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
        }
        rgstrs.gen[R1] = sum;
    }

    /*Performs Logical add between R1 and R2, stores result in R1. Due to the 2's complement integer representation, Logical add is identical mathematically to arithmetic add, Logical Add does not recognize overflow, and only tracks if the result is zero or nonzero. Only uses b1. RR*/
    void ALR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        word add1 = rgstrs.gen[R1];
        word add2 = rgstrs.gen[R2];
        int carry = 0;
        word sum = add1 + add2;
        if (chkCarry(add1,add2)){
            carry++;
        }
        rgstrs.gen[R1] = sum;
        psw.cond = (carry << 1) + (sum == 0 ? 0 : 1);
    }

    /*Performs Logical add between R1 and a Word loaded from the address. Result is stored in R1. Uses b1 and word1. RX*/
    void AL(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word add1 = rgstrs.gen[R1];
        word add2 = getWord(getAddr(X1,B2,D2));
        int carry = 0;
        word sum = add1 + add2;
        if (chkCarry(add1,add2)){
            carry++;
        }
        rgstrs.gen[R1] = sum;
        psw.cond = (carry << 1) + (sum == 0 ? 0 : 1);
    }

    //STD Subtract operations

    /*Subtracts R2 register from R1 register. Result is stored in R1. Only b1 is used. RR*/
    void SR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        word min = rgstrs.gen[R1];
        word sub = ~rgstrs.gen[R2] + 1;
        word diff = min + sub;
        if (chkOverflow(min,sub)){
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){
                throw 0x08;
            }
        } else if (diff == 0){
            psw.cond = 0;
        } else if (diff & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
        }
        rgstrs.gen[R1] = diff;
    }

    /*Subtracts word from memory address from R1 register. b1 and word1 are used. RX*/
    void S(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word min = rgstrs.gen[R1];
        word sub = ~getWord(getAddr(X1,B2,D2)) + 1;
        word diff = min + sub;
        if (chkOverflow(min,sub)){
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){throw 0x08;}
        } else if (diff == 0){
            psw.cond = 0;
        } else if (diff & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
        }
        rgstrs.gen[R1] = diff;
    }

    /*Adds halfword from memory address to R1 register. b1 and word1 are used. RX*/
    void SH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word min = rgstrs.gen[R1];
        word sub = ~signex16_32(getHalfword(getAddr(X1,B2,D2))) + 1;
        word diff = min + sub;
        if (chkOverflow(min,sub)){
            psw.cond = 3;
            if (psw.pmsk & FP_OVERFLOW){throw 0x08;}
        } else if (diff == 0){
            psw.cond = 0;
        } else if (diff & MAXNEG_32) {
            psw.cond = 2;
        } else {
            psw.cond = 1;
        }
        rgstrs.gen[R1] = diff;
    }

    /*Performs Logical subtract between R1 and R2, stores result in R1. Only uses b1. RR*/
    void SLR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        word min = rgstrs.gen[R1];
        word sub = ~rgstrs.gen[R2] + 1;
        int carry = 0;
        word diff = min + sub;
        if (chkCarry(min,sub)){
            carry++;
        }
        rgstrs.gen[R1] = diff;
        psw.cond = (carry < 1) + (diff == 0 ? 0 : 1);
    }

    /*Performs Logical subtract between R1 and a Word loaded from the address. Result is stored in R1. Uses b1 and word1. RX*/
    void SL(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        word min = rgstrs.gen[R1];
        word sub = ~getWord(getAddr(X1,B2,D2)) + 1;
        int carry = 0;
        word diff = min + sub;
        if (chkCarry(min,sub)){
            carry++;
        }
        rgstrs.gen[R1] = diff;
        psw.cond = (carry < 1) + (diff == 0 ? 0 : 1);
    }

    // STD Multiplication ops
    
    /*Multiplies the first operand (multiplicand) by the second operand (multiplier), and stores the result in the location of the first operand. 
    NOTE: Both multiplier and multiplicand are 32 bit integers, while the product is a 64 bit integer. The product is
    stored in an even/odd register pair, and the multiplicand is retrieved from the odd register. R1 must refer to an even register, or a specification
    interrupt will occur. Only uses b1. RR*/
    void MR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (R1 % 2 != 0){throw 0x0E;}
        word multiplicand = rgstrs.gen[(R1 + 1) % 16];
        word multiplier = rgstrs.gen[R2];
        doubleword prod = (doubleword)multiplicand * multiplier;
        rgstrs.gen[R1] = prod / (1 << 32);
        rgstrs.gen[(R1 + 1) % 16] = prod % (1 << 32);
    }

    /*Multiplies the first operand (multiplicand) by the second operand (multiplier), and stores the result in the location of the first operand. 
    NOTE: Both multiplier and multiplicand are 32 bit integers, while the product is a 64 bit integer. The product is
    stored in an even/odd register pair, and the multiplicand is retrieved from the odd register. R1 must refer to an even register, or a specification
    interrupt will occur. Uses b1 and word1. RX*/
    void M(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        if (R1 % 2 != 0){throw 0x0E;}
        word multiplicand = rgstrs.gen[(R1 + 1) % 16];
        word multiplier = getWord(getAddr(X1,B2,D2));
        doubleword prod = (doubleword)multiplicand * multiplier;
        rgstrs.gen[R1] = prod / (1 << 32);
        rgstrs.gen[(R1 + 1) % 16] = prod % (1 << 32);
    }

    /*Multiplies halfword from memory with R1. Same rules apply for storing the product as other multiplication commands*/
    void MH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        if (R1 % 2 != 0){throw 0x0E;}
        word multiplicand = rgstrs.gen[(R1 + 1) % 16];
        word multiplier = signex16_32(getHalfword(getAddr(X1,B2,D2)));
        doubleword prod = (doubleword)multiplicand * multiplier;
        rgstrs.gen[R1] = prod / (1 << 32);
        rgstrs.gen[(R1 + 1) % 16] = prod % (1 << 32);
    }

    // STD comparator ops

    /*R1 is compared to R2, and the result is stored in the cond register. RR*/
    void CR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        int32_t signedOprnd1 = (int32_t) rgstrs.gen[R1];
        int32_t signedOprnd2 = (int32_t) rgstrs.gen[R2];
        if (signedOprnd1 == signedOprnd2) {
            psw.cond = 0;
        } else if (signedOprnd1 < signedOprnd2) {
            psw.cond = 1;
        } else {
            psw.cond = 2;
        }
    }

    /*R1 is compared to word loaded from memory, and the result is stored in the cond register. RX*/
    void C(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        int32_t signedOprnd1 = (int32_t) rgstrs.gen[R1];
        int32_t signedOprnd2 = (int32_t) getWord(getAddr(X1,B2,D2));
        if (signedOprnd1 == signedOprnd2) {
            psw.cond = 0;
        } else if (signedOprnd1 < signedOprnd2) {
            psw.cond = 1;
        } else {
            psw.cond = 2;
        }
    }

    /*R1 is compared to halfword loaded from memory, and the result is stored in the cond register. RX*/
    void CH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        int32_t signedOprnd1 = (int32_t) rgstrs.gen[R1];
        int32_t signedOprnd2 = (int32_t) signex16_32(getHalfword(getAddr(X1,B2,D2)));
        if (signedOprnd1 == signedOprnd2) {
            psw.cond = 0;
        } else if (signedOprnd1 < signedOprnd2) {
            psw.cond = 1;
        } else {
            psw.cond = 2;
        }
    }

    /*divides first operand by second operand. the first operand is stored in an even/odd register pair. The second operand is R2. the first operand is replaced by the
    quotient and remainder. RR*/
    void DR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        if (R1 % 2 != 0){throw 0x0E;} //Specification error
        doubleword dividend = (doubleword) (rgstrs.gen[R1] << 32) + rgstrs.gen[(R1 + 1) % 16];
        word divisor = rgstrs.gen[R2];
        if (divisor == 0 || dividend / divisor > 0xFFFFFFFF){throw 0x09;} //Floating point divide error
        rgstrs.gen[R1] = (word)(dividend/divisor);
        rgstrs.gen[(R1 + 1) % 16] = (word)(dividend % divisor);
    }

    /*divides first operand by second operand. the first operand is stored in an even/odd register pair. The second operand is retrieved from memory. the first operand is replaced by the
    quotient and remainder. RX*/
    void D(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        if (R1 % 2 != 0){throw 0x0E;} //Specification error
        doubleword dividend = (doubleword) (rgstrs.gen[R1] << 32) + rgstrs.gen[(R1 + 1) % 16];
        word divisor = getWord(getAddr(X1,B2,D2));
        if (divisor == 0 || dividend / divisor > 0xFFFFFFFF){throw 0x09;} //Floating point divide error
        rgstrs.gen[R1] = (word)(dividend/divisor);
        rgstrs.gen[(R1 + 1) % 16] = (word)(dividend % divisor);
    }

    /*Converts packed decimal number retrieved from a doubleword in memory to binary, and stores the result in R1. RX*/
    void CVB(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        doubleword decint = (doubleword) dec64ToInt(getDoubleword(getAddr(X1,B2,D2)));
        rgstrs.gen[R1] = static_cast<word>(decint);
        if (decint > 0xFFFFFFFF) {
            //Raises fixed point divide exception if the resulting decimal digit cannot be stored in 32 bits
            throw 0x09;
        }
    }

    /*Converts R1 to packed decimal format, which is stored as a doubleword at the memory address. RX*/
    void CVD(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        doubleword dec = bin32toDec(rgstrs.gen[R1]);
        writeDoubleword(dec,getAddr(X1,B2,D2));
    }

    // STD store operations

    /*R1 is stored into memory at the address. RX*/
    void ST(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        writeWord(rgstrs.gen[R1],getAddr(X1,B2,D2));
    }

    /*low order 16 bits of R1 are stored into memory at the address. RX*/
    void STH(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        writeHalfword((halfword)rgstrs.gen[R1],getAddr(X1,B2,D2));
    }

    /*Writes multiple registers, beginning with R1 and ending with R3, to memory*/
    void STM(byte b1, halfword word1, halfword word2){
        LOAD_RS_FIELDS
        word address = getAddr(0,B2,D2);
        do {
            writeWord(rgstrs.gen[R1],address);
            address += 4;
            R1 = ((R1 + 1) % 16);
        } while (R1 != ((R3 + 1) % 16));
    }

    //Shifting

    //Logic

    //Branching
    /*R1 is used as a 4 bit mask, and is bitwise ANDed to 2 raised to the power of the PSW condition code. If the AND is true, the PSW NXIA code is switched to the address stored in register R2. RR*/
    void BCR(byte b1, halfword word1, halfword word2){
        LOAD_RR_FIELDS
        byte comp = (byte)pow(2,psw.cond);
        if (R1 & comp){
            psw.nxia = rgstrs.gen[R2];
        }
    }

    /*R1 is used as a 4 bit mask, and is bitwise ANDed to 2 raised to the power of the PSW condition code. If the AND is true, the PSW NXIA code is switched to the address generated from X1, B2, and D2. RX*/
    void BCX(byte b1, halfword word1, halfword word2){
        LOAD_RX_FIELDS
        byte comp = (byte)pow(2,psw.cond);
        if(R1 & comp){
            psw.nxia = getAddr(X1,B2,D2);
        }
    }

    //STD Operation List
    std::unordered_map<byte,op> stdOps;
    
    void generateStdOps(){
        stdOps[0x1A] = &AR;
        stdOps[0x5A] = &A;
        stdOps[0x4A] = &AH;
        stdOps[0x1A] = &AR;
        stdOps[0x1E] = &ALR;
        stdOps[0x5E] = &AL;
        stdOps[0x07] = &BCR;
        stdOps[0x1D] = &DR;
        stdOps[0x5D] = &D;
        stdOps[0x18] = &LR;
        stdOps[0x58] = &L;
        stdOps[0x13] = &LCR;
        stdOps[0x12] = &LTR;
        stdOps[0x48] = &LH;
        stdOps[0x98] = &LM;
        stdOps[0x11] = &LNR;
        stdOps[0x10] = &LPR;
        stdOps[0x50] = &ST;
        stdOps[0x40] = &STH;
        stdOps[0x90] = &STM;
        stdOps[0x1B] = &SR;
        stdOps[0x5B] = &S;
        stdOps[0x4B] = &SH;
        stdOps[0x1F] = &SLR;
        stdOps[0x5F] = &SL;
        stdOps[0x4F] = &CVB;
        stdOps[0x4E] = &CVD;
    }
    //SCI Operation List
    std::unordered_map<byte,op> sciOps;
    //COM Operation List
    std::unordered_map<byte,op> comOps;

    //Operation lengths (in halfwords)
    std::unordered_map<byte,int> opLens;
    
    void generateOpLens(){
        opLens[0x1A] = 1;
        opLens[0x5A] = 2;
        opLens[0x4A] = 2;
        opLens[0x1A] = 1;
        opLens[0x1E] = 1;
        opLens[0x5E] = 2;
        opLens[0x07] = 1;
        opLens[0x1D] = 1;
        opLens[0x5D] = 2;
        opLens[0x18] = 1;
        opLens[0x58] = 2;
        opLens[0x13] = 1;
        opLens[0x12] = 1;
        opLens[0x48] = 2;
        opLens[0x98] = 2;
        opLens[0x11] = 1;
        opLens[0x10] = 1;
        opLens[0x50] = 2;
        opLens[0x40] = 2;
        opLens[0x90] = 2;
        opLens[0x1B] = 1;
        opLens[0x5B] = 2;
        opLens[0x4B] = 2;
        opLens[0x1F] = 1;
        opLens[0x5F] = 2;
        opLens[0x4F] = 2;
        opLens[0x4E] = 2;
    }

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
    void cycle(word instructionAddress){
        try {
            byte opcode = getByte(instructionAddress);
            op operation = getOperation(opcode);
            psw.nxia += 1; //Increases instruction counter, can be overridden by the actual operation
            psw.ilc = opLens.at(opcode);
            switch (psw.ilc){
                case 1:
                    operation(getByte(instructionAddress + 1),0,0);
                    break;
                case 2:
                    operation(getByte(instructionAddress + 1),getHalfword(instructionAddress+2));
                    break;
                case 3:
                    operation(getByte(instructionAddress + 1),getHalfword(instructionAddress+2),getHalfword(instructionAddress+4));
                    break;
            }
        } catch (int interruptCode) {
            //Interruption handling here
        }
    }

    /*Begins CPU in auto mode*/
    void startAuto(word initialAddress){
        psw.nxia = initialAddress;
        runAuto = true;
        while (runAuto) {
            cycle((word)psw.nxia);
        }
    }
};