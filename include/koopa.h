#pragma once
#include <memory>
#include <vector>
#include <string>
#include "ir.h"

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

// 各个类型的结构体
struct KoopaGramStruct {
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