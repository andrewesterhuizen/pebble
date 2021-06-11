cmake_minimum_required(VERSION 3.17)
project(pebble)

set(CMAKE_CXX_STANDARD 20)

add_executable(pebble main.cpp vm/opcode.h vm/vm.cpp vm/vm.h assembler/assembler.cpp assembler/assembler.h assembler/lexer.cpp assembler/lexer.h assembler/token.h assembler/token.cpp vm/instruction.cpp vm/instruction.h)

