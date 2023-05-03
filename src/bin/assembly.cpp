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

TokenStream<GrammerToken> stream;
KoopaGenerator* generator;
Scope* curScope;


void ConstructSyntaxTree() {

}

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");

    stream.LoadFile(file_name);

    generator = new KoopaGenerator();
    curScope = &generator->global_scope;
    // CompUnits_func();

    return 0;
}
