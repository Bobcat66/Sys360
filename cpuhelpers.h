#ifndef CPUHELPERS_H
#define CPUHELPERS_H

#include "helpers.h"
#include <bit>
#include <optional>
#include <string>

/*-----------------------------------------------------------------*/
/* INSTRUCTION FIELD MACROS                                        */
/*-----------------------------------------------------------------*/

#define LOAD_RR_FIELDS byte R1 = leftNibble(b1); byte R2 = rightNibble(b1);
#define LOAD_RX_FIELDS byte R1 = leftNibble(b1); byte X1 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);
#define LOAD_RS_FIELDS byte R1 = leftNibble(b1); byte R3 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);
#define LOAD_SI_FIELDS byte I2 = b1; byte B1 = baseRegister(word1); byte D1 = baseRegister(word1);

/*-----------------------------------------------------------------*/
/* SYSMASK BITMASKS                                                */
/*-----------------------------------------------------------------*/

#define CHANNEL0 0b10000000
#define CHANNEL1 0b01000000
#define CHANNEL2 0b00100000
#define CHANNEL3 0b00010000
#define CHANNEL4 0b00001000
#define CHANNEL5 0b00000100
#define OTHERCHN 0b00000010
#define EXTRNCHN 0b00000001

/*-----------------------------------------------------------------*/
/* PROGMASK BITMASKS                                               */
/*-----------------------------------------------------------------*/

#define FIP_OVERFLOW 0b1000 //Fixed-point overflow
#define DEC_OVERFLOW 0b0100 //Decimal overflow
#define EX_UNDERFLOW 0b0010 //Exponent underflow
#define SIGNIFICANCE 0b0001 //Significance

/*-----------------------------------------------------------------*/
/* INTERRUPTION CODES                                              */
/*-----------------------------------------------------------------*/

#define OPRN_ITRP 0x0001    //OPERATION
#define PROP_ITRP 0x0002    //PRIVILEGED OPERATION
#define EXEC_ITRP 0x0003    //EXECUTE
#define PRTC_ITRP 0x0004    //PROTECTION
#define ADDR_ITRP 0x0005    //ADDRESSING
#define SPEC_ITRP 0x0006    //SPECIFICATION
#define DATA_ITRP 0x0007    //DATA    
#define FIOF_ITRP 0x0008    //FIXED-POINT OVERFLOW
#define FIDV_ITRP 0x0009    //FIXED-POINT DIVISION
#define DCOF_ITRP 0x000A    //DECIMAL OVERFLOW
#define DCDV_ITRP 0x000B    //DECIMAL DIVISION
#define EXOF_ITRP 0x000C    //EXPONENT OVERFLOW
#define EXUF_ITRP 0x000D    //EXPONENT UNDERFLOW
#define SIGN_ITRP 0x000E    //SIGNIFICANCE
#define FLDV_ITRP 0x000F    //FLOATING-POINT DIVISION

/*-----------------------------------------------------------------*/
/* ADDRESSING HELPERS                                              */
/*-----------------------------------------------------------------*/

inline halfword displacement(halfword in){return in%(1<<12);}
inline uint8_t baseRegister(halfword in){return in/(1<<12);}

/*-----------------------------------------------------------------*/
/* ARITHMETIC HELPERS                                              */
/*-----------------------------------------------------------------*/

//Checks for overflow in addition between two 32 bit integers
inline bool chkOverflow(word add1, word add2){return ((add1 & MAXNEG_32) != (add2 & MAXNEG_32)) && (((add1 + add2) & MAXNEG_32) != (add1 & MAXNEG_32));}
//Checks for carry in logical addition between two 32 bit unsigned integers
inline bool chkCarry(word add1, word add2){return add1 > (INT32_MAX - add2);}

inline word signex16_32(halfword in){return std::bit_cast<word>(static_cast<int32_t>(static_cast<int16_t>(in)));} //Ignore the error, Intellisense is being stupid. The code compiles as long as you use C++ 20

/*-----------------------------------------------------------------*/
/* STRUCTS                                                         */
/*-----------------------------------------------------------------*/

class cpu;

struct instruction {
    unsigned int pst : 1; //0 = privileged instruction, 1 = unprivileged instruction
    unsigned int ilc : 2;
    std::string name;
    std::optional<int> (*run)(cpu *cpuptr,byte b1,halfword word1,halfword word2);
};

#endif