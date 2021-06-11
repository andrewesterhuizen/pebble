#include <iostream>

#include "lexer.h"

namespace pebble {

std::string Lexer::get_text_until_delimiter() {
    std::string out;

    while (index < source.size() && source[index] != ' ' && source[index] != '\n' && source[index] != ',' &&
           source[index] != ':' && source[index] != '-' && source[index] != '[' && source[index] != ']') {
        out += source[index];
        index++;
    }

    return out;
}

std::vector<Token> Lexer::get_tokens(std::string src) {
    source = src;

    while (index < source.size()) {
        auto c = source[index];
        if (c == '&') {
            index++;
            auto val = get_text_until_delimiter();

            TokenType type = TokenType::Label;

            if (source[index] == ':') {
                index++;
                type = TokenType::LabelDefinition;
            }

            tokens.push_back(Token{.type = type, .value = val});
        } else if (isalpha(c)) {
            auto val = get_text_until_delimiter();

            if (val == "a" || val == "b" || val == "ip" || val == "sp" || val == "fp") {
                tokens.push_back(Token{.type = TokenType::Register, .value = val});
            } else if (instruction_names.find(val) != instruction_names.end()) {
                tokens.push_back(Token{.type = TokenType::Instruction, .value = val});
            } else {
                std::cerr << "lexer: unknown instruction \"" << val << "\"\n";
                assert(false);
            }
            index--;
        } else if (isnumber(c)) {
            auto val = get_text_until_delimiter();
            tokens.push_back(Token{.type = TokenType::Integer, .value = val});
            index--;
        } else if (c == '-' || c == '+') {
            index++;
            assert(isnumber(source[index]));

            std::string n;
            if (c == '-') {
                n += '-';
            }
            while (isnumber(source[index])) {
                n += source[index];
                index++;
            }

            tokens.push_back(Token{.type = TokenType::Integer, .value = n});
            index--;

        } else {
            switch (c) {
                case ',':
                    tokens.push_back(Token{.type = TokenType::Comma, .value = ","});
                    break;
                case '[':
                    tokens.push_back(Token{.type = TokenType::BracketLeft, .value = "["});
                    break;
                case ']':
                    tokens.push_back(Token{.type = TokenType::BracketRight, .value = "]"});
                    break;
            }
        }

        index++;
    }

    tokens.push_back(Token{.type = TokenType::EndOfFile, .value = ""});

    return tokens;
}

}
