#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "backend.h"
#include "koopa.h"
#include "stream.h"
#include "debug.h"

#define ENTRY_GRAMMER(return_type) \
    return_type* ret_ptr = new return_type(); \
    GrammerToken curToken; \
    stream.GetToken(curToken); \
    DEBUG_FRONTEND_ENTRY std::cout << "Enter -- " << curToken << std::endl;


struct FrontendBase {
    //******************************************************************************
    //输出与输入
    // KoopaGenerator* generator;
    TokenStream<GrammerToken> stream;
    std::string filename;
    //******************************************************************************
    
    //******************************************************************************
    // 解析过程中的全局变量区
    // 当前作用域/基本块
    Scope* curScope;
    BasicBlock* curBlock;
    KoopaIR* koopaIR;
    //******************************************************************************
    FrontendBase(std::string fname) {
        filename = fname;
    }
    virtual void Prepare() {
        stream.LoadFile(filename);
        curScope = &koopaIR->global_scope;
        curBlock = nullptr;
    }
    virtual KoopaIR* Process() {
        return nullptr;
    }
};

struct SysyFrontend : public FrontendBase {
    int line_num;
    //******************************************************************************
    // 接口函数
    SysyFrontend(std::string fname) : FrontendBase(fname){ }
    virtual KoopaIR* Process() {
        koopaIR = new KoopaIR();
        line_num = 1;
        Prepare();
        CompUnits_func();
        return koopaIR;
    }

    // 前端符号表及其操作
    struct Name_Map {
        std::unordered_map<std::string, std::string> map;
        std::vector<std::string> const_list;
        Name_Map* parent;
    } name_map;

    void AddName(const std::string& source_name, const std::string& ir_name, bool is_const = false) {
        name_map.map[source_name] = ir_name;
        if (is_const) {
            name_map.const_list.push_back(source_name);
        }
    }

    std::string GetIRName(const std::string& source_name) {
        if(name_map.map.find(source_name) == name_map.map.end()) {
            return source_name;
        }
        return name_map.map[source_name];
    }

    bool IsConst(const std::string& source_name) {
        return std::find(name_map.const_list.begin(), name_map.const_list.end(), source_name) != name_map.const_list.end();
    }

    void PushNameMap() {
        Name_Map* new_map = new Name_Map();
        new_map->parent = &name_map;
        name_map = *new_map;
    }

    void PopNameMap() {
        name_map = *name_map.parent;
    }

    void Error(const std::string& error_info) {
        std::cerr << "Error: " << error_info << " at line " << line_num << std::endl;
        exit(1);
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
    struct BlockItem_Struct;
    struct Stmt_Struct;

    struct Exp_Struct;
    struct LVal_Struct;
    struct PrimaryExp_Struct;
    struct UnaryExp_Struct;
    struct UnaryOp_Struct;
    struct MulExp_Struct;
    struct AddExp_Struct;
    struct RelExp_Struct;
    struct EqExp_Struct;
    struct LAndExp_Struct;
    struct LOrExp_Struct;
    struct Number_Struct;

    struct ConstExp_Struct;
    struct Integer_Struct;
    struct Decl_Struct;
    struct ConstDecl_Struct;
    struct BType_Struct;
    struct ConstDef_Struct;
    struct ConstInitVal_Struct;
    struct VarDecl_Struct;
    struct VarDef_Struct;
    struct InitVal_Struct;

    // 终结符
    struct DEC_INTEGER_Struct;
    struct OCT_INTEGER_Struct;
    struct HEX_INTEGER_Struct;
    struct IDENT_Struct;
    struct RESERVED_Struct;
    // //******************************************************************************

    //******************************************************************************
    // 带属性语法树节点
    // 各个类型的结构体
    struct SysyGramStruct {
        CompUnits_Struct* CompUnits;
    };

    struct CompUnits_Struct {
        std::vector<CompUnit_Struct*> CompUnit;
    };

    struct CompUnit_Struct {
        FuncDef_Struct* FuncDef;
    };

    struct FuncDef_Struct {
        FuncType_Struct* funcRetType;
        IDENT_Struct*   funcnameIDENT;
        Block_Struct* Block;
    };

    struct FuncType_Struct {
        KoopaVarType type;
    };

    struct Block_Struct {
        std::vector<BlockItem_Struct*> BlockItems;

        BasicBlock* block;
    };

    struct BlockItem_Struct {
        enum BlockItemType {
            BlockItemType_Decl,
            BlockItemType_Stmt,
        } type;

        union SubStructPointer {
            Decl_Struct* Decl;
            Stmt_Struct* Stmt;
        } subStructPointer;
    };    

    struct Stmt_Struct {
        enum StmtType {
            StmtType_Assign,
            StmtType_Return,
            StmtType_Block,
            StmtType_Exp,
        } type;
        
        struct Assign_Struct {
            LVal_Struct* LVal;
            Exp_Struct* Exp;
        };

        union SubStructPointer { 
            Assign_Struct Assign;
            Exp_Struct* Return;
            Block_Struct* Block;
            Exp_Struct* Exp;
        } subStructPointer;
    };

    struct Exp_Struct {
        LOrExp_Struct* LOrExp;

        KoopaSymbol value;
    };

    struct LVal_Struct
    {
        std::string ident;
    };

    struct PrimaryExp_Struct {
        enum PrimaryExpType {
            PrimaryExpType_Exp,
            PrimaryExpType_LVal,
            PrimaryExpType_Number,
        } type;

        union SubStructPointer {
            Exp_Struct* Exp;
            LVal_Struct* LVal;
            Number_Struct* Number;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct UnaryExp_Struct {
        enum UnaryExpType {
            UnaryExpType_PrimaryExp,
            UnaryExpType_UnaryExpWithOp,
        } type;

        struct UnaryExpWithOp_Struct
        {
            UnaryOp_Struct* UnaryOp;
            UnaryExp_Struct* UnaryExp;
        };
        

        union SubStructPointer {
            PrimaryExp_Struct* PrimaryExp;
            UnaryExpWithOp_Struct UnaryExpWithOp;
        } subStructPointer;

        KoopaSymbol value;
    };
    
    struct UnaryOp_Struct {
        enum UnaryOpType {
            UnaryOpType_Add,
            UnaryOpType_Sub,
            UnaryOpType_Not,
        } type;
    };

    struct MulExp_Struct {
        enum MulExpType {
            MulExpType_UnaryExp,
            MulExpType_MulAndUnary,
        } type;

        enum OpType {
            OpType_Mul,
            OpType_Div,
            OpType_Mod,
        };

        struct MulAndUnary_Struct
        {
            MulExp_Struct* MulExp;
            OpType op;
            UnaryExp_Struct* UnaryExp;
        };

        union SubStructPointer {
            UnaryExp_Struct* UnaryExp;
            MulAndUnary_Struct MulAndUnary;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct AddExp_Struct {
        enum AddExpType {
            AddExpType_MulExp,
            AddExpType_AddAndMul,
        } type;

        enum OpType {
            OpType_Add,
            OpType_Sub,
        };

        struct AddAndMul_Struct
        {
            AddExp_Struct* AddExp;
            OpType op;
            MulExp_Struct* MulExp;
        };

        union SubStructPointer {
            MulExp_Struct* MulExp;
            AddAndMul_Struct AddAndMul;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct RelExp_Struct {
        enum RelExpType {
            RelExpType_AddExp,
            RelExpType_RelAndAdd,
        } type;

        enum OpType {
            OpType_LT,
            OpType_GT,
            OpType_LE,
            OpType_GE,
        };

        struct RelAndAdd_Struct
        {
            RelExp_Struct* RelExp;
            OpType op;
            AddExp_Struct* AddExp;
        };

        union SubStructPointer {
            AddExp_Struct* AddExp;
            RelAndAdd_Struct RelAndAdd;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct EqExp_Struct {
        enum EqExpType {
            EqExpType_RelExp,
            EqExpType_EqAndRel,
        } type;

        enum OpType {
            OpType_EQ,
            OpType_NE,
        };

        struct EqAndRel_Struct
        {
            EqExp_Struct* EqExp;
            OpType op;
            RelExp_Struct* RelExp;
        };

        union SubStructPointer {
            RelExp_Struct* RelExp;
            EqAndRel_Struct EqAndRel;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct LAndExp_Struct
    {
        enum LAndExpType {
            LAndExpType_EqExp,
            LAndExpType_LAndAndEq,
        } type;

        struct LAndAndEq_Struct
        {
            LAndExp_Struct* LAndExp;
            EqExp_Struct* EqExp;
        };

        union SubStructPointer {
            EqExp_Struct* EqExp;
            LAndAndEq_Struct LAndAndEq;
        } subStructPointer;

        KoopaSymbol value;
    };

    struct LOrExp_Struct
    {
        enum LOrExpType {
            LOrExpType_LAndExp,
            LOrExpType_LOrAndLAnd,
        } type;

        struct LOrAndLAnd_Struct
        {
            LOrExp_Struct* LOrExp;
            LAndExp_Struct* LAndExp;
        };

        union SubStructPointer {
            LAndExp_Struct* LAndExp;
            LOrAndLAnd_Struct LOrAndLAnd;
        } subStructPointer;

        KoopaSymbol value;
    };    

    struct Number_Struct {
        Integer_Struct* Integer;

        KoopaSymbol value;
    };

    struct ConstExp_Struct {
        Exp_Struct* Exp;

        KoopaSymbol value;
    };

    struct Integer_Struct {
        enum IntegerType {
            IntegerType_DEC_INTEGER,
            IntegerType_OCT_INTEGER,
            IntegerType_HEX_INTEGER,
        } type;

        union subStructPointer {
            DEC_INTEGER_Struct* DEC_INTEGER;
            OCT_INTEGER_Struct* OCT_INTEGER;
            HEX_INTEGER_Struct* HEX_INTEGER;
        } subStructPointer;

        int value;
    };

    struct Decl_Struct {
        enum DeclType {
            DeclType_ConstDecl,
            DeclType_VarDecl,
        } type;
        
        union subStructPointer {
            ConstDecl_Struct* ConstDecl;
            VarDecl_Struct* VarDecl;
        } subStructPointer;
    };

    struct ConstDecl_Struct {
        BType_Struct* BType;
        std::vector<ConstDef_Struct*> ConstDefs;
    };

    struct BType_Struct {
        enum BTypeType {
            BTypeType_INT,
        } type;
    };

    struct ConstDef_Struct {
        std::string ident;
        ConstInitVal_Struct* ConstInitVal;
    };

    struct ConstInitVal_Struct {
        ConstExp_Struct* ConstExp;

        KoopaSymbol value;
    };

    struct VarDecl_Struct {
        BType_Struct* BType;
        std::vector<VarDef_Struct*> VarDefs;
    };

    struct VarDef_Struct {
        std::string ident;
        
        enum VarDefType {
            VarDefType_Initialized,
            VarDefType_Uninitialized,
        } type;

        InitVal_Struct* InitVal;
    };

    struct InitVal_Struct {
        Exp_Struct* Exp;

        KoopaSymbol value;
    };

    // 终结符
    struct IDENT_Struct {
        std::string identifer;
    };

    struct OCT_INTEGER_Struct {
        std::string value;

        int value_int;
    };

    struct DEC_INTEGER_Struct {
        std::string value;

        int value_int;
    };

    struct HEX_INTEGER_Struct {
        std::string value;

        int value_int;
    };

    struct RESERVED_Struct {
        std::string reserved;
    };
    //******************************************************************************

    //******************************************************************************
    // 建树与解析函数
    // basic
    CompUnits_Struct* CompUnits_func();
    CompUnit_Struct* CompUnit_func();
    FuncDef_Struct* FuncDef_func();
    FuncType_Struct* FuncType_func();
    Block_Struct* Block_func(std::string block_name);
    BlockItem_Struct* BlockItem_func();
    Stmt_Struct* Stmt_func();
    // exps
    Exp_Struct* Exp_func(KoopaVar* receiver = nullptr);
    LVal_Struct* LVal_func();
    PrimaryExp_Struct* PrimaryExp_func(KoopaVar* receiver = nullptr);
    UnaryExp_Struct* UnaryExp_func(KoopaVar* receiver = nullptr);
    UnaryOp_Struct* UnaryOp_func();
    MulExp_Struct* MulExp_func(KoopaVar* receiver = nullptr);
    AddExp_Struct* AddExp_func(KoopaVar* receiver = nullptr);
    RelExp_Struct* RelExp_func(KoopaVar* receiver = nullptr);
    EqExp_Struct* EqExp_func(KoopaVar* receiver = nullptr);
    LAndExp_Struct* LAndExp_func(KoopaVar* receiver = nullptr);
    LOrExp_Struct* LOrExp_func(KoopaVar* receiver = nullptr);
    Number_Struct* Number_func();
    // vars
    ConstExp_Struct* ConstExp_func(KoopaVar* receiver = nullptr);
    Integer_Struct* Integer_func();
    Decl_Struct* Decl_func();
    ConstDecl_Struct* ConstDecl_func();
    BType_Struct* BType_func();
    ConstDef_Struct* ConstDef_func(BType_Struct* BType);
    ConstInitVal_Struct* ConstInitVal_func(KoopaVar* receiver);
    VarDecl_Struct* VarDecl_func();
    VarDef_Struct* VarDef_func(BType_Struct* BType);
    InitVal_Struct* InitVal_func(KoopaVar* receiver);
    // 终结符
    DEC_INTEGER_Struct* DEC_INTEGER_func();
    HEX_INTEGER_Struct* HEX_INTEGER_func();
    OCT_INTEGER_Struct* OCT_INTEGER_func();
    IDENT_Struct* IDENT_func();
    RESERVED_Struct* RESERVED_func();
    //******************************************************************************
};

// struct KoopaFrontend : public FrontendBase {
//     //******************************************************************************
//     // 接口函数
//     KoopaFrontend(std::string fname, KoopaGenerator* generator) : FrontendBase(fname, generator){ }
//     virtual void Process() {
//         Prepare();
//         CompUnits_func();
//     }
//     //******************************************************************************
    
//     //******************************************************************************
//     // 函数和结构体前置声明
//     // 非终结符
//     struct CompUnits_Struct;
//     struct CompUnit_Struct;
//     struct FunDef_Struct;
//     struct Type_Struct;
//     struct Block_Struct;
//     struct Statement_Struct;
//     struct SymbolDef_Struct;
//     struct BinaryExpr_Struct;
//     struct EndStatement_Struct;
//     struct Return_Struct;
//     struct Value_Struct;
//     // 终结符
//     struct SYMBOL_Struct;
//     struct INT_Struct;
//     struct BINARY_OP_Struct;
//     struct RESERVED_Struct;
//     //******************************************************************************

//     //******************************************************************************
//     // 带属性语法树节点
//     // 各个类型的结构体
//     struct KoopaGramStruct {
//         CompUnits_Struct* CompUnits;
//         // ATTRIBUTES
//     };

//     struct CompUnits_Struct {
//         std::vector<CompUnit_Struct*> CompUnit;
//         // ATTRIBUTES
//     };

//     struct CompUnit_Struct {
//         FunDef_Struct* FunDef;
//         // ATTRIBUTES
//     };

//     struct FunDef_Struct {
//         SYMBOL_Struct* SYMBOL;
//         Type_Struct* Type;
//         std::vector<Block_Struct*> Block;
//         // ATTRIBUTES
//     };

//     struct Type_Struct {
//         KoopaVarType Type;
//         // ATTRIBUTES
//     };

//     struct Block_Struct {
//         SYMBOL_Struct* SYMBOL;
//         std::vector<Statement_Struct*> Statement;
//         EndStatement_Struct* EndStatement;
//         // ATTRIBUTES
//         BasicBlock* block;
//     };

//     struct Statement_Struct {
//         SymbolDef_Struct* SymbolDef;
//         // ATTRIBUTES
//     };

//     struct SymbolDef_Struct {
//         SYMBOL_Struct* SYMBOL;
//         BinaryExpr_Struct* BinaryExpr;
//         // ATTRIBUTES
//         std::string SymbolName;
//     };

//     struct BinaryExpr_Struct {
//         BinaryExpr_Struct* BinaryExpr;
//         BINARY_OP_Struct* BINARY_OP;
//         Value_Struct* Value1;
//         Value_Struct* Value2;
//         // ATTRIBUTES
//     };

//     struct EndStatement_Struct {
//         Return_Struct* Return;
//         // ATTRIBUTES
//     };

//     struct Return_Struct {
//         Value_Struct* Value;
//         // ATTRIBUTES
//     };

//     struct Value_Struct {
//         enum ValueType {
//             SYMBOL_TYPE,
//             INT_TYPE,
//             UNDEF_TYPE
//         };
//         ValueType type;
//         // SYMBOL_TYPE
//         SYMBOL_Struct* SYMBOL;
//         // INT_TYPE
//         INT_Struct* INT;
//         // ATTRIBUTES
//     };

//     struct SYMBOL_Struct {
//         std::string SYMBOL;
//         // ATTRIBUTES
//     };

//     struct INT_Struct {
//         int INT;
//         // ATTRIBUTES
//     };

//     struct BINARY_OP_Struct {
//         std::string BINARY_OP;
//         // ATTRIBUTES
//     };

//     struct RESERVED_Struct {
//         std::string RESERVED;
//         // ATTRIBUTES
//     };

//     //******************************************************************************

//     // CompUnits ::= CompUnit {CompUnit};
//     CompUnits_Struct* CompUnits_func() {
//         CompUnits_Struct* compUnits_ptr = new CompUnits_Struct();
//         std::cout << "CompUnits_func" << std::endl;
//         GrammerToken curToken; 
//         stream.GetToken(curToken);
//         for(auto rule : curToken.rule) {
//             if(rule == "CompUnit") {
//                 CompUnit_Struct* compUnit_ptr = CompUnit_func();
//                 compUnits_ptr->CompUnit.push_back(compUnit_ptr);
//             }
//         }
//         return compUnits_ptr;
//     }

//     // CompUnit  ::= FunDef;
//     CompUnit_Struct* CompUnit_func() {
//         CompUnit_Struct* compUnit_ptr = new CompUnit_Struct();
//         std::cout << "CompUnit_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "FunDef") {
//             compUnit_ptr->FunDef = FunDef_func();
//             return compUnit_ptr;
//         }

//         return compUnit_ptr; 
//     }

//     // FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";
//     FunDef_Struct* FunDef_func() {
//         FunDef_Struct* funDef_ptr = new FunDef_Struct();
//         std::cout << "FunDef_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         // 创建作用域
//         Scope* func_scope = new Scope(curScope);
//         curScope = func_scope;
//         generator->func_scopes.push_back(func_scope);

//         int i = 0;
//         RESERVED_func();    // fun
//         funDef_ptr->SYMBOL = SYMBOL_func();
//         RESERVED_func();    // (
//         RESERVED_func();    // )
//         // std::cout << "sda" << i << curToken.rule[4] << std::endl;
//         i += 4;
//         if(curToken.rule[i] == "\":\"") {
//             RESERVED_func();    // :
//             funDef_ptr->Type = Type_func();
//             i += 2;
//         }
//         RESERVED_func();    // {
//         i++;
//         // std::cout << "reqw" << i << curToken.rule[i] << std::endl;
//         while(curToken.rule[i] == "Block") {
//             funDef_ptr->Block.push_back(Block_func());
//             i++;
//         }
//         RESERVED_func();    // }

//         // 退出作用域
//         func_scope->func_name = funDef_ptr->SYMBOL->SYMBOL;
//         func_scope->func_ret_type = funDef_ptr->Type->Type;
//         for(auto& block : funDef_ptr->Block) {
//             func_scope->basicBlocks.push_back(block->block);
//         }

//         return funDef_ptr;

//     }
//     // Type ::= "i32";
//     Type_Struct* Type_func() {
//         Type_Struct* type_ptr = new Type_Struct();
//         std::cout << "Type_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "\"i32\"") {
//             RESERVED_func();    // i32
//             type_ptr->Type = KOOPA_INT32;
//             return type_ptr;
//         }

//         return type_ptr;
//     }

//     // Block ::= SYMBOL ":" {Statement ";"} EndStatement ";";
//     Block_Struct* Block_func() {
//         Block_Struct* block_ptr = new Block_Struct();
//         std::cout << "Block_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);


//         int i = 0;
//         std::cout << "sda" << i << curToken.rule[0] << std::endl;
//         if(curToken.rule[i] == "SYMBOL") {
//             block_ptr->SYMBOL = SYMBOL_func();
//             // 创建基本块
//             BasicBlock* block = new BasicBlock();
//             block->parent = curBlock;
//             curBlock = block;
//             // 基本块命名
//             block->label = block_ptr->SYMBOL->SYMBOL;
//             SymbolItem* block_item = new SymbolItem();
//             block_item->InitLabelSymbol(block);
//             curScope->AddSymbol(block->label, block_item);

//             RESERVED_func();    // :
//             i += 2;
//             while(curToken.rule[i] == "Statement") {
//                 block_ptr->Statement.push_back(Statement_func());
//                 std::cout << "sdaad" << std::endl;
//                 RESERVED_func();    // ;
//                 i += 2;
//             }
//             block_ptr->EndStatement = EndStatement_func();
//             RESERVED_func();    // ;
        

//             block_ptr->block = block;
//             return block_ptr;
//         }

//         return block_ptr;
//     }

//     // Statement ::= SymbolDef;
//     Statement_Struct* Statement_func() {
//         Statement_Struct* statement_ptr = new Statement_Struct();
//         std::cout << "Statement_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "SymbolDef") {
//             statement_ptr->SymbolDef = SymbolDef_func();
//             std::cout << "eeew" << std::endl;
//             return statement_ptr;
//         }

//         return statement_ptr;
//     }

//     // SymbolDef ::= SYMBOL "=" BinaryExpr;
//     SymbolDef_Struct* SymbolDef_func() {
//         SymbolDef_Struct* symbolDef_ptr = new SymbolDef_Struct();
//         std::cout << "SymbolDef_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "SYMBOL") {
//             symbolDef_ptr->SYMBOL = SYMBOL_func();
//             RESERVED_func();    // =
//             symbolDef_ptr->BinaryExpr = BinaryExpr_func();

//             symbolDef_ptr->SymbolName = symbolDef_ptr->SYMBOL->SYMBOL;
            
            
//             // 创建符号
//             SymbolItem* symbol_item = new SymbolItem();
//             symbol_item->InitVarSymbol(KOOPA_INT32);
//             curScope->AddSymbol(symbolDef_ptr->SymbolName, symbol_item);
//             std::cout << "OOO" << std::endl;

//             Statement *statement = new Statement();
//             std::string op = symbolDef_ptr->BinaryExpr->BINARY_OP->BINARY_OP;
//             std::string value1 = std::to_string(symbolDef_ptr->BinaryExpr->Value1->INT->INT);
//             std::string value2 = std::to_string(symbolDef_ptr->BinaryExpr->Value2->INT->INT);
//             statement->InitOperationStatement(op,
//                                     value1, 
//                                     value2,
//                                     symbolDef_ptr->SymbolName);
//             curBlock->statements.push_back(statement);
//             return symbolDef_ptr;
//         }

//         return symbolDef_ptr;
//     }

//     // BinaryExpr ::= BINARY_OP Value "," Value;
//     BinaryExpr_Struct* BinaryExpr_func() {
//         BinaryExpr_Struct* binaryExpr_ptr = new BinaryExpr_Struct();
//         std::cout << "BinaryExpr_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "BINARY_OP") {
//             binaryExpr_ptr->BINARY_OP = BINARY_OP_func();
//             binaryExpr_ptr->Value1 = Value_func();
//             RESERVED_func();    // ,
//             binaryExpr_ptr->Value2 = Value_func();
//             std::cout << "WWWW" << std::endl;
//             return binaryExpr_ptr;
//         }

//         return binaryExpr_ptr;
//     }

//     // EndStatement ::= Return;
//     EndStatement_Struct* EndStatement_func() {
//         EndStatement_Struct* endStatement_ptr = new EndStatement_Struct();
//         std::cout << "EndStatement_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "Return") {
//             endStatement_ptr->Return = Return_func();
//             return endStatement_ptr;
//         }

//         return endStatement_ptr;
//     }

//     // Return ::= "ret" [Value];
//     Return_Struct* Return_func() {
//         Return_Struct* return_ptr = new Return_Struct();
//         std::cout << "Return_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         RESERVED_func();    // ret
//         if(curToken.rule[1] == "Value") {
//             return_ptr->Value = Value_func();
//         }

//         Statement* statement = new Statement();
//         statement->InitReturnStatement(return_ptr->Value->SYMBOL->SYMBOL);
//         curBlock->statements.push_back(statement);

//         return return_ptr;

//     }

//     // Value ::= SYMBOL | INT | "undef";
//     Value_Struct* Value_func() {
//         Value_Struct* value_ptr = new Value_Struct();
//         std::cout << "Value_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.rule[0] == "SYMBOL") {
//             value_ptr->type = Value_Struct::SYMBOL_TYPE;
//             value_ptr->SYMBOL = SYMBOL_func();
//             return value_ptr;
//         }
//         if(curToken.rule[0] == "INT") {
//             value_ptr->type = Value_Struct::INT_TYPE;
//             value_ptr->INT = INT_func();
//             return value_ptr;
//         }
//         if(curToken.rule[0] == "\"undef\"") {
//             value_ptr->type = Value_Struct::UNDEF_TYPE;
//             return value_ptr;
//         }

//         return value_ptr;
//     }

//     // SYMBOL
//     SYMBOL_Struct* SYMBOL_func() {
//         SYMBOL_Struct* symbol_ptr = new SYMBOL_Struct();
//         std::cout << "SYMBOL_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.token == "SYMBOL") {
//             symbol_ptr->SYMBOL = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
//             return symbol_ptr;
//         }

//         return symbol_ptr;
//     }

//     // INT
//     INT_Struct* INT_func() {
//         INT_Struct* int_ptr = new INT_Struct();
//         std::cout << "INT_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         std::cout << "sdada" << curToken.rule[0] << std::endl;
//         if(curToken.token == "INT") {
//             std::cout << "sdada" << curToken.rule[0] << std::endl;
//             std::string int_num = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
//             int_ptr->INT = std::stoi(int_num, nullptr, 10);
//             return int_ptr;
//         }

//         return int_ptr;
//     }

//     // BINARY_OP
//     BINARY_OP_Struct* BINARY_OP_func() {
//         BINARY_OP_Struct* binary_op_ptr = new BINARY_OP_Struct();
//         std::cout << "BINARY_OP_func" << std::endl;
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         if(curToken.token == "BINARY_OP") {
//             binary_op_ptr->BINARY_OP = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
//             return binary_op_ptr;
//         }

//         return binary_op_ptr;
//     }

//     // RESERVED
//     RESERVED_Struct* RESERVED_func() {
//         RESERVED_Struct* reserved_ptr = new RESERVED_Struct();
//         GrammerToken curToken;
//         stream.GetToken(curToken);
//         std::cout << "RESERVED_func" << curToken << std::endl;
//         reserved_ptr->RESERVED = curToken.token;
//         return reserved_ptr;

//         return reserved_ptr;
//     }
//     //******************************************************************************

// };

struct RiscvFrontend : public FrontendBase {
    //******************************************************************************
    // 接口函数
    RiscvFrontend(std::string fname) : FrontendBase(fname){ }
    virtual KoopaIR* Process() {
        koopaIR = new KoopaIR();
        Prepare();
        Program_func();
        return koopaIR;
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