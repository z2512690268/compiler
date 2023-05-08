#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"
#include "koopa.h"
#include "riscv.h"
#include "ir.h"

TokenStream<GrammerToken> stream;
KoopaGenerator* generator;
Scope* curScope;

// CompUnits ::= CompUnit {CompUnit};

// CompUnit  ::= FunDef;

// FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";

// Type ::= "i32";

// Block ::= SYMBOL ":" EndStatement ";";

// EndStatement ::= Return;

// Return ::= "ret" [Value];

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");

    stream.LoadFile(file_name);

    generator = new KoopaGenerator();
    curScope = &generator->global_scope;
    // CompUnits_func();

    return 0;
}