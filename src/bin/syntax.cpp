#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <any>
#include "defs.h"
#include "stream.h"
#include "koopa.h"
#include "ir.h"

// 输入流
TokenStream<GrammerToken> stream;
// 生成器
KoopaGenerator* generator;
// 当前作用域/基本块
Scope* curScope;
BasicBlock* curBlock;

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


// CompUnits      ::= CompUnit {CompUnit};
CompUnits_Struct* CompUnits_func() {
    std::cout << "CompUnits_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    CompUnits_Struct* compUnits_ptr = new CompUnits_Struct();
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
    std::cout << "CompUnit_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    CompUnit_Struct* compUnit_ptr = new CompUnit_Struct();
    if(curToken.rule[0] == "FuncDef") {
        FuncDef_Struct* funcDef_ptr = FuncDef_func();
        compUnit_ptr->FuncDef = funcDef_ptr;
        return compUnit_ptr;
    }

    return compUnit_ptr; 
}


// FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncDef_Struct* FuncDef_func() {
    std::cout << "FuncDef_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    // 创建作用域
    Scope* func_scope = new Scope(curScope);
    curScope = func_scope;
    generator->func_scopes.push_back(func_scope);

    // 创建函数
    FuncDef_Struct* funcDef_ptr = new FuncDef_Struct();
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
    std::cout << "FuncType_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    FuncType_Struct* funcType_ptr = new FuncType_Struct();
    funcType_ptr->type = KOOPA_INT32;
    // "int"
    RESERVED_func();
    return funcType_ptr;
}


// Block     ::= "{" Stmt "}";
Block_Struct* Block_func(std::string block_name) {
    std::cout << "Block_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);

    // 创建基本块
    BasicBlock* block = new BasicBlock();
    block->parent = curBlock;
    std::string name = "%" + (block_name == "" ? curScope->GetUniqueName("block") 
                                    : curScope->GetUniqueName(block_name + "_block"));
    block->label = name;
    SymbolItem* block_item = new SymbolItem();
    block_item->InitLabelSymbol(block);
    curScope->AddSymbol(name, block_item);
    curBlock = block;

    // 创建语句
    Block_Struct* block_ptr = new Block_Struct();
    // 只有一句语句
    RESERVED_func();    //{
    Stmt_Struct* stmt_ptr = Stmt_func();
    RESERVED_func();    //}
    block_ptr->block = block;
    return block_ptr;
}

// Stmt      ::= "return" Number ";";
Stmt_Struct* Stmt_func() {
    std::cout << "Stmt_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    Stmt_Struct* stmt_ptr = new Stmt_Struct();
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
    std::cout << "Number_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    Number_Struct* number_ptr = new Number_Struct();
    if(curToken.rule[0] == "Integer") {
        Integer_Struct* integer_ptr = Integer_func();
        number_ptr->Integer = integer_ptr;
        number_ptr->value.INT32 = integer_ptr->value;
    }
    return number_ptr;
}

// Integer   ::= OCT_INTEGER;
Integer_Struct* Integer_func() {
    std::cout << "Integer_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    Integer_Struct* integer_ptr = new Integer_Struct();
    if(curToken.rule[0] == "OCT_INTEGER") {
        OCT_INTEGER_Struct* octInteger_ptr = OCT_INTEGER_func();
        integer_ptr->OCT_INTEGER = octInteger_ptr;
        integer_ptr->value = octInteger_ptr->value;
    }
    return integer_ptr;
}

OCT_INTEGER_Struct* OCT_INTEGER_func() {
    std::cout << "OCT_INTEGER_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    OCT_INTEGER_Struct* octInteger_ptr = new OCT_INTEGER_Struct();
    if(curToken.rule[0] == "OCT_INTEGER") {
        octInteger_ptr->value = std::stoi(curToken.token, nullptr, 8);
    }
    return octInteger_ptr;
}

IDENT_Struct* IDENT_func() {
    std::cout << "IDENT_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    IDENT_Struct* ident_ptr = new IDENT_Struct();
    ident_ptr->identifer = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);

    return ident_ptr;
}

RESERVED_Struct* RESERVED_func() {
    std::cout << "RESERVED_func" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    RESERVED_Struct* reserved_ptr = new RESERVED_Struct();
    reserved_ptr->reserved = curToken.rule[0];

    return reserved_ptr;
}

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/sysy_t1.gram");
    stream.LoadFile(file_name);

    generator = new KoopaGenerator();
    curScope = &generator->global_scope;
    curBlock = nullptr;

    CompUnits_func();

    std::cout << std::endl << std::endl;

    std::cout << generator->GenerateCode() << std::endl;

    return 0;
}
