#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"
#include "riscv.h"
#include "elf.h"

TokenStream<GrammerToken> stream;

// Program ::= Lines;
// Lines ::= Line NewLines {Line NewLines};
// NewLines ::= NEWLINE {NEWLINE};
// Line ::= LABEL Instruction
//         |  LABEL { Directive }
//         |  Instruction
//         |  Directive { Directive };
// Instruction ::= OPERATOR [OPERAND] {"," OPERAND};
// Directive ::= PSEUDO {OPERAND};


int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");

    stream.LoadFile(file_name);

    // CompUnits_func();

    return 0;
}
