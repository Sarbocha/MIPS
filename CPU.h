#ifndef CPU_H
#define CPU_H

#include <vector>
#include "Instruction.h"
#include "PipelineRegisters.h"
#include "Memory.h"

class CPU {
private:
    int PC = 0;
    int registers[32] = {0};

    vector<Instruction> instructions;

    Memory memory;

    // Pipeline registers
    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;

    bool debugMode = false;
    bool branchOrJumpFlush = false;

public:
    CPU(vector<Instruction> instrs, bool debug = false);

    void run();

private:
    void IF_stage();
    void ID_stage();
    void EX_stage();
    void MEM_stage();
    void WB_stage();

    void printState(int cycle);
    void printFinalState();
};

#endif
