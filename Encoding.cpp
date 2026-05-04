#include "Encoding.h"
#include <iostream>

using namespace std;

string toBinary(int num, int bits) {
    unsigned mask = (bits >= 32) ? 0xFFFFFFFFu : ((1u << bits) - 1u);
    unsigned u = static_cast<unsigned>(num) & mask;
    string s;
    for (int i = bits - 1; i >= 0; i--) {
        s += ((u >> i) & 1u) ? '1' : '0';
    }
    return s;
}

string getBinaryInstruction(const Instruction& inst) {
    if (inst.opcode == "ADD")
        return "000000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) + "00000" + "100000";

    if (inst.opcode == "SUB")
        return "000000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) + "00000" + "100010";

    if (inst.opcode == "MUL")
        return "000000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) +
               toBinary(inst.rd, 5) + "00000" + "011000";

    if (inst.opcode == "AND")
        return "000000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) + "00000" + "100100";

    if (inst.opcode == "OR")
        return "000000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) + "00000" + "100101";

    if (inst.opcode == "SLL")
        return "000000" + toBinary(0, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) +
               toBinary(inst.shamt, 5) + "000000";

    if (inst.opcode == "SRL")
        return "000000" + toBinary(0, 5) + toBinary(inst.rt, 5) + toBinary(inst.rd, 5) +
               toBinary(inst.shamt, 5) + "000010";

    if (inst.opcode == "ADDI")
        return "001000" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.immediate, 16);

    if (inst.opcode == "LW")
        return "100011" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.immediate, 16);

    if (inst.opcode == "SW")
        return "101011" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.immediate, 16);

    if (inst.opcode == "BEQ")
        return "000100" + toBinary(inst.rs, 5) + toBinary(inst.rt, 5) + toBinary(inst.branchOffset, 16);

    if (inst.opcode == "J")
        return "000010" + toBinary(inst.address, 26);

    return "00000000000000000000000000000000";
}

void printControlSignals(const Instruction& inst) {
    int RegWrite = 0, MemRead = 0, MemWrite = 0, Branch = 0, Jump = 0;

    if (inst.opcode == "ADD" || inst.opcode == "SUB" ||
        inst.opcode == "MUL" || inst.opcode == "AND" ||
        inst.opcode == "OR" || inst.opcode == "SLL" ||
        inst.opcode == "SRL") {
        RegWrite = 1;
    } else if (inst.opcode == "ADDI") {
        RegWrite = 1;
    } else if (inst.opcode == "LW") {
        RegWrite = 1;
        MemRead = 1;
    } else if (inst.opcode == "SW") {
        MemWrite = 1;
    } else if (inst.opcode == "BEQ") {
        Branch = 1;
    } else if (inst.opcode == "J") {
        Jump = 1;
    }

    cout << "Control: RegWrite=" << RegWrite
         << " MemRead=" << MemRead
         << " MemWrite=" << MemWrite
         << " Branch=" << Branch
         << " Jump=" << Jump << endl;
}
