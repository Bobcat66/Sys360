#include "stdisa.h"
#include <cmath>

/*#################################################################*/
/*#                                                               #*/
/*#-------------------# FIXED-POINT ARITHMETIC #------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> ADD_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    word sum = cpu->rgstrs.gen[R1] + cpu->rgstrs.gen[R2];
    if (chkOverflow(cpu->rgstrs.gen[R1],cpu->rgstrs.gen[R2])){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){
            throw 0x08;
        }
    } else if (sum == 0){
        ret = 0;
    } else if (sum & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = sum;
    return ret;
}

std::optional<int> ADD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word address = cpu->getAddr(X1,B2,D2);
    word add1 = cpu->getWord(address);
    word add2 = cpu->rgstrs.gen[R1];
    word sum = add1 + add2;
    if (chkOverflow(add1,add2)){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){throw 0x08;}
    } else if (sum == 0){
        ret = 0;
    } else if (sum & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = sum;
    return ret;
}

std::optional<int> ADD_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word address = cpu->getAddr(X1,B2,D2);
    word add1 = signex16_32(cpu->getHalfword(address));
    word add2 = cpu->rgstrs.gen[R1];
    word sum = add1 + add2;
    if (chkOverflow(add1,add2)){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){throw 0x08;}
    } else if (sum == 0){
        ret = 0;
    } else if (sum & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = sum;
    return ret;
}

std::optional<int> ADD_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    word add1 = cpu->rgstrs.gen[R1];
    word add2 = cpu->rgstrs.gen[R2];
    int carry = 0;
    word sum = add1 + add2;
    if (chkCarry(add1,add2)){
        carry++;
    }
    cpu->rgstrs.gen[R1] = sum;
    ret = (carry << 1) + (sum == 0 ? 0 : 1);
    return ret;
}

std::optional<int> ADD_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word add1 = cpu->rgstrs.gen[R1];
    word add2 = cpu->getWord(cpu->getAddr(X1,B2,D2));
    int carry = 0;
    word sum = add1 + add2;
    if (chkCarry(add1,add2)){
        carry++;
    }
    cpu->rgstrs.gen[R1] = sum;
    ret = (carry << 1) + (sum == 0 ? 0 : 1);
    return ret;
}

std::optional<int> SUBTRACT_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    word min = cpu->rgstrs.gen[R1];
    word sub = ~(cpu->rgstrs.gen[R2]) + 1;
    word diff = min + sub;
    if (chkOverflow(min,sub)){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){
            throw 0x08;
        }
    } else if (diff == 0){
        ret = 0;
    } else if (diff & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = diff;
    return ret;
}

std::optional<int> SUBTRACT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word min = cpu->rgstrs.gen[R1];
    word sub = ~(cpu->getWord(cpu->getAddr(X1,B2,D2))) + 1;
    word diff = min + sub;
    if (chkOverflow(min,sub)){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){throw 0x08;}
    } else if (diff == 0){
        ret = 0;
    } else if (diff & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = diff;    
    return ret;
}

std::optional<int> SUBTRACT_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word min = cpu->rgstrs.gen[R1];
    word sub = ~signex16_32((cpu->getWord(cpu->getAddr(X1,B2,D2)))) + 1;
    word diff = min + sub;
    if (chkOverflow(min,sub)){
        ret = 3;
        if (cpu->getPmsk() & FIP_OVERFLOW){throw 0x08;}
    } else if (diff == 0){
        ret = 0;
    } else if (diff & MAXNEG_32) {
        ret = 2;
    } else {
        ret = 1;
    }
    cpu->rgstrs.gen[R1] = diff;    
    return ret;
}

std::optional<int> SUBTRACT_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    word min = cpu->rgstrs.gen[R1];
    word sub = ~(cpu->rgstrs.gen[R2]) + 1;
    int carry = 0;
    word diff = min + sub;
    if (chkCarry(min,sub)){
        carry++;
    }
    cpu->rgstrs.gen[R1] = diff;
    ret = (carry << 1) + (diff == 0 ? 0 : 1);
    return ret;
}

std::optional<int> SUBTRACT_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    word min = cpu->rgstrs.gen[R1];
    word sub = ~(cpu->getWord(cpu->getAddr(X1,B2,D2))) + 1;
    int carry = 0;
    word diff = min + sub;
    if (chkCarry(min,sub)){
        carry++;
    }
    cpu->rgstrs.gen[R1] = diff;
    ret = (carry < 1) + (diff == 0 ? 0 : 1);
    return ret;
}

std::optional<int> MULTIPLY_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    if (R1 % 2 != 0){throw 0x0E;}
    word multiplicand = cpu->rgstrs.gen[(R1 + 1) % 16];
    word multiplier = cpu->rgstrs.gen[R2];
    doubleword prod = (doubleword)multiplicand * multiplier;
    cpu->rgstrs.gen[R1] = prod / LSHFT32;
    cpu->rgstrs.gen[(R1 + 1) % 16] = prod % LSHFT32;
    return ret;
}

std::optional<int> MULTIPLY(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    if (R1 % 2 != 0){throw 0x0E;}
    word multiplicand = cpu->rgstrs.gen[(R1 + 1) % 16];
    word multiplier = cpu->getWord(cpu->getAddr(X1,B2,D2));
    doubleword prod = (doubleword)multiplicand * multiplier;
    cpu->rgstrs.gen[R1] = prod / LSHFT32;
    cpu->rgstrs.gen[(R1 + 1) % 16] = prod % LSHFT32;
    return ret;
}

std::optional<int> MULTIPLY_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    if (R1 % 2 != 0){throw 0x0E;}
    word multiplicand = cpu->rgstrs.gen[(R1 + 1) % 16];
    word multiplier = signex16_32(cpu->getHalfword(cpu->getAddr(X1,B2,D2)));
    doubleword prod = (doubleword)multiplicand * multiplier;
    cpu->rgstrs.gen[R1] = prod / LSHFT32;
    cpu->rgstrs.gen[(R1 + 1) % 16] = prod % LSHFT32;
    return ret;
}

std::optional<int> DIVIDE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    if (R1 % 2 != 0){throw 0x0E;} //Specification error
    doubleword dividend = ((doubleword)(cpu->rgstrs.gen[R1]) << 32) + cpu->rgstrs.gen[(R1 + 1) % 16];
    word divisor = cpu->rgstrs.gen[R2];
    if (divisor == 0 || dividend / divisor > 0xFFFFFFFF){throw 0x09;} //Floating point divide error
    cpu->rgstrs.gen[R1] = (word)(dividend/divisor);
    cpu->rgstrs.gen[(R1 + 1) % 16] = (word)(dividend % divisor);
    return ret;
}

std::optional<int> DIVIDE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    if (R1 % 2 != 0){throw 0x0E;} //Specification error
    doubleword dividend = ((doubleword)(cpu->rgstrs.gen[R1]) << 32) + cpu->rgstrs.gen[(R1 + 1) % 16];
    word divisor = cpu->getWord(cpu->getAddr(X1,B2,D2));
    if (divisor == 0 || dividend / divisor > 0xFFFFFFFF){throw 0x09;} //Floating point divide error
    cpu->rgstrs.gen[R1] = (word)(dividend/divisor);
    cpu->rgstrs.gen[(R1 + 1) % 16] = (word)(dividend % divisor);
    return ret;
}

std::optional<int> SHIFT_LEFT_DOUBLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SHIFT_LEFT_DOUBLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SHIFT_LEFT_SINGLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SHIFT_LEFT_SINGLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SHIFT_RIGHT_DOUBLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SHIFT_RIGHT_DOUBLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}
std::optional<int> SHIFT_RIGHT_SINGLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}
std::optional<int> SHIFT_RIGHT_SINGLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# DECIMAL ARITHMETIC #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> CONVERT_TO_BINARY(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> CONVERT_TO_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> PACK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> UNPACK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> ADD_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SUBTRACT_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MULTIPLY_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> DIVIDE_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# LOGICAL OPERATIONS #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> AND_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> AND(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> AND_SI(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> AND_SS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> OR_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> OR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> OR_SI(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> OR_SS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EXCLUSIVE_OR_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EXCLUSIVE_OR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EXCLUSIVE_OR_SI(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EXCLUSIVE_OR_SS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RR_FIELDS
    int32_t signedOprnd1 = std::bit_cast<int32_t>(cpu->rgstrs.gen[R1]);
    int32_t signedOprnd2 = std::bit_cast<int32_t>(cpu->rgstrs.gen[R2]);
    if (signedOprnd1 == signedOprnd2) {
        ret = 0;
    } else if (signedOprnd1 < signedOprnd2) {
        ret = 1;
    } else {
        ret = 2;
    } 
    return ret;
}

std::optional<int> COMPARE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    int32_t signedOprnd1 = std::bit_cast<int32_t>(cpu->rgstrs.gen[R1]);
    int32_t signedOprnd2 = std::bit_cast<int32_t>(cpu->getWord(cpu->getAddr(X1,B2,D2)));
    if (signedOprnd1 == signedOprnd2) {
        ret = 0;
    } else if (signedOprnd1 < signedOprnd2) {
        ret = 1;
    } else {
        ret = 2;
    }
    return ret;
}

std::optional<int> COMPARE_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_LOGICAL_SS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> COMPARE_LOGICAL_SI(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#-------------------------# BRANCHING #-------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> BRANCH_AND_LINK_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_AND_LINK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_AND_STORE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_AND_STORE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_ON_CONDITION_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    byte comp = static_cast<byte>(pow(2,cpu->getCond()));
    if (R1 & comp){
        cpu->setAddr(cpu->rgstrs.gen[R2]);
    }
    return ret;
}

std::optional<int> BRANCH_ON_CONDITION(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    byte comp = static_cast<byte>(pow(2,cpu->getCond()));
    if(R1 & comp){
        cpu->setAddr(cpu->getAddr(X1,B2,D2));
    }
    return ret;
}

std::optional<int> BRANCH_ON_COUNT_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_ON_COUNT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_ON_INDEX_HIGH(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> BRANCH_ON_INDEX_LOW_OR_EQUAL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EXECUTE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# SYSTEM #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> SUPERVISOR_CALL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SET_PROGRAM_MASK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SET_STORAGE_KEY(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> SET_SYSTEM_MASK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> DIAGNOSE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#------------------------# INPUT/OUTPUT #-----------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> HALT_IO(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> START_IO(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_SI_FIELDS
    halfword devAddr = cpu->getAddr(0,B1,D1);
    ret = cpu->startIO(leftByte(devAddr),leftNibble(rightByte(devAddr)),rightNibble(rightByte(devAddr)));
    return ret;
}

std::optional<int> TEST_IO(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> TEST_CHANNEL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> READ_DIRECT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> WRITE_DIRECT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# MEMORY #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> EDIT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> EDIT_AND_MARK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> LOAD_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2) { 
    std::optional<int> ret;
    LOAD_RR_FIELDS
    cpu->rgstrs.gen[R1] = cpu->rgstrs.gen[R2];
    return ret;
}

std::optional<int> LOAD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RX_FIELDS
    word address = cpu->getAddr(X1,B2,D2);
    cpu->rgstrs.gen[R1] = cpu->getWord(address); 
    return ret;
}

std::optional<int> LOAD_ADDRESS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
 
    return ret;
}

std::optional<int> LOAD_AND_TEST(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    cpu->rgstrs.gen[R1] = cpu->rgstrs.gen[R2];
    if (cpu->rgstrs.gen[R1] == 0){
        ret = 0;
    } else if (cpu->rgstrs.gen[R1] >= MAXNEG_32){
        ret = 1;
    } else {
        ret = 2;
    }
    return ret;
}

std::optional<int> LOAD_COMPLEMENT(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    if (cpu->rgstrs.gen[R2] == MAXNEG_32 && cpu->getPmsk() & FIP_OVERFLOW){
        //Raises FP overflow interrupt if the most negative number is complemented
        throw 0x08;
    }
    cpu->rgstrs.gen[R1] = ~(cpu->rgstrs.gen[R2]) + 1;
    return ret;
}

std::optional<int> LOAD_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    word address = cpu->getAddr(X1,B2,D2);
    cpu->rgstrs.gen[R1] = signex16_32(cpu->getHalfword(address));
    return ret;
}

std::optional<int> LOAD_MULTIPLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret;
    LOAD_RS_FIELDS
    word address = cpu->getAddr(0,B2,D2);
    do {
        cpu->rgstrs.gen[R1] = cpu->getWord(address);
        address += 4;
        R1 = ((R1 + 1) % 16);
    } while (R1 != ((R3 + 1) % 16));
    return ret;
}

std::optional<int> LOAD_MULTIPLE_CONTROL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> LOAD_NEGATIVE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    if (!(cpu->rgstrs.gen[R2] & MAXNEG_32)){
        cpu->rgstrs.gen[R1] = ~(cpu->rgstrs.gen[R2] + 1);
    } else {
        cpu->rgstrs.gen[R1] = cpu->rgstrs.gen[R2];
    }
    return ret;
}

std::optional<int> LOAD_POSITIVE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RR_FIELDS
    if (cpu->rgstrs.gen[R2] == MAXNEG_32 && cpu->getPmsk() & FIP_OVERFLOW){
        //Raises FP overflow interrupt if the most negative number is complemented
        throw 0x08;
    }
    if (cpu->rgstrs.gen[R2] & MAXNEG_32){
        cpu->rgstrs.gen[R1] = ~(cpu->rgstrs.gen[R2]) + 1;
    } else {
        cpu->rgstrs.gen[R1] = cpu->rgstrs.gen[R2];
    }
    return ret;
}

std::optional<int> LOAD_PSW(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> LOAD_REAL_ADDRESS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> STORE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> STORE_CHARACTER(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    LOAD_RX_FIELDS
    cpu->writeByte(cpu->rgstrs.gen[R1]%LSHFT8,cpu->getAddr(X1,B2,D2));
    return ret;
}

std::optional<int> STORE_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> STORE_MULTIPLE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> STORE_MULTIPLE_CONTROL(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MOVE_SI(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MOVE_SS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MOVE_NUMERICS(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MOVE_WITH_OFFSET(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> MOVE_ZONES(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> INSERT_CHARACTER(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> INSERT_STORAGE_KEY(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

/*#################################################################*/
/*#                                                               #*/
/*#----------------------------# MISC #---------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> TEST_AND_SET(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> TEST_UNDER_MASK(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> TRANSLATE(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> TRANSLATE_AND_TEST(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::optional<int> ZERO_AND_ADD(cpu *cpu,byte b1,halfword word1, halfword word2) {
    std::optional<int> ret; 
    return ret;
}

std::unordered_map<byte,instruction> stdisa = {
    {0x1A,{1,1,"AR (ADD)",&ADD_RGSTR}},
    {0x5A,{1,2,"A (ADD)",&ADD}},
    {0xFA,{1,2,"AP (ADD DECIMAL)",&ADD_DECIMAL}},
    {0x4A,{1,2,"AH (ADD HALFWORD)",&ADD_HALFWORD}},
    {0x1E,{1,1,"ALR (ADD LOGICAL)",&ADD_LOGICAL_RGSTR}},
    {0x5E,{1,2,"AL (ADD LOGICAL)",&ADD_LOGICAL}},
    {0x14,{1,1,"NR (AND)",&AND_RGSTR}},
    {0x54,{1,2,"N (AND)",&AND}},
    {0x94,{1,2,"NI (AND)",&AND_SI}},
    {0xD4,{1,3,"NC (AND)",&AND_SS}},
    {0x05,{1,1,"BALR (BRANCH AND LINK)",&BRANCH_AND_LINK_RGSTR}},
    {0x45,{1,2,"BAL (BRANCH AND LINK)",&BRANCH_AND_LINK}},
    {0x0D,{1,1,"BASR (BRANCH AND STORE)",&BRANCH_AND_STORE_RGSTR}},
    {0x4D,{1,2,"BAS (BRANCH AND STORE)",&BRANCH_AND_STORE}},
    {0x07,{1,1,"BCR (BRANCH ON CONDITION)",&BRANCH_ON_CONDITION_RGSTR}},
    {0x47,{1,2,"BC (BRANCH ON CONDITION)",&BRANCH_ON_CONDITION}},
    {0x06,{1,1,"BCTR (BRANCH ON COUNT)",&BRANCH_ON_COUNT_RGSTR}},
    {0x46,{1,2,"BCT (BRANCH ON COUNT)",&BRANCH_ON_COUNT}},
    {0x86,{1,2,"BXH (BRANCH ON INDEX HIGH)",&BRANCH_ON_INDEX_HIGH}},
    {0x87,{1,2,"BXLE (BRANCH ON INDEX LOW OR EQUAL)",&BRANCH_ON_INDEX_LOW_OR_EQUAL}},
    {0x19,{1,1,"CR (COMPARE)",&COMPARE_RGSTR}},
    {0x59,{1,2,"C (COMPARE)",&COMPARE}},
    {0xF9,{1,3,"CP (COMPARE DECIMAL)",&COMPARE_DECIMAL}},
    {0x49,{1,2,"CH (COMPARE HALFWORD)",&COMPARE_HALFWORD}},
    {0x15,{1,2,"CLR (COMPARE LOGICAL)",&COMPARE_LOGICAL_RGSTR}},
    {0x55,{1,2,"CL (COMPARE LOGICAL)",&COMPARE_LOGICAL}},
    {0xD5,{1,3,"CLC (COMPARE LOGICAL)",&COMPARE_LOGICAL_SS}},
    {0x94,{1,2,"CLI (COMPARE LOGICAL)",&COMPARE_LOGICAL_SI}},
    {0x4F,{1,2,"CVB (CONVERT TO BINARY)",&CONVERT_TO_BINARY}},
    {0x4E,{1,2,"CVD (CONVERT TO DECIMAL)",&CONVERT_TO_DECIMAL}},
    {0x83,{0,2,"DIAGNOSE",&DIAGNOSE}},
    {0x1D,{1,1,"DR (DIVIDE)",&DIVIDE_RGSTR}},
    {0x5D,{1,2,"D (DIVIDE)",&DIVIDE}},
    {0xFD,{1,3,"DP (DIVIDE DECIMAL)",&DIVIDE_DECIMAL}},
    {0xDE,{1,3,"ED (EDIT)",&EDIT}},
    {0xDF,{1,3,"EDMK (EDIT AND MARK)",&EDIT_AND_MARK}},
    {0x17,{1,1,"XR (EXCLUSIVE OR)",&EXCLUSIVE_OR_RGSTR}},
    {0x57,{1,2,"X (EXCLUSIVE OR)",&EXCLUSIVE_OR}},
    {0x97,{1,2,"XI (EXCLUSIVE OR)",&EXCLUSIVE_OR_SI}},
    {0xD7,{1,3,"XC (EXCLUSIVE OR)",&EXCLUSIVE_OR_SS}},
    {0x44,{1,2,"EX (EXECUTE)",&EXECUTE}},
    {0x9E,{0,2,"HIO (HALT I/O)",&HALT_IO}},
    {0x43,{1,2,"IC (INSERT CHARACTER)",&INSERT_CHARACTER}},
    {0x09,{0,1,"ISK (INSERT STORAGE KEY)",&INSERT_STORAGE_KEY}},
    {0x18,{1,1,"LR (LOAD)",&LOAD_RGSTR}},
    {0x58,{1,2,"L (LOAD)",&LOAD}},
    {0x41,{1,2,"LA (LOAD ADDRESS)",&LOAD_ADDRESS}},
    {0x12,{1,1,"LTR (LOAD AND TEST)",&LOAD_AND_TEST}},
    {0x13,{1,1,"LCR (LOAD COMPLEMENT)",&LOAD_COMPLEMENT}},
    {0x48,{1,2,"LH (LOAD HALFWORD)",&LOAD_HALFWORD}},
    {0x98,{1,2,"LM (LOAD MULTIPLE)",&LOAD_MULTIPLE}},
    {0xB8,{0,2,"LMC (LOAD MULTIPLE CONTROL)",&LOAD_MULTIPLE_CONTROL}},
    {0x11,{1,1,"LNR (LOAD NEGATIVE)",&LOAD_NEGATIVE}},
    {0x10,{1,1,"LPR (LOAD POSITIVE)",&LOAD_POSITIVE}},
    {0x82,{0,2,"LPSW (LOAD PSW)",&LOAD_PSW}},
    {0xB1,{0,2,"LRA (LOAD REAL ADDRESS)",&LOAD_REAL_ADDRESS}},
    {0x92,{1,2,"MVI (MOVE)",&MOVE_SI}},
    {0xD2,{1,3,"MVC (MOVE)",&MOVE_SS}},
    {0xD1,{1,3,"MVO (MOVE WITH OFFSET)",&MOVE_WITH_OFFSET}},
    {0xF1,{1,3,"MVN (MOVE NUMERICS)",&MOVE_NUMERICS}},
    {0xD3,{1,3,"MVZ (MOVE ZONES)",&MOVE_ZONES}},
    {0x1C,{1,1,"MR (MULTIPLY)",&MULTIPLY_RGSTR}},
    {0x5C,{1,2,"M (MULTIPLY)",&MULTIPLY}},
    {0xFC,{1,3,"MP (MULTIPLY DECIMAL)",&MULTIPLY_DECIMAL}},
    {0x4C,{1,2,"MH (MULTIPLY HALFWORD)",&MULTIPLY_HALFWORD}},
    {0x16,{1,1,"OR (OR)",&OR_RGSTR}},
    {0x56,{1,2,"O (OR)",&OR}},
    {0x96,{1,2,"OI (OR)",&OR_SI}},
    {0xD6,{1,3,"OC (OR)",&OR_SS}},
    {0xF2,{1,3,"PACK (PACK)",&PACK}},
    {0x85,{0,2,"RDD (READ DIRECT)",&READ_DIRECT}},
    {0x04,{1,1,"SPM (SET PROGRAM MASK)",&SET_PROGRAM_MASK}},
    {0x08,{0,1,"SSK (SET STORAGE KEY)",&SET_STORAGE_KEY}},
    {0x80,{0,2,"SSM (SET SYSTEM MASK)",&SET_SYSTEM_MASK}},
    {0x8F,{1,2,"SLDA (SHIFT LEFT DOUBLE)",&SHIFT_LEFT_DOUBLE}},
    {0x8D,{1,2,"SLDL (SHIFT LEFT DOUBLE LOGICAL)",&SHIFT_LEFT_DOUBLE_LOGICAL}},
    {0x8B,{1,2,"SLA (SHIFT LEFT SINGLE)",&SHIFT_LEFT_SINGLE}},
    {0x89,{1,2,"SLL (SHIFT LEFT SINGLE LOGICAL)",&SHIFT_LEFT_SINGLE_LOGICAL}},
    {0x8E,{1,2,"SRDA (SHIFT RIGHT DOUBLE)",&SHIFT_RIGHT_DOUBLE}},
    {0x8C,{1,2,"SRDL (SHIFT RIGHT DOUBLE LOGICAL)",&SHIFT_RIGHT_DOUBLE_LOGICAL}},
    {0x8A,{1,2,"SRA (SHIFT RIGHT SINGLE)",&SHIFT_RIGHT_SINGLE}},
    {0x88,{1,2,"SRL (SHIFT RIGHT SINGLE LOGICAL)",&SHIFT_RIGHT_SINGLE_LOGICAL}},
    {0x9C,{0,2,"SIO (START I/O)",&START_IO}},
    {0x50,{1,2,"ST (STORE)",&STORE}},
    {0x42,{1,2,"STC (STORE CHARACTER)",&STORE_CHARACTER}},
    {0x40,{1,2,"STH (STORE HALFWORD)",&STORE_HALFWORD}},
    {0x90,{1,2,"STM (STORE MULTIPLE)",&STORE_MULTIPLE}},
    {0xB0,{0,2,"STMC (STORE MULTIPLE CONTROL)",&STORE_MULTIPLE_CONTROL}},
    {0x1B,{1,1,"SR (SUBTRACT)",&SUBTRACT_RGSTR}},
    {0x5B,{1,2,"S (SUBTRACT)",&SUBTRACT}},
    {0xFB,{1,3,"SP (SUBTRACT DECIMAL)",&SUBTRACT_DECIMAL}},
    {0x4B,{1,2,"SH (SUBTRACT HALFWORD)",&SUBTRACT_HALFWORD}},
    {0x1F,{1,1,"SLR (SUBTRACT LOGICAL)",&SUBTRACT_LOGICAL_RGSTR}},
    {0x5F,{1,2,"SL (SUBTRACT LOGICAL)",&SUBTRACT_LOGICAL}},
    {0x0A,{1,1,"SVC (SUPERVISOR CALL)",&SUPERVISOR_CALL}},
    {0x93,{1,2,"TS (TEST AND SET)",&TEST_AND_SET}},
    {0x9F,{0,2,"TCH (TEST CHANNEL)",&TEST_CHANNEL}},
    {0x9D,{0,2,"TIO (TEST I/O)",&TEST_IO}},
    {0x91,{1,2,"TM (TEST UNDER MASK)",&TEST_UNDER_MASK}},
    {0xDC,{1,3,"TR (TRANSLATE)",&TRANSLATE}},
    {0xDD,{1,3,"TRT (TRANSLATE AND TEST)",&TRANSLATE_AND_TEST}},
    {0xF3,{1,3,"UNPK (UNPACK)",&UNPACK}},
    {0x84,{0,2,"WRD (WRITE DIRECT)",&WRITE_DIRECT}},
    {0xF8,{1,3,"ZAP (ZERO AND ADD)",&ZERO_AND_ADD}},
};