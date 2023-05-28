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

    void AddLibFunc(std::string name, bool isVoid, bool hasI32, bool hasI32Ptr) {
        FuncDef_Struct* func = new FuncDef_Struct();
        func->funcnameIDENT = new IDENT_Struct();
        func->funcnameIDENT->identifer = "@" + name;
        func->funcRetType = new Type_Struct();
        func->funcRetType->type = isVoid ? Type_Struct::Type::Type_Void : Type_Struct::Type::Type_Int;
        func_list.push_back(func);

        KoopaVarType ret_type = isVoid ? KoopaVarType::KOOPA_undef : KoopaVarType::KOOPA_INT32;
        std::vector<KoopaVarType> param_types;
        if (hasI32) {
            param_types.push_back(KoopaVarType::KOOPA_INT32);
        }
        if (hasI32Ptr) {
            KoopaVarType type = KoopaVarType::PTR_Type(KoopaVarType::KOOPA_INT32);
            param_types.push_back(type);
        }
        KoopaVarType funcType = KoopaVarType::FUNC_Type(ret_type, param_types);
        koopaIR->NewVar(funcType, func->funcnameIDENT->identifer);
    }

    virtual KoopaIR* Process() {
        koopaIR = new KoopaIR();
        line_num = 1;
        Prepare();
        AddLibFunc("getint", false, false, false);
        AddLibFunc("getch", false, false, false);
        AddLibFunc("getarray", false, false, true);
        AddLibFunc("putint", true, true, false);
        AddLibFunc("putch", true, true, false);
        AddLibFunc("putarray", true, true, true);
        AddLibFunc("starttime", true, false, false);
        AddLibFunc("stoptime", true, false, false);
        CompUnits_func();
        return koopaIR;
    }

    // 前端符号表及其操作
    struct Name_Struct
    {
        std::string source_name;
        std::string ir_name;
        bool is_const;
        bool is_func_param;
        bool assigned;

        Name_Struct(std::string source_name, std::string ir_name, bool is_const = false, bool is_func_param = false) {
            this->source_name = source_name;
            this->ir_name = ir_name;
            this->is_const = is_const;
            this->is_func_param = is_func_param;
            this->assigned = false;
        }
    };
    

    struct Name_Map {
        std::unordered_map<std::string, Name_Struct*> map;
        Name_Map* parent;
    };

    Name_Map* name_map = new Name_Map();

    void AddName(const std::string source_name, const std::string ir_name, bool is_const = false, bool is_func_param = false) {
        name_map->map[source_name] = new Name_Struct(source_name, ir_name, is_const, is_func_param);
    }

    std::string GetIRName(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                return cur_map->map[source_name]->ir_name;
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    bool IsConst(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                return cur_map->map[source_name]->is_const;
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    bool IsParam(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                return cur_map->map[source_name]->is_func_param;
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    void ChangeName(const std::string source_name, const std::string ir_name) {
        Name_Map* cur_map = name_map;
        while (cur_map != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                cur_map->map[source_name]->ir_name = ir_name;
                cur_map->map[source_name]->assigned = true;
                return;
            }
            cur_map = cur_map->parent;
        }
        std::cerr << "Error: " << source_name << " not found" << std::endl;
        exit(1);
    }

    bool IsAssigned(const std::string source_name) {
        Name_Map* cur_map = name_map;
        while (cur_map != nullptr)
        {
            if (cur_map->map.find(source_name) != cur_map->map.end()) {
                return cur_map->map[source_name]->assigned;
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
    struct Block_Struct;
    struct BlockItem_Struct;
    struct Stmt_Struct;
    struct ElseStmt_Struct;
    struct NoIfStmt_Struct;

    struct FuncDef_Struct;
    struct Type_Struct;
    struct FuncFParams_Struct;
    struct FuncFParam_Struct;
    struct FuncRParams_Struct;

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

    // 函数列表
    std::vector<FuncDef_Struct*> func_list;

    FuncDef_Struct* GetFuncDef(const std::string& func_name) {
        for (auto func : func_list) {
            if (func->funcnameIDENT->identifer == func_name) {
                return func;
            }
        }
        std::cerr << "Error: " << func_name << " not found" << std::endl;
        exit(1);
    }

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
        enum CompUnitType {
            CompUnitType_Decl,
            CompUnitType_FuncDef,
        } compUnitType;
        
        struct SubStructPointer {
            Decl_Struct* Decl;
            FuncDef_Struct* FuncDef;
        } subStructPointer;        
    };

    struct FuncDef_Struct {
        Type_Struct* funcRetType;
        IDENT_Struct* funcnameIDENT;
        FuncFParams_Struct* funcFParams;
        Block_Struct* Block;
    };
    
    struct Type_Struct
    {
        enum Type {
            Type_Int,
            Type_Void,
        } type;

        KoopaVarType koopa_type;
    };

    struct FuncFParams_Struct {
        std::vector<FuncFParam_Struct*> Params;
        std::vector<KoopaVar> koopa_params;
    };

    struct FuncFParam_Struct {
        Type_Struct* Type;
        IDENT_Struct* IDENT;
    };

    struct FuncRParams_Struct {
        std::vector<Exp_Struct*> Params;
        std::vector<KoopaSymbol> koopa_params;
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
            UnaryExpType_FuncCall,
        } type;

        struct UnaryExpWithOp_Struct
        {
            UnaryOp_Struct* UnaryOp;
            UnaryExp_Struct* UnaryExp;
        };
        
        struct FuncCall_Struct
        {
            IDENT_Struct* IDENT;
            FuncRParams_Struct* FuncRParams;
        };

        union SubStructPointer {
            PrimaryExp_Struct* PrimaryExp;
            UnaryExpWithOp_Struct UnaryExpWithOp;
            FuncCall_Struct FuncCall;
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
        Type_Struct* Type;
        std::vector<ConstDef_Struct*> ConstDefs;
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
        Type_Struct* Type;
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
    Block_Struct* Block_func(std::string block_name);
    BlockItem_Struct* BlockItem_func();
    Stmt_Struct* Stmt_func(std::string end_label);
    ElseStmt_Struct* ElseStmt_func(std::string end_label);
    NoIfStmt_Struct* NoIfStmt_func();
    // func
    FuncDef_Struct* FuncDef_func();
    Type_Struct* Type_func();
    FuncFParams_Struct* FuncFParams_func();
    FuncFParam_Struct* FuncFParam_func();
    FuncRParams_Struct* FuncRParams_func();
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
    ConstDef_Struct* ConstDef_func(Type_Struct* Type);
    ConstInitVal_Struct* ConstInitVal_func(KoopaVar* receiver = nullptr);
    VarDecl_Struct* VarDecl_func();
    VarDef_Struct* VarDef_func(Type_Struct* Type);
    InitVal_Struct* InitVal_func(KoopaVar* receiver = nullptr);
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

struct KoopaFrontend : public FrontendBase
{
    //******************************************************************************
    // 接口函数
    KoopaFrontend(std::string fname) : FrontendBase(fname) {}
    virtual KoopaIR *Process()
    {
        koopaIR = new KoopaIR();
        Prepare();
        CompUnits_func();
        return koopaIR;
    }
    //******************************************************************************

    //******************************************************************************
    // 函数和结构体前置声明
    // 非终结符
    struct CompUnits_Struct;
    struct CompUnit_Struct;
    struct Type_Struct;
    struct ArrayType_Struct;
    struct PointerType_Struct;
    struct FunType_Struct;
    struct GetPointer_Struct;
    struct GetElementPointer_Struct;
    struct BinaryExpr_Struct;
    struct Value_Struct;
    struct Initializer_Struct;
    struct Aggregate_Struct;
    struct SymbolDef_Struct;
    struct GlobalSymbolDef_Struct;
    struct MemoryDeclaration_Struct;
    struct GlobalMemoryDeclaration_Struct;
    struct Load_Struct;
    struct Store_Struct;
    struct Branch_Struct;
    struct Jump_Struct;
    struct BlockArgList_Struct;
    struct FunCall_Struct;
    struct Return_Struct;
    struct FunDef_Struct;
    struct FunParams_Struct;
    struct Block_Struct;
    struct BlockParamList_Struct;
    struct Statement_Struct;
    struct EndStatement_Struct;
    struct FunDecl_Struct;
    struct FunDeclParams_Struct;

    // 终结符
    struct TAB_Struct;
    struct SYMBOL_Struct;
    struct INT_Struct;
    struct BINARY_OP_Struct;
    struct RESERVED_Struct;
    //******************************************************************************

    //******************************************************************************
    // 带属性语法树节点
    // 各个类型的结构体
    struct KoopaGramStruct
    {
        CompUnits_Struct *CompUnits;
        // ATTRIBUTES
    };

    struct CompUnits_Struct
    {
        std::vector<CompUnit_Struct *> CompUnit;
        // ATTRIBUTES
    };

    struct CompUnit_Struct
    {
        GlobalSymbolDef_Struct *GlobalSymbolDef;
        FunDecl_Struct *FunDecl;
        FunDef_Struct *FunDef;
        // ATTRIBUTES
    };

    struct FunDef_Struct
    {
        SYMBOL_Struct *SYMBOL;
        Type_Struct *Type;
        std::vector<Block_Struct *> Block;
        FunParams_Struct *FunParams;
        // ATTRIBUTES
    };

    struct FunParams_Struct
    {
        std::vector<SYMBOL_Struct *> SYMBOL;
        std::vector<Type_Struct *> Type;
        // ATTRIBUTES
    };

    struct Type_Struct
    {
        KoopaVarType Type;
        ArrayType_Struct *ArrayType;
        PointerType_Struct *PointerType;
        FunType_Struct *FunType;
        // ATTRIBUTES
    };

    struct ArrayType_Struct
    {
        Type_Struct *Type;
        INT_Struct *INT;
        // ATTRIBUTES
    };

    struct PointerType_Struct
    {
        Type_Struct *Type;
        // ATTRIBUTES
    };

    struct FunType_Struct
    {
        std::vector<Type_Struct *> Type;
        // ATTRIBUTES
    };

    struct Block_Struct
    {
        SYMBOL_Struct *SYMBOL;
        BlockParamList_Struct *BlockParamList;
        std::vector<Statement_Struct *> Statement;
        EndStatement_Struct *EndStatement;
        // ATTRIBUTES
        BasicBlock *block;
    };

    struct BlockParamList_Struct
    {
        std::vector<SYMBOL_Struct *> SYMBOL;
        std::vector<Type_Struct *> Type;
        // ATTRIBUTES
    };

    struct Statement_Struct
    {
        SymbolDef_Struct *SymbolDef;
        Store_Struct *Store;
        FunCall_Struct *FunCall;
        // ATTRIBUTES
    };

    struct SymbolDef_Struct
    {
        SYMBOL_Struct *SYMBOL;
        MemoryDeclaration_Struct *MemoryDeclaration;
        Load_Struct *Load;
        GetPointer_Struct *GetPointer;
        GetElementPointer_Struct *GetElementPointer;
        BinaryExpr_Struct *BinaryExpr;
        FunCall_Struct *FunCall;
        // ATTRIBUTES
        std::string SymbolName;
    };

    struct GlobalSymbolDef_Struct
    {
        SYMBOL_Struct *SYMBOL;
        GlobalMemoryDeclaration_Struct *GlobalMemoryDeclaration;
        // ATTRIBUTES
        std::string SymbolName;
    };

    struct GetPointer_Struct
    {
        SYMBOL_Struct *SYMBOL;
        Value_Struct *Value;
        // ATTRIBUTES
    };

    struct GetElementPointer_Struct
    {
        SYMBOL_Struct *SYMBOL;
        Value_Struct *Value;
        // ATTRIBUTES
    };

    struct BinaryExpr_Struct
    {
        BinaryExpr_Struct *BinaryExpr;
        BINARY_OP_Struct *BINARY_OP;
        Value_Struct *Value1;
        Value_Struct *Value2;
        KoopaSymbol value;
        // ATTRIBUTES
    };

    struct EndStatement_Struct
    {
        Branch_Struct *Branch;
        Jump_Struct *Jump;
        Return_Struct *Return;
        // ATTRIBUTES
    };

    struct FunCall_Struct
    {
        SYMBOL_Struct *SYMBOL;
        std::vector<Value_Struct *> Value;
        // ATTRIBUTES
    };

    struct Return_Struct
    {
        Value_Struct *Value;
        // ATTRIBUTES
    };

    struct Value_Struct
    {
        enum ValueType
        {
            SYMBOL_TYPE,
            INT_TYPE,
            UNDEF_TYPE
        };
        ValueType type;
        // SYMBOL_TYPE
        SYMBOL_Struct *SYMBOL;
        // INT_TYPE
        INT_Struct *INT;
        // ATTRIBUTES
    };

    struct Initializer_Struct
    {
        enum InitializerType
        {
            INT_TYPE,
            UNDEF_TYPE,
            AGGREGATE_TYPE,
            ZEROINIT_TYPE,
        };
        InitializerType type;
        // INT_TYPE
        INT_Struct *INT;
        // AGGREGATE_TYPE
        Aggregate_Struct *Aggregate;
        KoopaInitList *initList;
        // ATTRIBUTES
    };

    struct Aggregate_Struct
    {
        std::vector<Initializer_Struct *> InitializerList;

        // ATTRIBUTES
    };

    struct MemoryDeclaration_Struct
    {
        Type_Struct *Type;
    };

    struct GlobalMemoryDeclaration_Struct
    {
        Type_Struct *Type;
        Initializer_Struct *Initializer;
    };

    struct Load_Struct
    {
        SYMBOL_Struct *SYMBOL;
    };

    struct Store_Struct
    {
        SYMBOL_Struct *SYMBOL0;
        SYMBOL_Struct *SYMBOL;
        Initializer_Struct *Initializer;
    };

    struct Branch_Struct
    {
        Value_Struct *Value;
        SYMBOL_Struct *SYMBOL1;
        BlockArgList_Struct *BlockArgList1;
        SYMBOL_Struct *SYMBOL2;
        BlockArgList_Struct *BlockArgList2;
    };

    struct Jump_Struct
    {
        SYMBOL_Struct *SYMBOL;
        BlockArgList_Struct *BlockArgList;
    };

    struct BlockArgList_Struct
    {
        Value_Struct *Value;
        std::vector<Value_Struct *> ValueList;
        // ATTRIBUTES
    };

    struct FunDecl_Struct
    {
        SYMBOL_Struct *SYMBOL;
        FunDeclParams_Struct *FunDeclParams;
        Type_Struct *Type;
        // ATTRIBUTES
    };

    struct FunDeclParams_Struct
    {
        std::vector<Type_Struct *> Type;
        // ATTRIBUTES
    };

    struct TAB_Struct
    {

        // ATTRIBUTES
    };

    struct SYMBOL_Struct
    {
        std::string SYMBOL;
        // ATTRIBUTES
    };

    struct INT_Struct
    {
        int INT;
        // ATTRIBUTES
    };

    struct BINARY_OP_Struct
    {
        std::string BINARY_OP;
        // ATTRIBUTES
    };

    struct RESERVED_Struct
    {
        std::string RESERVED;
        // ATTRIBUTES
    };

    //******************************************************************************

    // CompUnits ::= CompUnit {CompUnit};
    CompUnits_Struct *CompUnits_func();

    // CompUnit  ::= FunDef;
    CompUnit_Struct *CompUnit_func();

    // FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";
    FunDef_Struct *FunDef_func();
    // Type ::= "i32";
    Type_Struct *Type_func();

    // ArrayType ::= "[" Type "," INT "]";
    ArrayType_Struct *ArrayType_func();

    // PointerType ::= "*" Type;
    PointerType_Struct *PointerType_func();

    // FunType ::= "(" [Type {"," Type}] ")" [":" Type];
    FunType_Struct *FunType_func();

    // Block ::= SYMBOL ":" {Statement ";"} EndStatement ";";
    Block_Struct *Block_func();

    // Statement ::= SymbolDef;
    Statement_Struct *Statement_func();

    // SymbolDef ::= SYMBOL "=" BinaryExpr;
    SymbolDef_Struct *SymbolDef_func();

    // GlobalSymbolDef ::= SYMBOL "=" GlobalMemoryDeclaration;
    GlobalSymbolDef_Struct *GlobalSymbolDef_func();

    // GetPointer ::= "getptr" SYMBOL "," Value;
    GetPointer_Struct *GetPointer_func();

    // GetElementPointer ::= "getelemptr" SYMBOL "," Value;    //2
    GetElementPointer_Struct *GetElementPointer_func();

    // BinaryExpr ::= BINARY_OP Value "," Value;
    BinaryExpr_Struct *BinaryExpr_func();

    // Branch ::= "br" Value "," SYMBOL [BlockArgList] "," SYMBOL [BlockArgList];
    Branch_Struct *Branch_func();

    // Jump ::= "jmp" SYMBOL [BlockArgList];
    Jump_Struct *Jump_func();

    // BlockArgList ::= Value {"," Value};
    BlockArgList_Struct *BlockArgList_func();

    // EndStatement ::= Return;
    EndStatement_Struct *EndStatement_func();

    // FunCall ::= SYMBOL "(" [Value {"," Value}] ")";
    FunCall_Struct *FunCall_func();

    // FunParams ::= SYMBOL ":" Type {"," SYMBOL ":" Type};
    FunParams_Struct *FunParams_func();

    // BlockParamList ::= "(" SYMBOL ":" Type {"," SYMBOL ":" Type} ")";
    BlockParamList_Struct *BlockParamList_func();

    // FunDecl ::= "decl" SYMBOL "(" [FunDeclParams] ")" [":" Type];
    FunDecl_Struct *FunDecl_func();

    // FunDeclParams ::= Type {"," Type};
    FunDeclParams_Struct *FunDeclParams_func();

    // Return ::= "ret" [Value];
    Return_Struct *Return_func();

    // Value ::= SYMBOL | INT | "undef";
    Value_Struct *Value_func();

    // Initializer  ::= INT | "undef" | Aggregate | "zeroinit";
    Initializer_Struct *Initializer_func();

    // Aggregate ::= "{" [Initializer {"," Initializer}] "}";
    Aggregate_Struct *Aggregate_func();

    // MemoryDeclaration ::= "alloc" Type;
    MemoryDeclaration_Struct *MemoryDeclaration_func();

    // GlobalMemoryDeclaration ::= "alloc" Type "," Initializer;
    GlobalMemoryDeclaration_Struct *GlobalMemoryDeclaration_func();

    // Load ::= "load" SYMBOL;
    Load_Struct *Load_func();

    // Store ::= "store" (Value | InitializerBlock) "," SYMBOL;
    Store_Struct *Store_func();

    TAB_Struct *TAB_func();

    // SYMBOL
    SYMBOL_Struct *SYMBOL_func();

    // INT
    INT_Struct *INT_func();

    // BINARY_OP
    BINARY_OP_Struct *BINARY_OP_func();

    // RESERVED
    RESERVED_Struct *RESERVED_func();
    //******************************************************************************
};