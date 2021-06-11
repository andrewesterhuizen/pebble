#include "assembler.h"

namespace pebble {

template<typename T>
void Assembler::add_instruction(T instruction) {
    auto parts = reinterpret_cast<unsigned int*>(&instruction);
    unsigned int width = sizeof(T) / 4;

    for (size_t i = 0; i < width; i++) {
        instructions.push_back(parts[i]);
    }
}

unsigned int signed_to_unsigned(int n) {
    return *(unsigned int*) &n;
}

unsigned int Assembler::get_fp_offset() {
    unsigned int offset = 0;

    if (tokens[token_index + 1].type == TokenType::BracketLeft) {
        expect(TokenType::BracketLeft);
        auto offset_token = expect(TokenType::Integer);
        expect(TokenType::BracketRight);

        if (offset_token.value[0] == '-') {
            auto string_value = offset_token.value.substr(1);
            auto n = atoi(string_value.data());
            offset = n * -1;
        } else {
            auto n = atoi(offset_token.value.data());
            offset = n;
        }
    }

    return offset;
}

unsigned int Assembler::get_register_index(std::string name) {
    auto reg = register_by_name_lookup.find(name);
    if (reg != register_by_name_lookup.end()) {
        return reg->second;
    }

    std::cerr << "assembler: invalid register \"" << name << "\"\n";
    assert(false);
}

InstructionType Assembler::get_instruction(std::string name) {
    if (auto i = instruction_lookup.find(name); i != instruction_lookup.end()) {
        return instruction_lookup[name];
    }

    std::cerr << "assembler: invalid instruction \"" << name << "\"\n";
    assert(false);
}

Token Assembler::next_token() {
    return tokens[++token_index];
}

Token Assembler::expect(TokenType type) {
    auto next_token = tokens[++token_index];
    if (next_token.type != type) {
        std::cerr << "assembler: unexpected token " << next_token.value << "\n";
        assert(false);
    }
    return next_token;
}

unsigned int Assembler::get_label_address(std::string label) {
    auto address_entry = labels.find(label);
    assert(address_entry != labels.end());
    return address_entry->second;
}

void Assembler::get_labels() {
    unsigned int current_address = 0;
    auto current_token = tokens[token_index];

    while (current_token.type != TokenType::EndOfFile) {
        switch (current_token.type) {
            case TokenType::LabelDefinition: {
                labels[current_token.value] = current_address;
                break;
            }
            case TokenType::Instruction: {
                auto instruction = get_instruction(current_token.value);
                auto width_entry = instruction_width_lookup.find(instruction);
                assert(width_entry != instruction_width_lookup.end());
                current_address += instruction_width_lookup.find(instruction)->second;
                break;
            }
            case TokenType::Label:
            case TokenType::Register:
            case TokenType::Integer:
            case TokenType::Comma:
            case TokenType::BracketLeft:
            case TokenType::BracketRight:
            case TokenType::EndOfFile:
                // ignore
                break;
        }
        current_token = next_token();
    }

    token_index = 0;
}


#define ARITHMETIC_LOGIC_ASSEMBLER_CASE(NAME) \
case InstructionType::NAME: { \
    auto destination_token = expect(TokenType::Register); \
    auto destination = get_register_index(destination_token.value); \
    expect(TokenType::Comma); \
    auto source_token = next_token(); \
    assert(source_token.type == TokenType::Register || source_token.type == TokenType::Integer); \
    auto is_register = source_token.type == TokenType::Register; \
    unsigned int source; \
    if (is_register) { \
        source = get_register_index(source_token.value); \
    } else { \
        source = atoi(source_token.value.data()); \
    } \
    add_instruction(Instruction::NAME{ \
            .destination = destination, \
            .source_type = is_register, \
            .source = source, \
    }); \
    break; \
}

std::vector<unsigned int> Assembler::run(std::string src) {
    tokens = lexer.get_tokens(src);
    get_labels();

    std::cout << "labels before\n";
    for(auto l : labels) {
        std::cout << l.first << " - " << l.second << "\n";
    }

    // add jump to entry point if it exists
    auto start_address_entry = labels.find("start");
    if (start_address_entry != labels.end()) {
        // adjust labels to account for jump
        auto jump_width = instruction_width_lookup.find(InstructionType::Jump);
        unsigned int w = jump_width->second;
        for (auto l : labels) {
            labels.at(l.first) = l.second + w;
        }

        add_instruction(Instruction::Jump{.address = start_address_entry->second});
    }

    std::cout << "labels after\n";
    for(auto l : labels) {
        std::cout << l.first << " - " << l.second << "\n";
    }


    auto current_token = tokens[token_index];

    while (current_token.type != TokenType::EndOfFile) {
        switch (current_token.type) {
            case TokenType::LabelDefinition: {
                break;
            }
            case TokenType::Instruction: {
                auto instruction = get_instruction(current_token.value);

                switch (instruction) {
                    case InstructionType::Halt: {
                        add_instruction(Instruction::Halt{});
                        break;
                    }
                    case InstructionType::Load: {
                        Instruction::Load instruction;

                        auto destination_token = expect(TokenType::Register);
                        instruction.destination = get_register_index(destination_token.value);

                        expect(TokenType::Comma);

                        auto source_token = next_token();

                        switch (source_token.type) {
                            case TokenType::Label: {
                                auto address_entry = labels.find(source_token.value);
                                assert(address_entry != labels.end());
                                instruction.source_mode = Opcode::AddressingModeAddress;
                                instruction.source = address_entry->second;
                                break;
                            }
                            case TokenType::Register: {
                                assert(source_token.value == "fp");
                                instruction.source_mode = Opcode::AddressingModeFramePointerOffset;
                                instruction.source = signed_to_unsigned(get_fp_offset());
                                break;
                            }
                            default:
                                std::cerr << "assembler: unexpected token " << source_token.value << "\n";
                                assert(false);
                        }

                        add_instruction(instruction);

                        break;
                    }

                    case InstructionType::Store: {
                        Instruction::Store instruction;
                        auto destination_token = next_token();

                        switch (destination_token.type) {
                            case TokenType::Label: {
                                auto address_entry = labels.find(destination_token.value);
                                assert(address_entry != labels.end());
                                instruction.destination_mode = Opcode::AddressingModeAddress;
                                instruction.destination = address_entry->second;
                                break;
                            }
                            case TokenType::Register: {
                                assert(destination_token.value == "fp");
                                instruction.destination_mode = Opcode::AddressingModeFramePointerOffset;
                                instruction.destination = signed_to_unsigned(get_fp_offset());
                                break;
                            }
                            default:
                                std::cerr << "assembler: unexpected token " << destination_token.value << "\n";
                                assert(false);
                        }

                        expect(TokenType::Comma);

                        auto source_token = expect(TokenType::Register);
                        instruction.source = get_register_index(source_token.value);

                        add_instruction(instruction);

                        break;
                    }

                    case InstructionType::Move: {
                        Instruction::Move instruction;

                        auto destination_token = expect(TokenType::Register);
                        expect(TokenType::Comma);
                        auto source_token = next_token();
                        assert(source_token.type == TokenType::Register || source_token.type == TokenType::Integer);

                        auto is_register = source_token.type == TokenType::Register;

                        instruction.destination = get_register_index(destination_token.value);
                        instruction.source_type = is_register;

                        if (is_register) {
                            instruction.source = get_register_index(source_token.value);
                        } else {
                            instruction.source = atoi(source_token.value.data());
                        }

                        add_instruction(instruction);

                        break;
                    }

                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Add)
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Subtract);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Multiply);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Divide);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Modulo);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(And);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Or);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(Not);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(ShiftLeft);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(ShiftRight);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(GreaterThan);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(GreaterThanOrEqualTo);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(LessThan);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(LessThanOrEqualTo);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(EqualTo);
                    ARITHMETIC_LOGIC_ASSEMBLER_CASE(NotEqualTo);

                    case InstructionType::Jump: {
                        auto label_token = expect(TokenType::Label);
                        auto address = get_label_address(label_token.value);
                        add_instruction(Instruction::Jump{.address = address});
                        break;
                    }

                    case InstructionType::JumpIfNonZero: {
                        auto label_token = expect(TokenType::Label);
                        auto address = get_label_address(label_token.value);
                        add_instruction(Instruction::JumpIfNonZero{.address = address});
                        break;
                    }

                    case InstructionType::JumpIfZero: {
                        auto label_token = expect(TokenType::Label);
                        auto address = get_label_address(label_token.value);
                        add_instruction(Instruction::JumpIfZero{.address = address});
                        break;
                    }

                    case InstructionType::Call: {
                        auto label_token = expect(TokenType::Label);
                        auto address_entry = labels.find(label_token.value);
                        assert(address_entry != labels.end());
                        add_instruction(Instruction::Call{.address = address_entry->second});
                        break;
                    }

                    case InstructionType::Push: {
                        auto source_token = next_token();
                        assert(source_token.type == TokenType::Register || source_token.type == TokenType::Integer);

                        auto is_register = source_token.type == TokenType::Register;

                        Instruction::Push instruction;
                        instruction.source_type = is_register;

                        if (is_register) {
                            instruction.source = get_register_index(source_token.value);
                        } else {
                            instruction.source = atoi(source_token.value.data());
                        }

                        add_instruction(instruction);
                        break;
                    }

                    case InstructionType::Pop: {
                        auto register_token = expect(TokenType::Register);
                        Instruction::Pop instruction;
                        instruction.destination = get_register_index(register_token.value);
                        add_instruction(instruction);
                        break;
                    }

                    case InstructionType::Return: {
                        add_instruction(Instruction::Return{});
                        break;
                    }
                }

                break;
            }

            case TokenType::Label:
            case TokenType::Register:
            case TokenType::Integer:
            case TokenType::Comma:
            case TokenType::BracketLeft:
            case TokenType::BracketRight:
            case TokenType::EndOfFile:
                std::cerr << "assembler: unexpected token " << current_token.value << "\n";
                assert(false);
        }

        current_token = next_token();
    }

    return instructions;
}

}
