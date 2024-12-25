#ifndef STDISA_H
#define STDISA_H

#include "cpuhelpers.h"
#include "cpu.h"
#include <unordered_map>

void ADD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ADD_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ADD_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ADD_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ADD_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ADD_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void AND_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void AND(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void AND_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void AND_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void BRANCH_AND_LINK_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_AND_LINK(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_AND_STORE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_AND_STORE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_CONDITION_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_CONDITION(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_COUNT_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_COUNT(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_INDEX_HIGH(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void BRANCH_ON_INDEX_LOW_OR_EQUAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void COMPARE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_LOGICAL_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void COMPARE_LOGICAL_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void CONVERT_TO_BINARY(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void CONVERT_TO_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void DIVIDE_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void DIVIDE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void DIVIDE_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void EDIT(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void EDIT_AND_MARK(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void EXCLUSIVE_OR_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void EXCLUSIVE_OR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void EXCLUSIVE_OR_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void EXCLUSIVE_OR_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void EXECUTE(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void HALT_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void INSERT_CHARACTER(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void INSERT_STORAGE_KEY(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void LOAD_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_ADDRESS(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_AND_TEST(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_COMPLEMENT(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_MULTIPLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_MULTIPLE_CONTROL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_NEGATIVE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_POSITIVE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_PSW(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_REAL_ADDRESS(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void MOVE_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MOVE_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MOVE_NUMERICS(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MOVE_WITH_OFFSET(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MOVE_ZONES(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void MULTIPLY_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MULTIPLY(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MULTIPLY_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void MULTIPLY_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void OR_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void OR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void OR_SI(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void OR_SS(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void PACK(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void READ_DIRECT(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void SET_PROGRAM_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SET_STORAGE_KEY(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SET_SYSTEM_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void SHIFT_LEFT_DOUBLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_LEFT_DOUBLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_LEFT_SINGLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_LEFT_SINGLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_RIGHT_DOUBLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_RIGHT_DOUBLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_RIGHT_SINGLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void LOAD_AND_TEST(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_RIGHT_SINGLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SHIFT_RIGHT_SINGLE_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void START_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void STORE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void STORE_CHARACTER(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void STORE_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void STORE_MULTIPLE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void STORE_MULTIPLE_CONTROL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void SUBTRACT_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SUBTRACT(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SUBTRACT_DECIMAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SUBTRACT_HALFWORD(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SUBTRACT_LOGICAL_RGSTR(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void SUBTRACT_LOGICAL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void SUPERVISOR_CALL(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void TEST_AND_SET(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void TEST_CHANNEL(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void TEST_IO(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void TEST_UNDER_MASK(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void TRANSLATE(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void TRANSLATE_AND_TEST(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void UNPACK(cpu *cpuptr,byte b1,halfword word1, halfword word2);

void WRITE_DIRECT(cpu *cpuptr,byte b1,halfword word1, halfword word2);
void ZERO_AND_ADD(cpu *cpuptr,byte b1,halfword word1, halfword word2);

const std::unordered_map<byte,instruction> stdisa = {
    {0x1A,{1,"AR (ADD)",&ADD_RGSTR}},
    {0x5A,{1,"A (ADD)",&ADD}},
    {0xFA,{1,"AP (ADD DECIMAL)",&ADD_DECIMAL}},
    {0x4A,{1,"AH (ADD HALFWORD)",&ADD_HALFWORD}},
    {0x1E,{1,"ALR (ADD LOGICAL)",&ADD_LOGICAL_RGSTR}},
    {0x5E,{1,"AL (ADD LOGICAL)",&ADD_LOGICAL}},
};

#endif