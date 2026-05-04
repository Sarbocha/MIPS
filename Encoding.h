#ifndef ENCODING_H
#define ENCODING_H

#include <string>
#include "Instruction.h"

std::string toBinary(int num, int bits);
std::string getBinaryInstruction(const Instruction& inst);
void printControlSignals(const Instruction& inst);

#endif
