#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

string Parser::trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    return s.substr(start, end - start);
}

int Parser::parseRegister(const string& token) {
    string cleaned = trim(token);
    if (!cleaned.empty() && cleaned[0] == '$') {
        cleaned = cleaned.substr(1);
    }
    return stoi(cleaned);
}

int Parser::parseInteger(const string& token) {
    return stoi(trim(token));
}

vector<string> Parser::splitOperands(const string& operands) {
    vector<string> out;
    string current;
    for (char c : operands) {
        if (c == ',') {
            out.push_back(trim(current));
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        out.push_back(trim(current));
    }
    return out;
}

void Parser::firstPassLabels(const vector<string>& lines, unordered_map<string, int>& labelToIndex) {
    int instructionIndex = 0;
    for (string line : lines) {
        size_t commentPos = line.find('#');
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string label = trim(line.substr(0, colonPos));
            if (!label.empty()) {
                labelToIndex[label] = instructionIndex;
            }
            string afterLabel = trim(line.substr(colonPos + 1));
            if (!afterLabel.empty()) {
                instructionIndex++;
            }
        } else {
            instructionIndex++;
        }
    }
}

Instruction Parser::parseInstructionLine(
    const string& line,
    int currentIndex,
    const unordered_map<string, int>& labelToIndex
) {
    Instruction inst;
    inst.raw = line;

    string work = line;
    size_t colonPos = work.find(':');
    if (colonPos != string::npos) {
        work = trim(work.substr(colonPos + 1));
    }

    stringstream ss(work);
    ss >> inst.opcode;
    transform(inst.opcode.begin(), inst.opcode.end(), inst.opcode.begin(), ::toupper);

    string operandsText;
    getline(ss, operandsText);
    operandsText = trim(operandsText);
    vector<string> ops = splitOperands(operandsText);

    if (inst.opcode == "ADD" || inst.opcode == "SUB" || inst.opcode == "MUL" ||
        inst.opcode == "AND" || inst.opcode == "OR") {
        inst.rd = parseRegister(ops.at(0));
        inst.rs = parseRegister(ops.at(1));
        inst.rt = parseRegister(ops.at(2));
    } else if (inst.opcode == "SLL" || inst.opcode == "SRL") {
        inst.rd = parseRegister(ops.at(0));
        inst.rt = parseRegister(ops.at(1));
        inst.shamt = parseInteger(ops.at(2));
    } else if (inst.opcode == "ADDI") {
        inst.rt = parseRegister(ops.at(0));
        inst.rs = parseRegister(ops.at(1));
        inst.immediate = parseInteger(ops.at(2));
    } else if (inst.opcode == "LW" || inst.opcode == "SW") {
        // formats: rt, offset(rs)  OR  rt, offset, rs (comma-separated)
        inst.rt = parseRegister(ops.at(0));
        if (ops.size() >= 3 && ops[1].find('(') == string::npos) {
            inst.immediate = parseInteger(ops[1]);
            inst.rs = parseRegister(ops[2]);
        } else {
            string addr = ops.at(1);
            size_t leftParen = addr.find('(');
            size_t rightParen = addr.find(')');
            inst.immediate = parseInteger(addr.substr(0, leftParen));
            inst.rs = parseRegister(addr.substr(leftParen + 1, rightParen - leftParen - 1));
        }
    } else if (inst.opcode == "BEQ") {
        inst.rs = parseRegister(ops.at(0));
        inst.rt = parseRegister(ops.at(1));
        string target = trim(ops.at(2));
        auto it = labelToIndex.find(target);
        if (it != labelToIndex.end()) {
            inst.address = it->second;
        } else {
            inst.address = parseInteger(target);
        }
        inst.branchOffset = inst.address - (currentIndex + 1);
    } else if (inst.opcode == "J") {
        string target = trim(ops.at(0));
        auto it = labelToIndex.find(target);
        if (it != labelToIndex.end()) {
            inst.address = it->second;
        } else {
            inst.address = parseInteger(target);
        }
    } else if (inst.opcode == "NOP") {
        // No operands
    } else {
        throw runtime_error("Unsupported opcode: " + inst.opcode + " at instruction index " + to_string(currentIndex));
    }

    return inst;
}

vector<Instruction> Parser::parseFile(string filename) {
    vector<Instruction> instructions;
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Unable to open input file: " + filename);
    }

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    unordered_map<string, int> labelToIndex;
    firstPassLabels(lines, labelToIndex);

    int instructionIndex = 0;
    for (string rawLine : lines) {
        size_t commentPos = rawLine.find('#');
        if (commentPos != string::npos) {
            rawLine = rawLine.substr(0, commentPos);
        }
        rawLine = trim(rawLine);
        if (rawLine.empty()) {
            continue;
        }

        string withoutLeadingLabel = rawLine;
        size_t colonPos = rawLine.find(':');
        if (colonPos != string::npos) {
            withoutLeadingLabel = trim(rawLine.substr(colonPos + 1));
            if (withoutLeadingLabel.empty()) {
                continue;
            }
        }

        Instruction inst = parseInstructionLine(rawLine, instructionIndex, labelToIndex);
        instructions.push_back(inst);
        instructionIndex++;
    }

    return instructions;
}
