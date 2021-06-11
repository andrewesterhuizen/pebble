#pragma once

#include <iostream>
#include <vector>

#include "opcode.h"
#include "instruction.h"

namespace pebble {

enum Register {
    RegisterA,
    RegisterB,
    RegisterIP,
    RegisterSP,
    RegisterFP,
    NumRegisters
};

const unsigned int memory_size = 80;

class VM {
    unsigned int a = 0;
    unsigned int b = 0;
    unsigned int ip = 0;
    unsigned int sp = memory_size - 1;
    unsigned int fp = memory_size - 1;
    unsigned int memory[memory_size];
    unsigned int* registers[5];

    unsigned int fetch();
    void push(unsigned int value);
    unsigned int pop();

    void execute(unsigned int instruction);

    template<typename T>

    T* fetch_next_instruction();

public:
    VM();
    void load(std::vector<unsigned int> instructions);
    void run();
};

}