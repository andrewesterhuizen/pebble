#pragma once

#include <string>
#include <vector>
#include <set>

#include "token.h"

namespace pebble {

class Lexer {
    std::vector<Token> tokens;

    std::string source;
    int index;

    std::set<std::string> instruction_names = {
            "halt",
            "load",
            "store",
            "mov",
            "add",
            "sub",
            "mul",
            "div",
            "mod",
            "and",
            "or",
            "shl",
            "shr",
            "eq",
            "jump",
            "jumpz",
            "jumpnz",
            "push",
            "pop",
            "call",
            "ret",
    };

    std::string get_text_until_delimiter();

public:
    std::vector<Token> get_tokens(std::string src);
};

}