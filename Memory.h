#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
using namespace std;

class Memory {
public:
    vector<int> mem;

    Memory(int size = 1024) {
        mem.resize(size, 0);
    }

    int load(int address) {
        return mem[address];
    }

    void store(int address, int value) {
        mem[address] = value;
    }
};

#endif
