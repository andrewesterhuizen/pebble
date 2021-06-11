#pragma once

#include "opcode.h"

namespace pebble::Instruction {

struct Halt {
    unsigned int opcode = Opcode::Halt;
};

struct Load {
    unsigned int opcode = Opcode::Load;
    unsigned int destination;
    unsigned int source_mode;
    unsigned int source;
};

struct Store {
    unsigned int opcode = Opcode::Store;
    unsigned int destination_mode;
    unsigned int destination;
    unsigned int source;
};

struct Move {
    unsigned int opcode = Opcode::Move;
    unsigned int destination;
    unsigned int source_type;
    unsigned int source;
};

#define ARITHMETIC_LOGIC_INSTRUCTION(NAME) \
struct NAME { \
    unsigned int opcode = Opcode::NAME; \
    unsigned int destination; \
    unsigned int source_type; \
    unsigned int source; \
}

ARITHMETIC_LOGIC_INSTRUCTION(Add);
ARITHMETIC_LOGIC_INSTRUCTION(Subtract);
ARITHMETIC_LOGIC_INSTRUCTION(Multiply);
ARITHMETIC_LOGIC_INSTRUCTION(Divide);
ARITHMETIC_LOGIC_INSTRUCTION(Modulo);
ARITHMETIC_LOGIC_INSTRUCTION(And);
ARITHMETIC_LOGIC_INSTRUCTION(Or);
ARITHMETIC_LOGIC_INSTRUCTION(Not);
ARITHMETIC_LOGIC_INSTRUCTION(ShiftLeft);
ARITHMETIC_LOGIC_INSTRUCTION(ShiftRight);
ARITHMETIC_LOGIC_INSTRUCTION(GreaterThan);
ARITHMETIC_LOGIC_INSTRUCTION(GreaterThanOrEqualTo);
ARITHMETIC_LOGIC_INSTRUCTION(LessThan);
ARITHMETIC_LOGIC_INSTRUCTION(LessThanOrEqualTo);
ARITHMETIC_LOGIC_INSTRUCTION(EqualTo);
ARITHMETIC_LOGIC_INSTRUCTION(NotEqualTo);

struct Jump {
    unsigned int opcode = Opcode::Jump;
    unsigned int address;
};

struct JumpIfZero {
    unsigned int opcode = Opcode::JumpIfZero;
    unsigned int address;
};

struct JumpIfNonZero {
    unsigned int opcode = Opcode::JumpIfNonZero;
    unsigned int address;
};

struct Call {
    unsigned int opcode = Opcode::Call;
    unsigned int address;
};

struct Return {
    unsigned int opcode = Opcode::Return;
};

struct Push {
    unsigned int opcode = Opcode::Push;
    unsigned int source_type;
    unsigned int source;
};

struct Pop {
    unsigned int opcode = Opcode::Pop;
    unsigned int destination;
};

}