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
struct FunDef_Struct;
struct Type_Struct;
struct Block_Struct;
struct Statement_Struct;
struct SymbolDef_Struct;
struct BinaryExpr_Struct;
struct EndStatement_Struct;
struct Return_Struct;
struct Value_Struct;
// 终结符
struct SYMBOL_Struct;
struct INT_Struct;
struct BINARY_OP_Struct;
struct RESERVED_Struct;

// 语法树函数
// 非终结符
CompUnits_Struct* CompUnits_func();
CompUnit_Struct* CompUnit_func();
FunDef_Struct* FunDef_func();
Type_Struct* Type_func();
Block_Struct* Block_func();
Statement_Struct* Statement_func();
SymbolDef_Struct* SymbolDef_func();
BinaryExpr_Struct* BinaryExpr_func();
EndStatement_Struct* EndStatement_func();
Return_Struct* Return_func();
Value_Struct* Value_func();
// 终结符
SYMBOL_Struct* SYMBOL_func();
INT_Struct* INT_func();
BINARY_OP_Struct* BINARY_OP_func();
RESERVED_Struct* RESERVED_func();
//******************************************************************************

//******************************************************************************
// 带属性语法树节点
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
    FunDef_Struct* FunDef;
    // ATTRIBUTES
};

struct FunDef_Struct {
    SYMBOL_Struct* SYMBOL;
    Type_Struct* Type;
    std::vector<Block_Struct*> Block;
    // ATTRIBUTES
};

struct Type_Struct {
    KoopaVarType Type;
    // ATTRIBUTES
};

struct Block_Struct {
    SYMBOL_Struct* SYMBOL;
    std::vector<Statement_Struct*> Statement;
    EndStatement_Struct* EndStatement;
    // ATTRIBUTES
    BasicBlock* block;
};

struct Statement_Struct {
    SymbolDef_Struct* SymbolDef;
    // ATTRIBUTES
};

struct SymbolDef_Struct {
    SYMBOL_Struct* SYMBOL;
    BinaryExpr_Struct* BinaryExpr;
    // ATTRIBUTES
    std::string SymbolName;
};

struct BinaryExpr_Struct {
    BinaryExpr_Struct* BinaryExpr;
    BINARY_OP_Struct* BINARY_OP;
    Value_Struct* Value1;
    Value_Struct* Value2;
    // ATTRIBUTES
};

struct EndStatement_Struct {
    Return_Struct* Return;
    // ATTRIBUTES
};

struct Return_Struct {
    Value_Struct* Value;
    // ATTRIBUTES
};

struct Value_Struct {
    enum ValueType {
        SYMBOL_TYPE,
        INT_TYPE,
        UNDEF_TYPE
    };
    ValueType type;
    // SYMBOL_TYPE
    SYMBOL_Struct* SYMBOL;
    // INT_TYPE
    INT_Struct* INT;
    // ATTRIBUTES
};

struct SYMBOL_Struct {
    std::string SYMBOL;
    // ATTRIBUTES
};

struct INT_Struct {
    int INT;
    // ATTRIBUTES
};

struct BINARY_OP_Struct {
    std::string BINARY_OP;
    // ATTRIBUTES
};

struct RESERVED_Struct {
    std::string RESERVED;
    // ATTRIBUTES
};

//******************************************************************************

// CompUnits ::= CompUnit {CompUnit};
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

// CompUnit  ::= FunDef;
CompUnit_Struct* CompUnit_func() {
    CompUnit_Struct* compUnit_ptr = new CompUnit_Struct();
    std::cout << "CompUnit_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "FunDef") {
        compUnit_ptr->FunDef = FunDef_func();
        return compUnit_ptr;
    }

    return compUnit_ptr; 
}

// FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";
FunDef_Struct* FunDef_func() {
    FunDef_Struct* funDef_ptr = new FunDef_Struct();
    std::cout << "FunDef_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    // 创建作用域
    Scope* func_scope = new Scope(curScope);
    curScope = func_scope;
    generator->func_scopes.push_back(func_scope);

    int i = 0;
    RESERVED_func();    // fun
    funDef_ptr->SYMBOL = SYMBOL_func();
    RESERVED_func();    // (
    RESERVED_func();    // )
    // std::cout << "sda" << i << curToken.rule[4] << std::endl;
    i += 4;
    if(curToken.rule[i] == "\":\"") {
        RESERVED_func();    // :
        funDef_ptr->Type = Type_func();
        i += 2;
    }
    RESERVED_func();    // {
    i++;
    // std::cout << "reqw" << i << curToken.rule[i] << std::endl;
    while(curToken.rule[i] == "Block") {
        funDef_ptr->Block.push_back(Block_func());
        i++;
    }
    RESERVED_func();    // }

    // 退出作用域
    func_scope->func_name = funDef_ptr->SYMBOL->SYMBOL;
    func_scope->func_ret_type = funDef_ptr->Type->Type;
    for(auto& block : funDef_ptr->Block) {
        func_scope->basicBlocks.push_back(block->block);
    }

    return funDef_ptr;

}
// Type ::= "i32";
Type_Struct* Type_func() {
    Type_Struct* type_ptr = new Type_Struct();
    std::cout << "Type_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "\"i32\"") {
        RESERVED_func();    // i32
        type_ptr->Type = KOOPA_INT32;
        return type_ptr;
    }

    return type_ptr;
}

// Block ::= SYMBOL ":" {Statement ";"} EndStatement ";";
Block_Struct* Block_func() {
    Block_Struct* block_ptr = new Block_Struct();
    std::cout << "Block_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);


    int i = 0;
    std::cout << "sda" << i << curToken.rule[0] << std::endl;
    if(curToken.rule[i] == "SYMBOL") {
        block_ptr->SYMBOL = SYMBOL_func();
        // 创建基本块
        BasicBlock* block = new BasicBlock();
        block->parent = curBlock;
        curBlock = block;
        // 基本块命名
        block->label = block_ptr->SYMBOL->SYMBOL;
        SymbolItem* block_item = new SymbolItem();
        block_item->InitLabelSymbol(block);
        curScope->AddSymbol(block->label, block_item);

        RESERVED_func();    // :
        i += 2;
        while(curToken.rule[i] == "Statement") {
            block_ptr->Statement.push_back(Statement_func());
            std::cout << "sdaad" << std::endl;
            RESERVED_func();    // ;
            i += 2;
        }
        block_ptr->EndStatement = EndStatement_func();
        RESERVED_func();    // ;
    

        block_ptr->block = block;
        return block_ptr;
    }

    return block_ptr;
}

// Statement ::= SymbolDef;
Statement_Struct* Statement_func() {
    Statement_Struct* statement_ptr = new Statement_Struct();
    std::cout << "Statement_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "SymbolDef") {
        statement_ptr->SymbolDef = SymbolDef_func();
        std::cout << "eeew" << std::endl;
        return statement_ptr;
    }

    return statement_ptr;
}

// SymbolDef ::= SYMBOL "=" BinaryExpr;
SymbolDef_Struct* SymbolDef_func() {
    SymbolDef_Struct* symbolDef_ptr = new SymbolDef_Struct();
    std::cout << "SymbolDef_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "SYMBOL") {
        symbolDef_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func();    // =
        symbolDef_ptr->BinaryExpr = BinaryExpr_func();

        symbolDef_ptr->SymbolName = symbolDef_ptr->SYMBOL->SYMBOL;
        
        
        // 创建符号
        SymbolItem* symbol_item = new SymbolItem();
        symbol_item->InitVarSymbol(KOOPA_INT32);
        curScope->AddSymbol(symbolDef_ptr->SymbolName, symbol_item);
        std::cout << "OOO" << std::endl;

        Statement *statement = new Statement();
        std::string op = symbolDef_ptr->BinaryExpr->BINARY_OP->BINARY_OP;
        std::string value1 = std::to_string(symbolDef_ptr->BinaryExpr->Value1->INT->INT);
        std::string value2 = std::to_string(symbolDef_ptr->BinaryExpr->Value2->INT->INT);
        // std::cout << symbolDef_ptr->BinaryExpr->BINARY_OP->BINARY_OP << std::endl;
        // std::cout << symbolDef_ptr->BinaryExpr->Value1->SYMBOL->SYMBOL << std::endl;
        // std::cout << symbolDef_ptr->BinaryExpr->Value2->SYMBOL->SYMBOL << std::endl;
        // std::cout << symbolDef_ptr->SymbolName << std::endl;
        statement->InitOperationStatement(op,
                                value1, 
                                value2,
                                symbolDef_ptr->SymbolName);
        curBlock->statements.push_back(statement);
        return symbolDef_ptr;
    }

    return symbolDef_ptr;
}

// BinaryExpr ::= BINARY_OP Value "," Value;
BinaryExpr_Struct* BinaryExpr_func() {
    BinaryExpr_Struct* binaryExpr_ptr = new BinaryExpr_Struct();
    std::cout << "BinaryExpr_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "BINARY_OP") {
        binaryExpr_ptr->BINARY_OP = BINARY_OP_func();
        binaryExpr_ptr->Value1 = Value_func();
        RESERVED_func();    // ,
        binaryExpr_ptr->Value2 = Value_func();
        std::cout << "WWWW" << std::endl;
        return binaryExpr_ptr;
    }

    return binaryExpr_ptr;
}

// EndStatement ::= Return;
EndStatement_Struct* EndStatement_func() {
    EndStatement_Struct* endStatement_ptr = new EndStatement_Struct();
    std::cout << "EndStatement_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "Return") {
        endStatement_ptr->Return = Return_func();
        return endStatement_ptr;
    }

    return endStatement_ptr;
}

// Return ::= "ret" [Value];
Return_Struct* Return_func() {
    Return_Struct* return_ptr = new Return_Struct();
    std::cout << "Return_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    RESERVED_func();    // ret
    if(curToken.rule[1] == "Value") {
        return_ptr->Value = Value_func();
    }

    Statement* statement = new Statement();
    statement->InitReturnStatement(return_ptr->Value->SYMBOL->SYMBOL);
    curBlock->statements.push_back(statement);

    return return_ptr;

}

// Value ::= SYMBOL | INT | "undef";
Value_Struct* Value_func() {
    Value_Struct* value_ptr = new Value_Struct();
    std::cout << "Value_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.rule[0] == "SYMBOL") {
        value_ptr->type = Value_Struct::SYMBOL_TYPE;
        value_ptr->SYMBOL = SYMBOL_func();
        return value_ptr;
    }
    if(curToken.rule[0] == "INT") {
        value_ptr->type = Value_Struct::INT_TYPE;
        value_ptr->INT = INT_func();
        return value_ptr;
    }
    if(curToken.rule[0] == "\"undef\"") {
        value_ptr->type = Value_Struct::UNDEF_TYPE;
        return value_ptr;
    }

    return value_ptr;
}

// SYMBOL
SYMBOL_Struct* SYMBOL_func() {
    SYMBOL_Struct* symbol_ptr = new SYMBOL_Struct();
    std::cout << "SYMBOL_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.token == "SYMBOL") {
        symbol_ptr->SYMBOL = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        return symbol_ptr;
    }

    return symbol_ptr;
}

// INT
INT_Struct* INT_func() {
    INT_Struct* int_ptr = new INT_Struct();
    std::cout << "INT_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    std::cout << "sdada" << curToken.rule[0] << std::endl;
    if(curToken.token == "INT") {
        std::cout << "sdada" << curToken.rule[0] << std::endl;
        std::string int_num = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        int_ptr->INT = std::stoi(int_num, nullptr, 10);
        return int_ptr;
    }

    return int_ptr;
}

// BINARY_OP
BINARY_OP_Struct* BINARY_OP_func() {
    BINARY_OP_Struct* binary_op_ptr = new BINARY_OP_Struct();
    std::cout << "BINARY_OP_func" << std::endl;
    GrammerToken curToken;
    stream.GetToken(curToken);
    if(curToken.token == "BINARY_OP") {
        binary_op_ptr->BINARY_OP = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        return binary_op_ptr;
    }

    return binary_op_ptr;
}

// RESERVED
RESERVED_Struct* RESERVED_func() {
    RESERVED_Struct* reserved_ptr = new RESERVED_Struct();
    GrammerToken curToken;
    stream.GetToken(curToken);
    std::cout << "RESERVED_func" << curToken << std::endl;
    reserved_ptr->RESERVED = curToken.token;
    return reserved_ptr;

    return reserved_ptr;
}
//******************************************************************************

int main(int argc, char *argv[]) {
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
    CompUnits_func();

    std::string code = generator->GenerateCode();
    std::cout << std::endl << std::endl;
    std::cout << code << std::endl;
    fout << code << std::endl;
    return 0;
}
