#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
using namespace std;

struct Instruction {
    string opcode;

    int rs = 0;
    int rt = 0;
    int rd = 0;

    int immediate = 0;
    int address = 0;
    int shamt = 0;
    /// Word offset for BEQ (targetIndex - (beqIndex + 1)); used for machine-code listing.
    int branchOffset = 0;

    string raw; // original instruction (for debug)
};

#endif
