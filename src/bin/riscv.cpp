#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"
#include "koopa.h"

//******************************************************************************
// 文件读入与输出变量区
// 输入流
TokenStream<GrammerToken> stream;
// 生成器
KoopaGenerator* generator;
//******************************************************************************

//******************************************************************************
// 解析过程中的全局变量区
// 当前作用域/基本块
Scope* curScope;
BasicBlock* curBlock;
//******************************************************************************

// Program ::= Lines;
// Lines ::= Line NewLines {Line NewLines};
// NewLines ::= NEWLINE {NEWLINE};
// Line ::= LABEL Instruction
//         |  LABEL { Directive }
//         |  Instruction
//         |  Directive { Directive };
// Instruction ::= OPERATOR [OPERAND] {"," OPERAND};
// Directive ::= PSEUDO {OPERAND};

struct Program_Struct;
struct Lines_Struct;
struct NewLines_Struct;
struct Line_Struct;
struct Instruction_Struct;
struct Directive_Struct;

struct NEWLINE_Struct;
struct LABEL_Struct;
struct OPERATOR_Struct;
struct OPERAND_Struct;
struct PSEUDO_Struct;

Program_Struct* Program_func();
Lines_Struct* Lines_func();
NewLines_Struct* NewLines_func();
Line_Struct* Line_func();
Instruction_Struct* Instruction_func();
Directive_Struct* Directive_func();
NEWLINE_Struct* NEWLINE_func();
LABEL_Struct* LABEL_func();
OPERATOR_Struct* OPERATOR_func();

struct Program_Struct {
    Lines_Struct* Lines;
    // ATTRIBUTES
};

struct Lines_Struct {
    std::vector<Line_Struct*> Line;
    std::vector<NewLines_Struct*> NewLines;
    // ATTRIBUTES
};

struct NewLines_Struct {
    std::vector<NEWLINE_Struct*> NEWLINE;
    // ATTRIBUTES
};

struct Line_Struct {
    LABEL_Struct* LABEL;
    Instruction_Struct* Instruction;
    std::vector<Directive_Struct*> Directive;
    // ATTRIBUTES
};

struct Instruction_Struct {
    OPERATOR_Struct* OPERATOR;
    std::vector<OPERAND_Struct*> OPERAND;
    // ATTRIBUTES
};

struct Directive_Struct {
    PSEUDO_Struct* PSEUDO;
    std::vector<OPERAND_Struct*> OPERAND;
    // ATTRIBUTES
};

struct NEWLINE_Struct {
    // ATTRIBUTES
};

struct LABEL_Struct {
    // ATTRIBUTES
};

struct OPERATOR_Struct {
    // ATTRIBUTES
};

struct OPERAND_Struct {
    // ATTRIBUTES
};

struct PSEUDO_Struct {
    // ATTRIBUTES
};

Program_Struct* Program_func() {
    return nullptr;
}
Lines_Struct* Lines_func() {

    return nullptr;
}
NewLines_Struct* NewLines_func() {

    return nullptr;
}
Line_Struct* Line_func() {

    return nullptr;
}
Instruction_Struct* Instruction_func() {

    return nullptr;
}
Directive_Struct* Directive_func() {

    return nullptr;
}
NEWLINE_Struct* NEWLINE_func() {

    return nullptr;
}
LABEL_Struct* LABEL_func() {

    return nullptr;
}
OPERATOR_Struct* OPERATOR_func() {

    return nullptr;
}

int main(int argc, char *argv[]) {
    std::string project_dir = PROJECT_ROOT_DIR;
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input-file>" << " " << "<output-file>" << std::endl;
        return 1;
    }

    std::string file_name = (project_dir + "test/pipeline/" + argv[1] + ".riscv");
    std::ofstream fout(project_dir + "test/pipeline/" + argv[2] + ".exe");

    stream.LoadFile(file_name);

    // CompUnits_func();

    return 0;
}
