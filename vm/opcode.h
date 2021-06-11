#pragma once

#include <unordered_map>

namespace pebble::Opcode {

enum Opcode {
    Halt,
    // memory
    Load,
    Store,
    // registers
    Move,
    // arithmetic / logic
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    And,
    Or,
    Not,
    ShiftLeft,
    ShiftRight,
    GreaterThan,
    GreaterThanOrEqualTo,
    LessThan,
    LessThanOrEqualTo,
    EqualTo,
    NotEqualTo,
    // jumps
    Jump,
    JumpIfZero,
    JumpIfNonZero,
    // stack
    Push,
    Pop,
    Call,
    Return
};

enum {
    AddressingModeAddress,
    AddressingModeFramePointerOffset
};

}