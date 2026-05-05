#include "CPU.h"
#include <algorithm>
#include <iostream>
using namespace std;

CPU::CPU(vector<Instruction> instrs, bool debug) : instructions(instrs), debugMode(debug) {}

void CPU::run() {
    int cycle = 0;

    while (PC < static_cast<int>(instructions.size()) || if_id.valid || id_ex.valid || ex_mem.valid || mem_wb.valid) {
        cycle++;

        WB_stage();
        MEM_stage();
        EX_stage();
        ID_stage();
        IF_stage();

        if (debugMode)
            printState(cycle);
    }

    printFinalState();
}

void CPU::IF_stage() {
    if (branchOrJumpFlush) {
        if_id.valid = false;
        branchOrJumpFlush = false;
        return;
    }

    if (PC < static_cast<int>(instructions.size())) {
        if_id.instr = instructions[PC];
        if_id.pc = PC;
        if_id.valid = true;
        PC++;
    } else {
        if_id.valid = false;
    }
}

void CPU::ID_stage() {
    if (!if_id.valid) {
        id_ex.valid = false;
        return;
    }

    id_ex.instr = if_id.instr;
    id_ex.pc = if_id.pc;
    id_ex.rs = if_id.instr.rs;
    id_ex.rt = if_id.instr.rt;
    id_ex.rd = if_id.instr.rd;
    id_ex.readData1 = registers[if_id.instr.rs];
    id_ex.readData2 = registers[if_id.instr.rt];
    id_ex.immediate = if_id.instr.immediate;
    id_ex.address = if_id.instr.address;
    id_ex.shamt = if_id.instr.shamt;

    id_ex.regWrite = false;
    id_ex.memRead = false;
    id_ex.memWrite = false;
    id_ex.memToReg = false;
    id_ex.useImmediate = false;
    id_ex.branch = false;
    id_ex.jump = false;
    id_ex.isNop = false;

    const string& op = if_id.instr.opcode;
    if (op == "ADD" || op == "SUB" || op == "MUL" || op == "AND" || op == "OR" || op == "SLL" || op == "SRL") {
        id_ex.regWrite = true;
    } else if (op == "ADDI") {
        id_ex.regWrite = true;
        id_ex.useImmediate = true;
    } else if (op == "LW") {
        id_ex.regWrite = true;
        id_ex.memRead = true;
        id_ex.memToReg = true;
        id_ex.useImmediate = true;
    } else if (op == "SW") {
        id_ex.memWrite = true;
        id_ex.useImmediate = true;
    } else if (op == "BEQ") {
        id_ex.branch = true;
    } else if (op == "J") {
        id_ex.jump = true;
    } else if (op == "NOP") {
        id_ex.isNop = true;
    }

    id_ex.valid = true;
}

void CPU::EX_stage() {
    if (!id_ex.valid) {
        ex_mem.valid = false;
        return;
    }

    Instruction instr = id_ex.instr;
    ex_mem.instr = instr;
    ex_mem.pc = id_ex.pc;
    ex_mem.writeData = id_ex.readData2;
    ex_mem.branchTarget = id_ex.address;
    ex_mem.branchTaken = false;
    ex_mem.jump = id_ex.jump;
    ex_mem.regWrite = id_ex.regWrite;
    ex_mem.memRead = id_ex.memRead;
    ex_mem.memWrite = id_ex.memWrite;
    ex_mem.memToReg = id_ex.memToReg;
    ex_mem.writeReg = 0;
    ex_mem.aluResult = 0;

    if (instr.opcode == "ADD") {
        ex_mem.aluResult = id_ex.readData1 + id_ex.readData2;
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "ADDI") {
        ex_mem.aluResult = id_ex.readData1 + id_ex.immediate;
        ex_mem.writeReg = instr.rt;
    } else if (instr.opcode == "SUB") {
        ex_mem.aluResult = id_ex.readData1 - id_ex.readData2;
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "MUL") {
        ex_mem.aluResult = id_ex.readData1 * id_ex.readData2;
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "AND") {
        ex_mem.aluResult = id_ex.readData1 & id_ex.readData2;
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "OR") {
        ex_mem.aluResult = id_ex.readData1 | id_ex.readData2;
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "SLL") {
        ex_mem.aluResult = static_cast<int>(static_cast<unsigned int>(id_ex.readData2) << id_ex.shamt);
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "SRL") {
        ex_mem.aluResult = static_cast<int>(static_cast<unsigned int>(id_ex.readData2) >> id_ex.shamt);
        ex_mem.writeReg = instr.rd;
    } else if (instr.opcode == "LW" || instr.opcode == "SW") {
        ex_mem.aluResult = id_ex.readData1 + id_ex.immediate;
        ex_mem.writeReg = instr.rt;
    } else if (instr.opcode == "BEQ") {
        ex_mem.branchTaken = (id_ex.readData1 == id_ex.readData2);
    } else if (instr.opcode == "J") {
        // Jump target already propagated in branchTarget
    } else if (instr.opcode == "NOP") {
        // No action
    }

    if ((instr.opcode == "BEQ" && ex_mem.branchTaken) || instr.opcode == "J") {
        PC = ex_mem.branchTarget;
        if_id.valid = false;
        id_ex.valid = false;
        branchOrJumpFlush = true;
    }

    ex_mem.valid = true;
}

void CPU::MEM_stage() {
    if (!ex_mem.valid) {
        mem_wb.valid = false;
        return;
    }

    Instruction instr = ex_mem.instr;
    mem_wb.instr = instr;
    mem_wb.regWrite = ex_mem.regWrite;
    mem_wb.memToReg = ex_mem.memToReg;
    mem_wb.writeReg = ex_mem.writeReg;

    if (ex_mem.memRead) {
        mem_wb.memData = memory.load(ex_mem.aluResult);
    } else {
        mem_wb.memData = 0;
    }

    if (ex_mem.memWrite) {
        memory.store(ex_mem.aluResult, ex_mem.writeData);
    }

    mem_wb.aluResult = ex_mem.aluResult;
    mem_wb.valid = true;
}

void CPU::WB_stage() {
    if (!mem_wb.valid) return;

    if (mem_wb.regWrite && mem_wb.writeReg != 0) {
        if (mem_wb.memToReg) {
            registers[mem_wb.writeReg] = mem_wb.memData;
        } else {
            registers[mem_wb.writeReg] = mem_wb.aluResult;
        }
    }
}

void CPU::printState(int cycle) {
    // Classic pipeline trace (early project style): stage latches + instruction text only.
    cout << "\nCycle " << cycle << ":\n";

    if (if_id.valid) cout << "IF/ID: " << if_id.instr.raw << endl;
    if (id_ex.valid) cout << "ID/EX: " << id_ex.instr.raw << endl;
    if (ex_mem.valid) cout << "EX/MEM: " << ex_mem.instr.raw << endl;
    if (mem_wb.valid) cout << "MEM/WB: " << mem_wb.instr.raw << endl;

    // PDF: debug must show emitted control signals — one compact line from decode stage.
    if (id_ex.valid) {
        cout << "Control: regWrite=" << id_ex.regWrite
             << " memRead=" << id_ex.memRead
             << " memWrite=" << id_ex.memWrite
             << " memToReg=" << id_ex.memToReg
             << " useImmediate=" << id_ex.useImmediate
             << " branch=" << id_ex.branch
             << " jump=" << id_ex.jump
             << " isNop=" << id_ex.isNop << endl;
    }

    cout << "Registers:\n";
    for (int i = 0; i < 8; i++) {
        cout << "R" << i << ":" << registers[i] << " ";
    }
    cout << endl;
}

void CPU::printFinalState() {
    cout << "\nFinal Register State:\n";
    for (int i = 0; i < 32; i++) {
        cout << "R" << i << ": " << registers[i] << endl;
    }

    // PDF: final memory contents — low words listed explicitly; then any non-zero above index 15.
    cout << "\nFinal Memory State:\n";
    int limit = min(16, static_cast<int>(memory.mem.size()));
    for (int i = 0; i < limit; i++) {
        cout << "Mem[" << i << "] = " << memory.mem[i] << endl;
    }
    for (int i = 16; i < static_cast<int>(memory.mem.size()); i++) {
        if (memory.mem[i] != 0) {
            cout << "Mem[" << i << "] = " << memory.mem[i] << endl;
        }
    }
}
