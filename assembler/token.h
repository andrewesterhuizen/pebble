#pragma once

#include <ostream>

namespace pebble {

enum class TokenType {
    Label,
    LabelDefinition,
    Instruction,
    Register,
    Integer,
    Comma,
    BracketLeft,
    BracketRight,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
};

std::ostream &operator<<(std::ostream &os, const Token &t);

}