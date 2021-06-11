#include "token.h"

namespace pebble {

std::string get_token_type_string(TokenType type) {
    switch (type) {
        case TokenType::Instruction:
            return "Instruction";
        case TokenType::Register:
            return "Register";
        case TokenType::Integer:
            return "Integer";
        case TokenType::Comma:
            return "Comma";
        case TokenType::EndOfFile:
            return "EndOfFile";
        case TokenType::Label:
            return "Label";
        case TokenType::LabelDefinition:
            return "LabelDefinition";
        case TokenType::BracketLeft:
            return "BracketLeft";
        case TokenType::BracketRight:
            return "BracketRight";
    }

    assert(false);
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
    os << "{ type: " << get_token_type_string(t.type) << ", value: \"" << t.value << "\" }";
    return os;
}

}
