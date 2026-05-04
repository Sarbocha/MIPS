#include <iostream>
#include <string>
#include "Parser.h"
#include "CPU.h"

using namespace std;

static void printUsage(const char* prog) {
    cerr << "Usage: " << prog << " [--debug] <program.asm>\n";
    cerr << "       " << prog << " <program.asm> [--debug]\n";
}

int main(int argc, char* argv[]) {
    string inputFile;
    bool debug = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--debug") {
            debug = true;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (!inputFile.empty()) {
            cerr << "Error: only one assembly file allowed (extra: " << arg << ")\n";
            printUsage(argv[0]);
            return 1;
        } else {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        cerr << "Error: missing assembly file path.\n";
        printUsage(argv[0]);
        return 1;
    }

    try {
        Parser parser;
        vector<Instruction> instructions = parser.parseFile(inputFile);

        CPU cpu(instructions, debug);
        cpu.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
