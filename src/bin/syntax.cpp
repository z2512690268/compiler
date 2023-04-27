#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "defs.h"
#include "stream.h"
#include "koopa.h"

void CompUnit_func();
void FuncDef_func();
void FuncType_func();
void Block_func();
void Stmt_func();
void Number_func();
void Integer_func();


std::unordered_map<std::string, std::function<void()>> func_map = {
    {"CompUnit", CompUnit_func},
    {"FuncDef", FuncDef_func},
    {"FuncType", FuncType_func},
    {"Block", Block_func},
    {"Stmt", Stmt_func},
    {"Number", Number_func},
    {"Integer", Integer_func}
};

TokenStream<GrammerToken> stream;
KoopaGenerator* generator;
Scope* curScope;

// CompUnit: FuncDef 
void CompUnit_func() {
    std::cout << "CompUnit" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "FuncDef") {
        FuncDef_func();
    } else {
        throw std::runtime_error("CompUnit_func: Invalid token");
    }
    return ;
}

// FuncDef: FuncType IDENT "(" ")" Block 
void FuncDef_func() {
    std::cout << "FuncDef" << std::endl;
    GrammerToken curToken; 
    GrammerToken funcTypeToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "FuncType") {
        funcTypeToken = curToken;
        FuncType_func();
    } else {
        throw std::runtime_error("FuncDef_func: Invalid token");
    } 
    stream.GetToken(curToken);
    if(curToken.rule[1] == "IDENT") {
    } else {
    }
    return ;
}
// FuncType: "int" 
// Block: "{" Stmt "}" 
// Stmt: "return" Number ";" 
// Number: Integer 
// Integer: OCT_INTEGER 
// $START: CompUnit 



void ConstructSyntaxTree() {
    generator = new KoopaGenerator();
    curScope = &generator->global_scope;
    CompUnit_func();
}

int main() {
    std::string project_dir = PROJECT_ROOT_DIR;
    std::string file_name = (project_dir + "test/pipeline/maze.gram");

    stream.LoadFile(file_name);

    ConstructSyntaxTree();

    return 0;
}
