#include "vm.h"

namespace pebble {

int unsigned_to_signed(unsigned int n) {
    return *(int*) &n;
}

VM::VM() {
    registers[RegisterA] = &a;
    registers[RegisterB] = &b;
    registers[RegisterIP] = &ip;
    registers[RegisterSP] = &sp;
    registers[RegisterFP] = &fp;
}

unsigned int VM::fetch() {
    return memory[++ip];
}

template<typename T>
T* VM::fetch_next_instruction() {
    auto i = reinterpret_cast<T*>(memory + ip);
    ip += sizeof(T) / 4;
    return i;
}

void VM::push(unsigned int value) {
    memory[sp--] = value;

}

unsigned int VM::pop() {
    return memory[++sp];
}

#define ARITHMETIC_LOGIC_OPERATION_CASE(NAME, OPERATOR) \
case Opcode::NAME: { \
    auto i = fetch_next_instruction<Instruction::NAME>(); \
    assert(i->destination < NumRegisters); \
    if (i->source_type) { \
        assert(i->source < NumRegisters); \
        *registers[i->destination] = *registers[i->destination] OPERATOR *registers[i->source]; \
    } else { \
        *registers[i->destination] = *registers[i->destination] OPERATOR i->source; \
    } \
    break; \
}

void VM::execute(unsigned int instruction) {
    switch (instruction) {
        case Opcode::Load: {
            auto i = fetch_next_instruction<Instruction::Load>();
            assert(i->destination < NumRegisters);

            switch (i->source_mode) {
                case Opcode::AddressingModeAddress:
                    *registers[i->destination] = memory[i->source];
                    break;
                case Opcode::AddressingModeFramePointerOffset: {
                    auto offset = unsigned_to_signed(i->source);
                    int address = fp + offset;
                    *registers[i->destination] = memory[address];
                    break;
                }
                default:
                    std::cerr << "vm: invalid addressing mode " << i->source_mode << "\n";
            }

            break;
        }

        case Opcode::Store: {
            auto i = fetch_next_instruction<Instruction::Store>();
            assert(i->source < NumRegisters);

            switch (i->destination_mode) {
                case Opcode::AddressingModeAddress:
                    memory[i->destination] = i->source;
                    break;
                case Opcode::AddressingModeFramePointerOffset: {
                    auto offset = unsigned_to_signed(i->destination);
                    int address = fp + offset;
                    memory[address] = i->source;
                    break;
                }
                default:
                    std::cerr << "vm: invalid addressing mode " << i->destination_mode << "\n";
            }

            break;
        }

        case Opcode::Move: {
            auto i = fetch_next_instruction<Instruction::Move>();
            assert(i->destination < NumRegisters);

            // 1 == register
            if (i->source_type) {
                assert(i->source < NumRegisters);
                *registers[i->destination] = *registers[i->source];
            } else {
                *registers[i->destination] = i->source;
            }

            break;
        }

        ARITHMETIC_LOGIC_OPERATION_CASE(Add, +)
        ARITHMETIC_LOGIC_OPERATION_CASE(Subtract, -)
        ARITHMETIC_LOGIC_OPERATION_CASE(Multiply, *)
        ARITHMETIC_LOGIC_OPERATION_CASE(Divide, /)
        ARITHMETIC_LOGIC_OPERATION_CASE(Modulo, %)
        ARITHMETIC_LOGIC_OPERATION_CASE(And, &)
        ARITHMETIC_LOGIC_OPERATION_CASE(Or, |)
        ARITHMETIC_LOGIC_OPERATION_CASE(ShiftLeft, <<)
        ARITHMETIC_LOGIC_OPERATION_CASE(ShiftRight, >>)
        ARITHMETIC_LOGIC_OPERATION_CASE(GreaterThan, >)
        ARITHMETIC_LOGIC_OPERATION_CASE(GreaterThanOrEqualTo, >=)
        ARITHMETIC_LOGIC_OPERATION_CASE(LessThan, <)
        ARITHMETIC_LOGIC_OPERATION_CASE(LessThanOrEqualTo, <=)
        ARITHMETIC_LOGIC_OPERATION_CASE(EqualTo, ==)
        ARITHMETIC_LOGIC_OPERATION_CASE(NotEqualTo, !=)

        case Opcode::Not: {
            auto i = fetch_next_instruction<Instruction::Not>();
            assert(i->destination < NumRegisters);
            if (i->source_type) {
                assert(i->source < NumRegisters);
                *registers[i->destination] = ~(*registers[i->destination]);
            } else {
                *registers[i->destination] = ~i->source;
            }
            break;
        }

        case Opcode::Jump: {
            auto address = fetch();
            ip = address;
            break;
        }

        case Opcode::JumpIfZero: {
            auto address = fetch();
            if (a == 0) {
                ip = address;
            } else {
                ip++;
            }
            break;
        }

        case Opcode::JumpIfNonZero: {
            auto address = fetch();
            if (a != 0) {
                ip = address;
            } else {
                ip++;
            }
            break;
        }

        case Opcode::Push: {
            auto is_register = fetch();
            auto source = fetch();

            if (is_register) {
                push(*registers[source]);
            } else {
                push(source);
            }

            ip++;
            break;
        }

        case Opcode::Pop: {
            auto destination = fetch();
            assert(destination < NumRegisters);
            auto value = pop();
            *registers[destination] = value;
            ip++;
            break;
        }

        case Opcode::Call: {
            auto address = fetch();
            push(ip + 1);
            ip = address;
            break;
        }

        case Opcode::Return: {
            auto address = pop();
            ip = address;
            break;
        }

        default:
            std::cerr << "unknown instruction: " << instruction << "\n";
            assert(false);
    }
}


void VM::load(std::vector<unsigned int> instructions) {
    for (int i = 0; i < instructions.size(); i++) {
        memory[i] = instructions[i];
    }
}

void VM::run() {
    while (memory[ip] != Opcode::Halt) {
        execute(memory[ip]);
    }
};

}