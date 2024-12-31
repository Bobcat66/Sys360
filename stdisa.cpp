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
