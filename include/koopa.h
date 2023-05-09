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