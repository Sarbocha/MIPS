#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Instruction.h"

using namespace std;

class Parser {
public:
    vector<Instruction> parseFile(string filename);
private:
    string trim(const string& s);
    int parseRegister(const string& token);
    int parseInteger(const string& token);
    vector<string> splitOperands(const string& operands);
    void firstPassLabels(const vector<string>& lines, unordered_map<string, int>& labelToIndex);
    Instruction parseInstructionLine(
        const string& line,
        int currentIndex,
        const unordered_map<string, int>& labelToIndex
    );
};

#endif
