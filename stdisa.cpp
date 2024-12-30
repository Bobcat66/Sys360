#include "stdisa.h"

/*#################################################################*/
/*#                                                               #*/
/*#-------------------# FIXED-POINT ARITHMETIC #------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> ADD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ADD_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ADD_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ADD_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ADD_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MULTIPLY_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MULTIPLY(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MULTIPLY_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> DIVIDE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> DIVIDE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_LEFT_DOUBLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_LEFT_DOUBLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_LEFT_SINGLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_LEFT_SINGLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_RIGHT_DOUBLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_RIGHT_DOUBLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_RIGHT_SINGLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SHIFT_RIGHT_SINGLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# DECIMAL ARITHMETIC #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> CONVERT_TO_BINARY(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> CONVERT_TO_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> PACK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> UNPACK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ADD_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SUBTRACT_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MULTIPLY_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> DIVIDE_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------# LOGICAL OPERATIONS #--------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> AND_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> AND(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> AND_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> AND_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> OR_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> OR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> OR_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> OR_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EXCLUSIVE_OR_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EXCLUSIVE_OR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EXCLUSIVE_OR_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EXCLUSIVE_OR_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_LOGICAL_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> COMPARE_LOGICAL_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#-------------------------# BRANCHING #-------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> BRANCH_AND_LINK_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_AND_LINK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_AND_STORE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_AND_STORE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_CONDITION_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_CONDITION(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_COUNT_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_COUNT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_INDEX_HIGH(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> BRANCH_ON_INDEX_LOW_OR_EQUAL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EXECUTE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# SYSTEM #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> SUPERVISOR_CALL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SET_PROGRAM_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SET_STORAGE_KEY(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> SET_SYSTEM_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> DIAGNOSE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#------------------------# INPUT/OUTPUT #-----------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> HALT_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> START_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> TEST_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> TEST_CHANNEL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> READ_DIRECT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> WRITE_DIRECT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#---------------------------# MEMORY #--------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> EDIT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> EDIT_AND_MARK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_ADDRESS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_AND_TEST(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_COMPLEMENT(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_MULTIPLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_MULTIPLE_CONTROL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_NEGATIVE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_POSITIVE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_PSW(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> LOAD_REAL_ADDRESS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> STORE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> STORE_CHARACTER(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> STORE_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> STORE_MULTIPLE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> STORE_MULTIPLE_CONTROL(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MOVE_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MOVE_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MOVE_NUMERICS(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MOVE_WITH_OFFSET(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> MOVE_ZONES(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> INSERT_CHARACTER(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> INSERT_STORAGE_KEY(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}

/*#################################################################*/
/*#                                                               #*/
/*#----------------------------# MISC #---------------------------#*/
/*#                                                               #*/
/*#################################################################*/

std::optional<int> TEST_AND_SET(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> TEST_UNDER_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> TRANSLATE(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> TRANSLATE_AND_TEST(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
std::optional<int> ZERO_AND_ADD(cpu *cpuptr,byte b1,halfword word1, halfword word2) {}
