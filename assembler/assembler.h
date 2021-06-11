#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

#include "lexer.h"
#include "../vm/opcode.h"
#include "../vm/instruction.h"
#include "../vm/vm.h"

namespace pebble {

enum class InstructionType {
    Halt,
    Load,
    Store,
    Move,
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
    Jump,
    JumpIfZero,
    JumpIfNonZero,
    Push,
    Pop,
    Call,
    Return,
};

class Assembler {
    Lexer lexer;
    std::vector<Token> tokens;
    unsigned int token_index = 0;

    std::unordered_map<std::string, Register> register_by_name_lookup = {
            {"a",  RegisterA},
            {"b",  RegisterB},
            {"ip", RegisterIP},
            {"sp", RegisterSP},
            {"fp", RegisterFP},
    };

    std::unordered_map<std::string, InstructionType> instruction_lookup = {
            {"halt",   InstructionType::Halt},
            {"load",   InstructionType::Load},
            {"store",  InstructionType::Store},
            {"mov",    InstructionType::Move},
            {"add",    InstructionType::Add},
            {"sub",    InstructionType::Subtract},
            {"mul",    InstructionType::Multiply},
            {"div",    InstructionType::Divide},
            {"mod",    InstructionType::Modulo},
            {"and",    InstructionType::And},
            {"or",     InstructionType::Or},
            {"shl",    InstructionType::ShiftLeft},
            {"shr",    InstructionType::ShiftRight},
            {"eq",     InstructionType::EqualTo},
            {"jump",   InstructionType::Jump},
            {"jumpz",  InstructionType::JumpIfZero},
            {"jumpnz", InstructionType::JumpIfNonZero},
            {"push",   InstructionType::Push},
            {"pop",    InstructionType::Pop},
            {"call",   InstructionType::Call},
            {"ret",    InstructionType::Return},
    };

    std::unordered_map<InstructionType, Opcode::Opcode> basic_instruction_lookup = {
            {InstructionType::Halt,       Opcode::Halt},
            {InstructionType::Add,        Opcode::Add},
            {InstructionType::Subtract,   Opcode::Subtract},
            {InstructionType::Multiply,   Opcode::Multiply},
            {InstructionType::Divide,     Opcode::Divide},
            {InstructionType::Modulo,     Opcode::Modulo},
            {InstructionType::And,        Opcode::And},
            {InstructionType::Or,         Opcode::Or},
            {InstructionType::ShiftLeft,  Opcode::ShiftLeft},
            {InstructionType::ShiftRight, Opcode::ShiftRight},
            {InstructionType::EqualTo,    Opcode::EqualTo},
            {InstructionType::Push,       Opcode::Push},
            {InstructionType::Pop,        Opcode::Pop},
            {InstructionType::Return,     Opcode::Return}
    };

    std::unordered_map<InstructionType, unsigned int> instruction_width_lookup = {
            {InstructionType::Halt,                 sizeof(Instruction::Halt) / 4},
            {InstructionType::Load,                 sizeof(Instruction::Load) / 4},
            {InstructionType::Store,                sizeof(Instruction::Store) / 4},
            {InstructionType::Move,                 sizeof(Instruction::Move) / 4},
            {InstructionType::Add,                  sizeof(Instruction::Add) / 4},
            {InstructionType::Subtract,             sizeof(Instruction::Subtract) / 4},
            {InstructionType::Multiply,             sizeof(Instruction::Multiply) / 4},
            {InstructionType::Divide,               sizeof(Instruction::Divide) / 4},
            {InstructionType::Modulo,               sizeof(Instruction::Modulo) / 4},
            {InstructionType::And,                  sizeof(Instruction::And) / 4},
            {InstructionType::Or,                   sizeof(Instruction::Or) / 4},
            {InstructionType::Not,                  sizeof(Instruction::Not) / 4},
            {InstructionType::ShiftLeft,            sizeof(Instruction::ShiftLeft) / 4},
            {InstructionType::ShiftRight,           sizeof(Instruction::ShiftRight) / 4},
            {InstructionType::GreaterThan,          sizeof(Instruction::GreaterThan) / 4},
            {InstructionType::GreaterThanOrEqualTo, sizeof(Instruction::GreaterThanOrEqualTo) / 4},
            {InstructionType::LessThan,             sizeof(Instruction::LessThan) / 4},
            {InstructionType::LessThanOrEqualTo,    sizeof(Instruction::LessThanOrEqualTo) / 4},
            {InstructionType::EqualTo,              sizeof(Instruction::EqualTo) / 4},
            {InstructionType::NotEqualTo,           sizeof(Instruction::NotEqualTo) / 4},
            {InstructionType::Jump,                 sizeof(Instruction::Jump) / 4},
            {InstructionType::JumpIfZero,           sizeof(Instruction::JumpIfZero) / 4},
            {InstructionType::JumpIfNonZero,        sizeof(Instruction::JumpIfNonZero) / 4},
            {InstructionType::Push,                 sizeof(Instruction::Push) / 4},
            {InstructionType::Pop,                  sizeof(Instruction::Pop) / 4},
            {InstructionType::Call,                 sizeof(Instruction::Call) / 4},
            {InstructionType::Return,               sizeof(Instruction::Return) / 4}
    };

    std::vector<unsigned int> instructions;
    std::unordered_map<std::string, unsigned int> labels;
    unsigned int get_fp_offset();
    unsigned int get_register_index(std::string name);
    InstructionType get_instruction(std::string name);
    Token next_token();
    Token expect(TokenType type);
    void get_labels();
    unsigned int get_label_address(std::string label);

    template<typename T>
    void add_instruction(T instruction);

public:
    std::vector<unsigned int> run(std::string source);
};

}


