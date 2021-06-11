#include <iostream>
#include <fstream>

#include "assembler/assembler.h"
#include "vm/vm.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "no file name was provided";
        return 1;
    }

    std::string src;
    std::string line;
    auto entry_point_file_name = argv[1];
    std::ifstream entry_point_file(entry_point_file_name);

    if(entry_point_file.fail()) {
        std::cerr << "failed to open file \"" << entry_point_file_name << "\"";
        return 1;
    }

    // there must be a better way to do this
    if (entry_point_file.is_open()) {
        while (getline(entry_point_file, line)) {
            src += line + '\n';
        }
        entry_point_file.close();
    }

    pebble::VM vm;
    pebble::Assembler assembler;

    auto bytecode = assembler.run(src);

    vm.load(bytecode);
    vm.run();

    return 0;
}
