#include "cpu.h"

cpu::cpu(int memblocks,std::unordered_map<byte,instruction> *ISAPtr){
    core = std::make_shared<memory>(memblocks);
    ISA.reset(ISAPtr);
}