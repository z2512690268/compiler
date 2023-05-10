#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"
#include "koopa.h"
#include "backend/riscv_generator.h"

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


//******************************************************************************
// 函数和结构体前置声明
// 非终结符
struct CompUnits_Struct;
struct CompUnit_Struct;
struct FuncDef_Struct;
struct FuncType_Struct;
struct Block_Struct;
struct Stmt_Struct;
struct Number_Struct;
struct Integer_Struct;

// 终结符
struct OCT_INTEGER_Struct;
struct IDENT_Struct;
struct RESERVED_Struct;

// 语法树函数
// 非终结符
CompUnits_Struct* CompUnits_func();
CompUnit_Struct* CompUnit_func();
FuncDef_Struct* FuncDef_func();
FuncType_Struct* FuncType_func();
Block_Struct* Block_func(std::string block_name = "");
Stmt_Struct* Stmt_func();
Number_Struct* Number_func();
Integer_Struct* Integer_func();
// 终结符
OCT_INTEGER_Struct* OCT_INTEGER_func();
IDENT_Struct* IDENT_func();
RESERVED_Struct* RESERVED_func();
//******************************************************************************




//******************************************************************************
// 带属性语法树节点
// 各个类型的结构体
struct SysyGramStruct {
    CompUnits_Struct* CompUnits;
    // ATTRIBUTES
};

struct CompUnits_Struct {
    std::vector<CompUnit_Struct*> CompUnit;
    // ATTRIBUTES
};

struct CompUnit_Struct {
    // FUNCDEF
    FuncDef_Struct* FuncDef;
    // ATTRIBUTES
};

struct FuncDef_Struct {
    Block_Struct* Block;

    // ATTRIBUTES
    KoopaVarType type;
    std::string funcName;
    FuncType_Struct* funcRetType;
};

struct FuncType_Struct {
    KoopaVarType type;
};

struct Block_Struct {
    std::vector<Stmt_Struct*> Stmt;

    // ATTRIBUTES
    BasicBlock* block;
};

struct Stmt_Struct {
    // RETURN
    Number_Struct* Number;

    // ATTRIBUTES
};

struct Number_Struct {
    // INTEGER
    Integer_Struct* Integer;
    // ATTRIBUTES
    KoopaVarValue value;
};

struct Integer_Struct {
    OCT_INTEGER_Struct* OCT_INTEGER;
    // ATTRIBUTES
    int value;
};

struct IDENT_Struct {
    // ATTRIBUTES
    std::string identifer;
};

struct OCT_INTEGER_Struct {
    // ATTRIBUTES
    int value;
};

struct RESERVED_Struct {
    // ATTRIBUTES
    std::string reserved;
};
//******************************************************************************




//******************************************************************************
// 建树与解析函数
// CompUnits      ::= CompUnit {CompUnit};
CompUnits_Struct* CompUnits_func() {
    CompUnits_Struct* compUnits_ptr = new CompUnits_Struct();
    std::cout << "CompUnits_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    for(auto rule : curToken.rule) {
        if(rule == "CompUnit") {
            CompUnit_Struct* compUnit_ptr = CompUnit_func();
            compUnits_ptr->CompUnit.push_back(compUnit_ptr);
        }
    }
    return compUnits_ptr;
}

// CompUnit       ::= FuncDef;
CompUnit_Struct* CompUnit_func() {
    CompUnit_Struct* compUnit_ptr = new CompUnit_Struct();
    std::cout << "CompUnit_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "FuncDef") {
        FuncDef_Struct* funcDef_ptr = FuncDef_func();
        compUnit_ptr->FuncDef = funcDef_ptr;
        return compUnit_ptr;
    }

    return compUnit_ptr; 
}


// FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncDef_Struct* FuncDef_func() {
    FuncDef_Struct* funcDef_ptr = new FuncDef_Struct();
    std::cout << "FuncDef_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    // 创建作用域
    Scope* func_scope = new Scope(curScope);
    curScope = func_scope;
    generator->func_scopes.push_back(func_scope);

    // 创建函数
    // FuncDef   ::= FuncType IDENT "(" ")" Block;
    if(curToken.rule[0] == "FuncType") {
        FuncType_Struct* funcType_ptr = FuncType_func();
        funcDef_ptr->funcRetType = funcType_ptr;

        IDENT_Struct* ident_ptr = IDENT_func();
        funcDef_ptr->funcName = ident_ptr->identifer;

        // "(" ")"
        RESERVED_func();
        RESERVED_func();

        Block_Struct* block_ptr = Block_func(funcDef_ptr->funcName);
        funcDef_ptr->Block = block_ptr;
    }

    // 退出作用域
    func_scope->func_name = funcDef_ptr->funcName;
    func_scope->func_ret_type = funcDef_ptr->funcRetType->type;
    func_scope->basicBlocks.push_back(funcDef_ptr->Block->block);
    curScope = curScope->parent;
    return funcDef_ptr;
}

// FuncType  ::= "int";
FuncType_Struct* FuncType_func() {
    FuncType_Struct* funcType_ptr = new FuncType_Struct();
    std::cout << "FuncType_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    funcType_ptr->type = KOOPA_INT32;
    // "int"
    RESERVED_func();
    return funcType_ptr;
}


// Block     ::= "{" Stmt "}";
Block_Struct* Block_func(std::string block_name) {
    Block_Struct* block_ptr = new Block_Struct();
    std::cout << "Block_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);

    // 创建基本块
    BasicBlock* block = new BasicBlock();
    block->parent = curBlock;
    std::string name = curScope->GetUniqueName("%entry");
    block->label = name;
    SymbolItem* block_item = new SymbolItem();
    block_item->InitLabelSymbol(block);
    curScope->AddSymbol(name, block_item);
    curBlock = block;

    // 创建语句
    // 只有一句语句
    RESERVED_func();    //{
    Stmt_Struct* stmt_ptr = Stmt_func();
    RESERVED_func();    //}
    block_ptr->block = block;
    curBlock = curBlock->parent;
    return block_ptr;
}

// Stmt      ::= "return" Number ";";
Stmt_Struct* Stmt_func() {
    Stmt_Struct* stmt_ptr = new Stmt_Struct();
    std::cout << "Stmt_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "\"return\"") {
        RESERVED_func();    //return
        Number_Struct* number_ptr = Number_func();
        stmt_ptr->Number = number_ptr;
        RESERVED_func();    //;

        std::string ret_var_name = curScope->GetUniqueName("%return");
        SymbolItem* ret_var = new SymbolItem();
        ret_var->InitVarSymbol(KOOPA_INT32);
        // 添加变量
        curScope->AddSymbol(ret_var_name, ret_var);
        // 赋值语句
        Statement* assign = new Statement();
        assign->InitOperationStatement("add", "0", std::to_string(stmt_ptr->Number->value.INT32), ret_var_name);
        curBlock->statements.push_back(assign);

        // 返回语句
        Statement* statement = new Statement();
        statement->InitReturnStatement(ret_var_name);
        curBlock->statements.push_back(statement);
    }

    return stmt_ptr;
}

// Number    ::= Integer;
Number_Struct* Number_func() {
    Number_Struct* number_ptr = new Number_Struct();
    std::cout << "Number_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "Integer") {
        Integer_Struct* integer_ptr = Integer_func();
        number_ptr->Integer = integer_ptr;
        number_ptr->value.INT32 = integer_ptr->value;
    }
    return number_ptr;
}

// Integer   ::= OCT_INTEGER;
Integer_Struct* Integer_func() {
    Integer_Struct* integer_ptr = new Integer_Struct();
    std::cout << "Integer_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "OCT_INTEGER") {
        OCT_INTEGER_Struct* octInteger_ptr = OCT_INTEGER_func();
        integer_ptr->OCT_INTEGER = octInteger_ptr;
        integer_ptr->value = octInteger_ptr->value;
    }
    return integer_ptr;
}

OCT_INTEGER_Struct* OCT_INTEGER_func() {
    OCT_INTEGER_Struct* octInteger_ptr = new OCT_INTEGER_Struct();
    std::cout << "OCT_INTEGER_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "OCT_INTEGER") {
        octInteger_ptr->value = std::stoi(curToken.token, nullptr, 8);
    }
    return octInteger_ptr;
}

IDENT_Struct* IDENT_func() {
    IDENT_Struct* ident_ptr = new IDENT_Struct();
    std::cout << "IDENT_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    ident_ptr->identifer = "@" + curToken.rule[0].substr(1, curToken.rule[0].size() - 2);

    return ident_ptr;
}

RESERVED_Struct* RESERVED_func() {
    RESERVED_Struct* reserved_ptr = new RESERVED_Struct();
    std::cout << "RESERVED_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    reserved_ptr->reserved = curToken.rule[0];

    return reserved_ptr;
}
//******************************************************************************




//******************************************************************************
// 主函数
int main(int argc, char* argv[]) {
    std::string project_dir = PROJECT_ROOT_DIR;
    if(argc < 3) {
        std::cout << "Usage: " << argv[0] << " " << "<koopa/riscv>" << "<input-file>" << " " << "<output-file>" << std::endl;
        exit(1);
    }

    if(std::string(argv[1]) == "koopa"){
        generator = new KoopaGenerator();
    } else if(std::string(argv[1]) == "riscv") {
        generator = new RiscvGenerator();
    } else {
        std::cerr << "Unknown target: " << argv[1] << std::endl;
        exit(1);
    }

    std::string file_name = (project_dir + "test/pipeline/" + argv[2] + ".gram");
    std::ofstream fout(project_dir + "test/pipeline/" + argv[3] + "." + std::string(argv[1]));

    stream.LoadFile(file_name);
    curScope = &generator->global_scope;
    curBlock = nullptr;

    CompUnits_func();

    std::cout << std::endl << std::endl;

    std::string output = generator->GenerateCode();
    std::cout << output << std::endl;
    fout << output << std::endl;

    return 0;
}
//******************************************************************************
