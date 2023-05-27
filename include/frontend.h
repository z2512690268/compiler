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
    KoopaIR* koopaIR;
    //******************************************************************************
    FrontendBase(std::string fname) {
        filename = fname;
    }
    virtual void Prepare() {
        stream.LoadFile(filename);
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
    };

    Name_Map* name_map = new Name_Map();

    void AddName(const std::string source_name, const std::string ir_name, bool is_const = false) {
        name_map->map[source_name] = ir_name;
        if (is_const) {
            name_map->const_list.push_back(source_name);
        }
    }

    std::string GetIRName(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map->parent != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                return cur_map->map[source_name];
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    bool IsConst(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map->parent != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                for (auto const_name : cur_map->const_list) {
                    if (const_name == source_name) {
                        return true;
                    }
                }
                return false;
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    void PushNameMap() {
        Name_Map* new_map = new Name_Map();
        new_map->parent = name_map;
        name_map = new_map;
    }

    void PopNameMap() {
        name_map = name_map->parent;
    }

    // 当前的while循环的基本块名及其操作
    struct While_Struct {
        std::string while_entry_name = "";
        std::string end_block_name = "";
        While_Struct* parent;
    };

    While_Struct* while_struct = nullptr;

    void PushWhile(const std::string while_entry_name, const std::string end_block_name) {
        While_Struct* new_while = new While_Struct();
        new_while->while_entry_name = while_entry_name;
        new_while->end_block_name = end_block_name;
        new_while->parent = while_struct;
        while_struct = new_while;
    }

    void PopWhile() {
        while_struct = while_struct->parent;
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
    struct ElseStmt_Struct;
    struct NoIfStmt_Struct;

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

    struct Stmt_Struct
    {
        enum StmtType {
            StmtType_IfElse,
            StmtType_If,
            StmtType_While,
            StmtType_NoIf,
        } type;

        struct IfElse_Struct
        {
            Exp_Struct* Condition;
            ElseStmt_Struct* IfClause;
            Stmt_Struct* ElseClause;
        };

        struct If_Struct
        {
            Exp_Struct* Condition;
            Stmt_Struct* Clause;
        };

        struct While_Struct {
            Exp_Struct* Condition;
            Stmt_Struct* Clause;
        };
        
        union SubStructPointer {
            IfElse_Struct IfElse;
            If_Struct If;
            While_Struct While;
            NoIfStmt_Struct* NoIfStmt;
        } subStructPointer;
    };

    struct ElseStmt_Struct
    {
        enum ElseStmtType {
            ElseStmtType_IfElse,
            ElseStmtType_While,
            ElseStmtType_NoIf,
        } type;

        struct IfElse_Struct
        {
            Exp_Struct* Condition;
            ElseStmt_Struct* IfClause;
            ElseStmt_Struct* ElseClause;
        };

        struct While_Struct {
            Exp_Struct* Condition;
            ElseStmt_Struct* Clause;
        };
        
        union SubStructPointer {
            IfElse_Struct IfElse;
            While_Struct While;
            NoIfStmt_Struct* NoIfStmt;
        } subStructPointer;
    };

    struct NoIfStmt_Struct {
        enum NoIfStmtType {
            NoIfStmtType_Assign,
            NoIfStmtType_Return,
            NoIfStmtType_Block,
            NoIfStmtType_Exp,
            NoIfStmtType_Break,
            NoIfStmtType_Continue,
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
    Stmt_Struct* Stmt_func(std::string end_label);
    ElseStmt_Struct* ElseStmt_func(std::string end_label);
    NoIfStmt_Struct* NoIfStmt_func();
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