#ifndef STDISA_H
#define STDISA_H

#include "cpuhelpers.h"
#include "cpu.h"
#include <unordered_map>
#include <optional>

/*#################################################################*/
/*#                                                               #*/
/*#-------------------# FIXED-POINT ARITHMETIC #------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> ADD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ADD_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ADD_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ADD_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ADD_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MULTIPLY_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MULTIPLY(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MULTIPLY_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> DIVIDE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> DIVIDE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_LEFT_DOUBLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_LEFT_DOUBLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_LEFT_SINGLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_LEFT_SINGLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_RIGHT_DOUBLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_RIGHT_DOUBLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_RIGHT_SINGLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SHIFT_RIGHT_SINGLE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# DECIMAL ARITHMETIC #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> CONVERT_TO_BINARY(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> CONVERT_TO_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> PACK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> UNPACK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ADD_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SUBTRACT_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MULTIPLY_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> DIVIDE_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# LOGICAL OPERATIONS #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> AND_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> AND(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> AND_SI(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> AND_SS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> OR_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> OR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> OR_SI(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> OR_SS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EXCLUSIVE_OR_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EXCLUSIVE_OR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EXCLUSIVE_OR_SI(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EXCLUSIVE_OR_SS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_DECIMAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_LOGICAL_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_LOGICAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_LOGICAL_SS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> COMPARE_LOGICAL_SI(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#-------------------------# BRANCHING #-------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> BRANCH_AND_LINK_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_AND_LINK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_AND_STORE_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_AND_STORE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_CONDITION_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_CONDITION(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_COUNT_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_COUNT(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_INDEX_HIGH(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> BRANCH_ON_INDEX_LOW_OR_EQUAL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EXECUTE(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# SYSTEM #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> SUPERVISOR_CALL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SET_PROGRAM_MASK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SET_STORAGE_KEY(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> SET_SYSTEM_MASK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> DIAGNOSE(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#------------------------# INPUT/OUTPUT #-----------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> HALT_IO(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> START_IO(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> TEST_IO(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> TEST_CHANNEL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> READ_DIRECT(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> WRITE_DIRECT(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# MEMORY #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> EDIT(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> EDIT_AND_MARK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_RGSTR(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_ADDRESS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_AND_TEST(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_COMPLEMENT(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_MULTIPLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_MULTIPLE_CONTROL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_NEGATIVE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_POSITIVE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_PSW(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> LOAD_REAL_ADDRESS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> STORE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> STORE_CHARACTER(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> STORE_HALFWORD(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> STORE_MULTIPLE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> STORE_MULTIPLE_CONTROL(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MOVE_SI(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MOVE_SS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MOVE_NUMERICS(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MOVE_WITH_OFFSET(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> MOVE_ZONES(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> INSERT_CHARACTER(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> INSERT_STORAGE_KEY(cpu *cpu,byte b1,halfword word1, halfword word2);

/*#################################################################*/
/*#                                                               #*/
/*#----------------------------# MISC #---------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> TEST_AND_SET(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> TEST_UNDER_MASK(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> TRANSLATE(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> TRANSLATE_AND_TEST(cpu *cpu,byte b1,halfword word1, halfword word2);
std::optional<int> ZERO_AND_ADD(cpu *cpu,byte b1,halfword word1, halfword word2);


extern std::unordered_map<byte,instruction> stdisa;
#endif