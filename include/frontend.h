#pragma once
#include <string>
#include <vector>
#include "backend.h"
#include "koopa.h"
#include "stream.h"


struct FrontendBase {
    //******************************************************************************
    //输出与输入
    KoopaGenerator* generator;
    TokenStream<GrammerToken> stream;
    std::string filename;
    //******************************************************************************
    
    //******************************************************************************
    // 解析过程中的全局变量区
    // 当前作用域/基本块
    Scope* curScope;
    BasicBlock* curBlock;
    //******************************************************************************
    FrontendBase(std::string fname, KoopaGenerator* generator) : generator(generator){
        filename = fname;
    }
    virtual void Prepare() {
        stream.LoadFile(filename);
        curScope = &generator->global_scope;
        curBlock = nullptr;
    }
    virtual void Process() {
    }
};

struct SysyFrontend : public FrontendBase {
    

    //******************************************************************************
    // 接口函数
    SysyFrontend(std::string fname, KoopaGenerator* generator) : FrontendBase(fname, generator){ }
    virtual void Process() {
        Prepare();
        CompUnits_func();
    }
    //******************************************************************************
    
    // //******************************************************************************
    // // 函数和结构体前置声明
    // // 非终结符
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
    // //******************************************************************************

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
        FuncType_Struct* funcRetType;
        IDENT_Struct*   funcnameIDENT;
        Block_Struct* Block;

        // ATTRIBUTES
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
        //-----------------------------------------------------------------
        CompUnits_Struct* compUnits_ptr = new CompUnits_Struct();
        std::cout << "CompUnits_func" << std::endl;
        GrammerToken curToken;
        stream.GetToken(curToken);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        if(curToken.rule[0] == "CompUnit") {
            for(auto rule : curToken.rule) {
                if(rule == "CompUnit") {
                    compUnits_ptr->CompUnit.push_back(CompUnit_func());
                }
            }
            return compUnits_ptr;
        } else {
            std::cerr << "CompUnits_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        return compUnits_ptr;
    }

    // CompUnit       ::= FuncDef;
    CompUnit_Struct* CompUnit_func() {
        //-----------------------------------------------------------------
        CompUnit_Struct* compUnit_ptr = new CompUnit_Struct();
        std::cout << "CompUnit_func" << std::endl;
        GrammerToken curToken;
        stream.GetToken(curToken);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        if(curToken.rule[0] == "FuncDef") {
            compUnit_ptr->FuncDef = FuncDef_func();
            return compUnit_ptr;
        } else {
            std::cerr << "CompUnit_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------

        return compUnit_ptr; 
    }


    // FuncDef   ::= FuncType IDENT "(" ")" Block;
    FuncDef_Struct* FuncDef_func() {
        //-----------------------------------------------------------------
        FuncDef_Struct* funcDef_ptr = new FuncDef_Struct();
        std::cout << "FuncDef_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        

        //-----------------------------------------------------------------
        // 创建函数
        // FuncDef   ::= FuncType IDENT "(" ")" Block;
        if(curToken.rule[0] == "FuncType") {
            //-----------------------------------------------------------------
            // 创建作用域
            Scope* func_scope = new Scope(curScope);
            curScope = func_scope;
            generator->func_scopes.push_back(func_scope);
            //-----------------------------------------------------------------
        
            //-----------------------------------------------------------------
            funcDef_ptr->funcRetType = FuncType_func();
            funcDef_ptr->funcnameIDENT = IDENT_func();
            RESERVED_func();    //(
            RESERVED_func();    //)
            funcDef_ptr->Block = Block_func(funcDef_ptr->funcnameIDENT->identifer);
            //-----------------------------------------------------------------
           
            //-----------------------------------------------------------------
            // 退出作用域
            func_scope->func_name = funcDef_ptr->funcnameIDENT->identifer;
            func_scope->func_ret_type = funcDef_ptr->funcRetType->type;
            func_scope->basicBlocks.push_back(funcDef_ptr->Block->block);
            curScope = curScope->parent;
            //-----------------------------------------------------------------
            
            return funcDef_ptr;
        } else {
            std::cerr << "FuncDef_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------


        return funcDef_ptr;
    }

    // FuncType  ::= "int";
    FuncType_Struct* FuncType_func() {
        //-----------------------------------------------------------------
        FuncType_Struct* funcType_ptr = new FuncType_Struct();
        std::cout << "FuncType_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // INT32
        // FuncType  ::= "int";
        if(curToken.rule[0] == "\"int\"") {
            funcType_ptr->type = KOOPA_INT32;
            RESERVED_func();  // "int"
            return funcType_ptr;
        } else {
            std::cerr << "FuncType_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        
        return funcType_ptr;
    }


    // Block     ::= "{" Stmt "}";
    Block_Struct* Block_func(std::string block_name) {
        //-----------------------------------------------------------------
        Block_Struct* block_ptr = new Block_Struct();
        std::cout << "Block_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // Block     ::= "{" Stmt "}";
        if(curToken.rule[0] == "\"{\"") {
            //-----------------------------------------------------------------
            // 创建基本块，并加入符号表
            BasicBlock* block = new BasicBlock(curBlock, curScope, "%entry");
            //-----------------------------------------------------------------

            //-----------------------------------------------------------------
            // 更新当前块
            curBlock = block;
            //-----------------------------------------------------------------

            //-----------------------------------------------------------------
            // 创建语句
            // 只有一句语句
            RESERVED_func();    //{
            Stmt_Struct* stmt_ptr = Stmt_func();
            RESERVED_func();    //}
            //-----------------------------------------------------------------
            
            //-----------------------------------------------------------------
            // 属性赋值
            block_ptr->block = block;
            //-----------------------------------------------------------------
            return block_ptr;
        } else {
            std::cerr << "Block_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        
        return block_ptr;
    }

    // Stmt      ::= "return" Number ";";
    Stmt_Struct* Stmt_func() {
        //-----------------------------------------------------------------
        Stmt_Struct* stmt_ptr = new Stmt_Struct();
        std::cout << "Stmt_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        // Stmt      ::= "return" Number ";";
        if(curToken.rule[0] == "\"return\"") {
            //-----------------------------------------------------------------
            RESERVED_func();    //return
            Number_Struct* number_ptr = Number_func();
            stmt_ptr->Number = number_ptr;
            RESERVED_func();    //;
            //-----------------------------------------------------------------

            //-----------------------------------------------------------------
            // 添加变量
            std::string ret_var_name = curScope->AddVarSymbol("%return", KOOPA_INT32);
            
            // 赋值语句
            Statement* assign = new Statement();
            assign->InitOperationStatement("add", "0", std::to_string(stmt_ptr->Number->value.INT32), ret_var_name);
            curBlock->statements.push_back(assign);

            // 返回语句
            Statement* statement = new Statement();
            statement->InitReturnStatement(ret_var_name);
            curBlock->statements.push_back(statement);
            //-----------------------------------------------------------------
            return stmt_ptr;
        } else {
            std::cerr << "Stmt_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------

        return stmt_ptr;
    }

    // Number    ::= Integer;
    Number_Struct* Number_func() {
        //-----------------------------------------------------------------
        Number_Struct* number_ptr = new Number_Struct();
        std::cout << "Number_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        // Number    ::= Integer;
        if(curToken.rule[0] == "Integer") {
            //-----------------------------------------------------------------
            Integer_Struct* integer_ptr = Integer_func();
            //-----------------------------------------------------------------

            //-----------------------------------------------------------------
            number_ptr->Integer = integer_ptr;
            number_ptr->value.INT32 = integer_ptr->value;
            //-----------------------------------------------------------------
        } else {
            std::cerr << "Number_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        return number_ptr;
    }

    // Integer   ::= OCT_INTEGER;
    Integer_Struct* Integer_func() {
        //-----------------------------------------------------------------
        Integer_Struct* integer_ptr = new Integer_Struct();
        std::cout << "Integer_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        if(curToken.rule[0] == "OCT_INTEGER") {
            //-----------------------------------------------------------------
            OCT_INTEGER_Struct* octInteger_ptr = OCT_INTEGER_func();
            
            //-----------------------------------------------------------------
            integer_ptr->OCT_INTEGER = octInteger_ptr;
            integer_ptr->value = octInteger_ptr->value;
            //-----------------------------------------------------------------
            return integer_ptr;
        } else {
            std::cerr << "Integer_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        return integer_ptr;
    }

    OCT_INTEGER_Struct* OCT_INTEGER_func() {
        //-----------------------------------------------------------------
        OCT_INTEGER_Struct* octInteger_ptr = new OCT_INTEGER_Struct();
        std::cout << "OCT_INTEGER_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        if(curToken.token == "OCT_INTEGER") {
            //-----------------------------------------------------------------
            octInteger_ptr->value = std::stoi(
                curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 8);
            //-----------------------------------------------------------------
            return octInteger_ptr;
        } else {
            std::cerr << "OCT_INTEGER_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------
        return octInteger_ptr;
    }

    IDENT_Struct* IDENT_func() {
        //-----------------------------------------------------------------
        IDENT_Struct* ident_ptr = new IDENT_Struct();
        std::cout << "IDENT_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        if(curToken.token == "IDENT") {
            //-----------------------------------------------------------------
            ident_ptr->identifer = "@" + curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
            //-----------------------------------------------------------------
            return ident_ptr;
        } else {
            std::cerr << "IDENT_func: " << curToken << std::endl;
            exit(1);
        }
        //-----------------------------------------------------------------

        return ident_ptr;
    }

    RESERVED_Struct* RESERVED_func() {
        //-----------------------------------------------------------------
        RESERVED_Struct* reserved_ptr = new RESERVED_Struct();
        std::cout << "RESERVED_func" << std::endl;
        GrammerToken curToken; 
        stream.GetToken(curToken);
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        reserved_ptr->reserved = curToken.rule[0];
        //-----------------------------------------------------------------
        return reserved_ptr;
    }
    //******************************************************************************
};

struct KoopaFrontend : public FrontendBase {
    //******************************************************************************
    // 接口函数
    KoopaFrontend(std::string fname, KoopaGenerator* generator) : FrontendBase(fname, generator){ }
    virtual void Process() {
        Prepare();
        CompUnits_func();
    }
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

};

struct RiscvFrontend : public FrontendBase {
    //******************************************************************************
    // 接口函数
    RiscvFrontend(std::string fname, KoopaGenerator* generator) : FrontendBase(fname, generator){ }
    virtual void Process() {
        Prepare();
        Program_func();
    }
    //******************************************************************************

    //******************************************************************************
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
    //******************************************************************************

    //******************************************************************************
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
    //******************************************************************************

    //******************************************************************************
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
    //******************************************************************************
};