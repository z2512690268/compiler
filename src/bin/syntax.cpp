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
void Decl_func();
void ConstDecl_func();
void ConstDef_func();
void ConstInitVal_func();
void VarDecl_func();
void VarDef_func();
void VarDefLeft_func();
void InitVal_func();
void FuncDef_func();
void Type_func();
void FuncFParams_func();
void FuncFParam_func();
void Block_func();
void BlockItem_func();
void Stmt_func();
void ElseStmt_func();
void NoIfStmt_func();
void Exp_func();
void LVal_func();
void PrimaryExp_func();
void Number_func();
void UnaryExp_func();
void UnaryOp_func();
void FuncRParams_func();
void MulExp_func();
void AddExp_func();
void RelExp_func();
void EqExp_func();
void LAndExp_func();
void LOrExp_func();
void ConstExp_func();
void Integer_func();

std::unordered_map<std::string, std::function<void()>> func_map = {
    {"CompUnit", CompUnit_func},
    {"Decl", Decl_func},
    {"ConstDecl", ConstDecl_func},
    {"ConstDef", ConstDef_func},
    {"ConstInitVal", ConstInitVal_func},
    {"VarDecl", VarDecl_func},
    {"VarDef", VarDef_func},
    {"VarDefLeft", VarDefLeft_func},
    {"InitVal", InitVal_func},
    {"FuncDef", FuncDef_func},
    {"Type", Type_func},
    {"FuncFParams", FuncFParams_func},
    {"FuncFParam", FuncFParam_func},
    {"Block", Block_func},
    {"BlockItem", BlockItem_func},
    {"Stmt", Stmt_func},
    {"ElseStmt", ElseStmt_func},
    {"NoIfStmt", NoIfStmt_func},
    {"Exp", Exp_func},
    {"LVal", LVal_func},
    {"PrimaryExp", PrimaryExp_func},
    {"Number", Number_func},
    {"UnaryExp", UnaryExp_func},
    {"UnaryOp", UnaryOp_func},
    {"FuncRParams", FuncRParams_func},
    {"MulExp", MulExp_func},
    {"AddExp", AddExp_func},
    {"RelExp", RelExp_func},
    {"EqExp", EqExp_func},
    {"LAndExp", LAndExp_func},
    {"LOrExp", LOrExp_func},
    {"ConstExp", ConstExp_func},
    {"Integer", Integer_func}
};

TokenStream<GrammerToken> stream;
KoopaGenerator* generator;
Scope* curScope;

// CompUnit: CompUnit Decl 
// CompUnit: CompUnit FuncDef 
// CompUnit: Decl 
// CompUnit: FuncDef 
void CompUnit_func() {
    std::cout << "CompUnit" << std::endl;
    GrammerToken curToken; 
    stream.GetToken(curToken);
    if(curToken.rule[0] == "CompUnit") {
        CompUnit_func();
    } 
    if(curToken.rule[0] == "Decl" || curToken.rule[1] == "Decl") {
        Decl_func();
    } else if(curToken.rule[0] == "FuncDef" || curToken.rule[1] == "FuncDef") {
        FuncDef_func();
    } else {
        throw std::runtime_error("CompUnit_func: Invalid token");
    }
    return ;
}

// Decl: ConstDecl 
// Decl: VarDecl 
void Decl_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "ConstDecl") {
        ConstDecl_func();
    } else if(curToken.rule[0] == "VarDecl") {
        VarDecl_func();
    } else {
        throw std::runtime_error("Decl_func: Invalid token");
    }
    return ;
}

// ConstDecl: "const" Type ConstDef ConstDecl$1 ";" 
// ConstDecl: "const" Type ConstDef ";" 
// ConstDecl$1: ConstDecl$1 "," ConstDef 
// ConstDecl$1: "," ConstDef 
void ConstDecl_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);
}

// ConstDef: IDENT ConstDef$1 "=" ConstInitVal 
// ConstDef: IDENT "=" ConstInitVal 
// ConstDef$1: ConstDef$1 "[" ConstExp "]" 
// ConstDef$1: "[" ConstExp "]" 
void ConstDef_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}

// ConstInitVal: ConstExp 
// ConstInitVal: "{" ConstInitVal ConstInitVal$1 "}" 
// ConstInitVal: "{" ConstInitVal "}" 
// ConstInitVal: "{" "}" 
// ConstInitVal$1: ConstInitVal$1 "," ConstInitVal 
// ConstInitVal$1: "," ConstInitVal
void ConstInitVal_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

} 
// VarDecl: Type VarDef VarDecl$1 ";" 
// VarDecl: Type VarDef ";" 
// VarDecl$1: VarDecl$1 "," VarDef 
// VarDecl$1: "," VarDef 
void VarDecl_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// VarDef: VarDefLeft 
// VarDef: VarDefLeft "=" InitVal 
void VarDef_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// VarDefLeft: IDENT VarDefLeft$1 
// VarDefLeft: IDENT 
// VarDefLeft$1: VarDefLeft$1 "[" ConstExp "]" 
// VarDefLeft$1: "[" ConstExp "]" 
void VarDefLeft_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// InitVal: Exp 
// InitVal: "{" InitVal InitVal$1 "}" 
// InitVal: "{" InitVal "}" 
// InitVal: "{" "}" 
// InitVal$1: InitVal$1 "," InitVal 
// InitVal$1: "," InitVal 
void InitVal_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// FuncDef: Type IDENT "(" FuncFParams ")" Block 
// FuncDef: Type IDENT "(" ")" Block 
void FuncDef_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// Type: "void" 
// Type: "int" 
void Type_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// FuncFParams: FuncFParam FuncFParams$1 
// FuncFParams: FuncFParam 
// FuncFParams$1: FuncFParams$1 "," FuncFParam 
// FuncFParams$1: "," FuncFParam 
void FuncFParams_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// FuncFParam: Type IDENT "[" "]" FuncFParam$1 
// FuncFParam: Type IDENT "[" "]" 
// FuncFParam: Type IDENT 
// FuncFParam$1: FuncFParam$1 "[" ConstExp "]" 
// FuncFParam$1: "[" ConstExp "]" 
void FuncFParam_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// Block: "{" Block$1 "}" 
// Block: "{" "}" 
// Block$1: Block$1 BlockItem 
// Block$1: BlockItem 
void Block_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// BlockItem: Decl 
// BlockItem: Stmt 
void BlockItem_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// Stmt: "if" "(" Exp ")" ElseStmt "else" Stmt 
// Stmt: "if" "(" Exp ")" Stmt 
// Stmt: "while" "(" Exp ")" Stmt 
// Stmt: NoIfStmt 
void Stmt_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// ElseStmt: "if" "(" Exp ")" ElseStmt "else" ElseStmt 
// ElseStmt: "while" "(" Exp ")" ElseStmt 
// ElseStmt: NoIfStmt 
void ElseStmt_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// NoIfStmt: LVal "=" Exp ";" 
// NoIfStmt: Exp ";" 
// NoIfStmt: ";" 
// NoIfStmt: Block 
// NoIfStmt: "break" ";" 
// NoIfStmt: "continue" ";" 
// NoIfStmt: "return" Exp ";" 
// NoIfStmt: "return" ";" 
void NoIfStmt_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// Exp: LOrExp 
void Exp_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// LVal: IDENT LVal$1 
// LVal: IDENT 
// LVal$1: LVal$1 "[" Exp "]" 
// LVal$1: "[" Exp "]" 
void LVal_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// PrimaryExp: "(" Exp ")" 
// PrimaryExp: LVal 
// PrimaryExp: Number 
void PrimaryExp_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// Number: Integer 
void Number_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// UnaryExp: PrimaryExp 
// UnaryExp: IDENT "(" FuncRParams ")" 
// UnaryExp: IDENT "(" ")" 
// UnaryExp: UnaryOp UnaryExp 
void UnaryExp_func() {
    GrammerToken curToken;
    stream.GetToken(curToken);

}
// UnaryOp: "+" 
// UnaryOp: "-" 
// UnaryOp: "!" 
// FuncRParams: Exp FuncRParams$1 
// FuncRParams: Exp 
// FuncRParams$1: FuncRParams$1 "," Exp 
// FuncRParams$1: "," Exp 
// MulExp: UnaryExp 
// MulExp: MulExp "*" UnaryExp 
// MulExp: MulExp "/" UnaryExp 
// MulExp: MulExp "%" UnaryExp 
// AddExp: MulExp 
// AddExp: AddExp "+" MulExp 
// AddExp: AddExp "-" MulExp 
// RelExp: AddExp 
// RelExp: RelExp "<" AddExp 
// RelExp: RelExp ">" AddExp 
// RelExp: RelExp "<=" AddExp 
// RelExp: RelExp ">=" AddExp 
// EqExp: RelExp 
// EqExp: EqExp "==" RelExp 
// EqExp: EqExp "!=" RelExp 
// LAndExp: EqExp 
// LAndExp: LAndExp "&&" EqExp 
// LOrExp: LAndExp 
// LOrExp: LOrExp "||" LAndExp 
// ConstExp: Exp 
// Integer: DEC_INTEGER 
// Integer: OCT_INTEGER 
// Integer: HEX_INTEGER 
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
