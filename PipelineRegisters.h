#ifndef PIPELINE_REGISTERS_H
#define PIPELINE_REGISTERS_H

#include "Instruction.h"

struct IF_ID {
    Instruction instr;
    int pc = 0;
    bool valid = false;
};

struct ID_EX {
    Instruction instr;
    int pc = 0;
    int readData1 = 0;
    int readData2 = 0;
    int immediate = 0;
    int address = 0;
    int shamt = 0;
    int rs = 0;
    int rt = 0;
    int rd = 0;
    // Control signals
    bool regWrite = false;
    bool memRead = false;
    bool memWrite = false;
    bool memToReg = false;
    bool useImmediate = false;
    bool branch = false;
    bool jump = false;
    bool isNop = false;
    bool valid = false;
};

struct EX_MEM {
    Instruction instr;
    int pc = 0;
    int aluResult = 0;
    int writeData = 0;
    int writeReg = 0;
    int branchTarget = 0;
    bool branchTaken = false;
    bool jump = false;
    bool regWrite = false;
    bool memRead = false;
    bool memWrite = false;
    bool memToReg = false;
    bool valid = false;
};

struct MEM_WB {
    Instruction instr;
    int memData = 0;
    int aluResult = 0;
    int writeReg = 0;
    bool regWrite = false;
    bool memToReg = false;
    bool valid = false;
};

#endif
