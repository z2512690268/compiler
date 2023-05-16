#pragma once

#include <cstddef>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "bits/unique_ptr.h"

// 变量类型
struct KoopaVarType {
    enum TopType {
        KOOPA_INT32,
        KOOPA_ARRAY,
        KOOPA_PTR,
        KOOPA_func,
        KOOPA_undef
    };
    struct KoopaArrayType {
        std::unique_ptr<KoopaVarType> type;
        int size;
    };
    struct KoopaPtrType {
        std::unique_ptr<KoopaVarType> type;
    };
    struct KoopaFuncType {
        std::unique_ptr<KoopaVarType> retType;
        std::vector<std::unique_ptr<KoopaVarType>> paramsType;
    };

    TopType topType;
    // ARRAY
    KoopaArrayType arrayType;
    // PTR
    KoopaPtrType ptrType;
    // FUNC
    KoopaFuncType funcType;

    KoopaVarType() {
        topType = KOOPA_undef;
    }

    KoopaVarType(TopType type) {
        if(type == KOOPA_INT32) {
            topType = type;
        } else {
            std::cerr << "Invalid KoopaVarType" << std::endl;
            exit(1);
        }
    }

    KoopaVarType(const KoopaVarType& other) {
        *this = other;
    }

    KoopaVarType& operator=(const KoopaVarType& other) {
        topType = other.topType;
        switch(topType) {
            case KOOPA_INT32:
                break;
            case KOOPA_ARRAY:
                arrayType.type = std::unique_ptr<KoopaVarType>(
                    new KoopaVarType(*other.arrayType.type));
                arrayType.size = other.arrayType.size;
                break;
            case KOOPA_PTR:
                ptrType.type = std::unique_ptr<KoopaVarType>(
                    new KoopaVarType(*(other.ptrType.type)));
                break;
            case KOOPA_func:
                funcType.retType = std::unique_ptr<KoopaVarType>(
                    new KoopaVarType(*(other.funcType.retType)));
                for(auto& param : other.funcType.paramsType) {
                    funcType.paramsType.push_back(std::unique_ptr<KoopaVarType>(
                        new KoopaVarType(*param)));
                }
                break;
            case KOOPA_undef:
                break;
        }
        return *this;
    }

    bool operator==(const KoopaVarType& other) const {
        if(topType != other.topType) {
            return false;
        }
        switch(topType) {
            case KOOPA_INT32:
                return other.topType == KOOPA_INT32;
            case KOOPA_ARRAY:
                return *arrayType.type == *other.arrayType.type && arrayType.size == other.arrayType.size;
            case KOOPA_PTR:
                return *ptrType.type == *other.ptrType.type;
            case KOOPA_func:
                if(*funcType.retType != *other.funcType.retType) {
                    return false;
                }
                if(funcType.paramsType.size() != other.funcType.paramsType.size()) {
                    return false;
                }
                for(int i = 0; i < funcType.paramsType.size(); i++) {
                    if(*funcType.paramsType[i] != *other.funcType.paramsType[i]) {
                        return false;
                    }
                }
                return true;
            case KOOPA_undef:
                return other.topType == KOOPA_undef;
        }
        return false;
    }

    bool operator!=(const KoopaVarType& other) const {
        return !(*this == other);
    }

    KoopaVarType INT32_Type() {
        KoopaVarType type;
        type.topType = KOOPA_INT32;
        return type;
    }

    KoopaVarType ARRAY_Type(KoopaVarType element_type, int size) {
        KoopaVarType type;
        type.topType = KOOPA_ARRAY;
        type.arrayType.type = std::unique_ptr<KoopaVarType>(new KoopaVarType(element_type));
        type.arrayType.size = size;
        return type;
    }

    KoopaVarType PTR_Type(KoopaVarType element_type) {
        KoopaVarType type;
        type.topType = KOOPA_PTR;
        type.ptrType.type = std::unique_ptr<KoopaVarType>(new KoopaVarType(element_type));
        return type;
    }

    KoopaVarType FUNC_Type(KoopaVarType ret_type, std::vector<KoopaVarType> params_type) {
        KoopaVarType type;
        type.topType = KOOPA_func;
        type.funcType.retType = std::unique_ptr<KoopaVarType>(new KoopaVarType(ret_type));
        for(auto& param : params_type) {
            type.funcType.paramsType.push_back(std::unique_ptr<KoopaVarType>(new KoopaVarType(param)));
        }
        return type;
    }
};

// 初始化列表
struct KoopaInitList {
    enum KoopaInitListType {
        KOOPA_INIT_INT,
        KOOPA_INIT_AGGREGATE,
        KOOPA_INIT_UNDEF,
        KOOPA_INIT_ZERO
    };
    KoopaInitList() {
        initListType = KOOPA_INIT_UNDEF;
    }
    
    KoopaInitList(int initInt) {
        initListType = KOOPA_INIT_INT;
        this->initInt = initInt;
    }

    KoopaInitList(std::vector<KoopaInitList> initList) {
        initListType = KOOPA_INIT_AGGREGATE;
        this->initList = initList;
    }

    KoopaInitList(std::string type) {
        if(type == "undef") {
            initListType = KOOPA_INIT_UNDEF;
        } else if(type == "zeroinit") {
            initListType = KOOPA_INIT_ZERO;
        } else {
            std::cerr << "Invalid KoopaInitList" << std::endl;
            exit(1);
        }
    }

    std::string GetInitListType() {
        switch(initListType) {
            case KOOPA_INIT_INT:
                return "int";
            case KOOPA_INIT_AGGREGATE:
                return "aggregate";
            case KOOPA_INIT_UNDEF:
                return "undef";
            case KOOPA_INIT_ZERO:
                return "zeroinit";
        }
        return "";
    }

    std::string GetInitString() {
        std::string ret;
        ret += "{";
        switch(initListType) {
            case KOOPA_INIT_INT:
                ret = std::to_string(initInt);
                break;
            case KOOPA_INIT_AGGREGATE:
                ret = "{";
                for(int i = 0; i < initList.size(); i++) {
                    ret += initList[i].GetInitString();
                    if(i != initList.size() - 1) {
                        ret += ", ";
                    }
                }
                ret += "}";
                break;
            case KOOPA_INIT_UNDEF:
                ret = "undef";
                break;
            case KOOPA_INIT_ZERO:
                ret = "zeroinit";
                break;
        }
        ret += "}";
        return ret;
    }

    KoopaInitListType initListType;
    int initInt;
    std::vector<KoopaInitList> initList;
};

//变量类型
struct KoopaVar {
    std::string varName;
    KoopaVarType type;
    KoopaInitList initList;
};


// 符号类型
struct  KoopaSymbol {
    enum KoopaSymbolType {
        KOOPA_SYMBOL,   //变量或标签，需要通过符号表进一步分类
        KOOPA_STRING,   //字符串
        KOOPA_IMM,      //立即数，均为十进制
        KOOPA_undef     //未定义类型
    };

    KoopaSymbol() {
        symbolType = KOOPA_undef;
    }

    KoopaSymbol(std::string symbol) {
        this->symbol = symbol;
        symbolType = GetKoopaSymbolType();
    }

    KoopaSymbol(KoopaVar var) {
        symbol = var.varName;
        symbolType = KOOPA_SYMBOL;
    }

    KoopaSymbol(int imm) {
        symbol = std::to_string(imm);
        symbolType = KOOPA_IMM;
    }

    bool IsSymbol() {
        return symbolType == KOOPA_SYMBOL;
    }

    bool IsImm() {
        return symbolType == KOOPA_IMM;
    }

    int GetImm() {
        return std::stoi(symbol, nullptr, 10);
    }

    std::string GetSymbol() {
        return symbol;
    }

    void SetSymbol(std::string symbol) {
        this->symbol = symbol;
        symbolType = KOOPA_SYMBOL;
    }

    void SetImm(int imm) {
        symbol = std::to_string(imm);
        symbolType = KOOPA_IMM;
    }

    bool DefindedSymbol() {
        return symbolType != KOOPA_undef;
    }
    
    KoopaSymbolType GetKoopaSymbolType() {
        if(symbol[0] == '%' || symbol[0] == '@') {
            return KOOPA_SYMBOL;
        } else if(symbol == "undef") {
            return KOOPA_undef;
        } else if(symbol[0] == '"') {
            return KOOPA_STRING;
        }  else {
            return KOOPA_IMM;
        }
    }
    std::string symbol;
    KoopaSymbolType symbolType;
};


struct KoopaBinaryOperation {

    bool SetOp(std::string op) {
        this->op = op;
        return IsValidBinaryOp();
    }

    bool IsValidBinaryOp() {
        // (ne)|(eq)|(gt)|(lt)|(ge)|(le)|(add)|(sub)|(mul)|(div)|(mod)|(and)|(or)|(xor)|(shl)|(shr)|(sar)
        static std::vector<std::string> operations = {
            "ne", "eq", "gt", "lt", "ge", "le", "add", "sub", "mul", "div", "mod", "and", "or", "xor", "shl", "shr", "sar"
        };
        return std::find(operations.begin(), operations.end(), op) != operations.end();
    }
    std::string op;
};

struct Statement {
    enum StatementType{
        OPERATION,
        RETURN,
        CALL,
        BRANCH,
        JUMP,
        ALLOC,
        LOAD,
        STORE,
        GETPTR,
        GETELEMENTPTR
    };
    
    StatementType type;
    //OPERATION 
    struct {
        KoopaBinaryOperation op;
        KoopaSymbol input1;
        KoopaSymbol input2;
        KoopaVar ret_var;
    } binaryOpStmt;

    // RETURN
    struct {
        KoopaSymbol ret;
    } returnStmt;
    
    // CALL
    struct {
        KoopaSymbol func_name;
        std::vector<KoopaSymbol> params;
        KoopaVar ret_var;
    } callStmt;
    
    // BRANCH
    struct {
        KoopaSymbol cond;
        std::string true_label;
        std::string false_label;
    } branchStmt;
    
    // JUMP
    struct {
        std::string label;
    } jumpStmt;
    
    // ALLOC
    struct {
        KoopaVar var;
    } allocStmt;

    // LOAD
    struct {
        KoopaSymbol addr;
        KoopaVar var;
    } loadStmt;

    // STORE
    struct {
        enum StoreType {
            STORE_SYMBOL,
            STORE_INIT
        };
        bool IsSymbol() {
            return storeType == STORE_SYMBOL;
        }
        bool IsInit() {
            return storeType == STORE_INIT;
        }
        StoreType storeType;
        KoopaSymbol symbol;
        KoopaInitList initList;
        KoopaSymbol addr;
    } storeStmt;

    // GETPTR
    struct {
        KoopaVar varptr;
        KoopaSymbol offset;
        KoopaVar ret_var;
    } getptrStmt;

    // GETELEMENTPTR
    struct {
        KoopaVar arrayptr;
        KoopaSymbol index;
        KoopaVar ret_var;
    } getelementptrStmt;
};

struct BasicBlock {
    std::string label;
    std::vector<KoopaVar> block_params;
    std::vector<Statement*> statements;
};

struct SymbolTable {
    struct VarSymbolItem  {
        std::string origin_name;
        std::string name;
        KoopaVar var;
    };

    bool CheckSymbol(std::string name) {
        return var_table.find(name) != var_table.end() || label_table.find(name) != label_table.end();
    }

    std::string GetUniqueName(std::string name) {
        int i = 0;
        std::string new_name = name;
        while(CheckSymbol(new_name)) {
            new_name = name + "_" + std::to_string(i);
            i++;
        }
        return new_name;
    }

    std::string GetUniqueTempName() {
        int i = 0;
        std::string new_name;
        while(CheckSymbol(new_name)) {
            new_name = "%" + std::to_string(i);
            i++;
        }
        return new_name;
    }

    std::string GetKoopaMappedName(std::string origin_name) {
        return origin2KoopaName[origin_name];
    }

    bool AddNewLabelSymbol(std::string origin_label, std::string final_label, BasicBlock* block) {
        origin2KoopaName[origin_label] = final_label;
        label_table[final_label] = block;
        return true;
    }

    bool AddNewVarSymbol(std::string origin_name, std::string final_name, KoopaVar var) {
        origin2KoopaName[origin_name] = final_name;
        VarSymbolItem item;
        item.origin_name = origin_name;
        item.name = final_name;
        item.var = var;
        var_table[final_name] = item;
        return true;
    }

    std::unordered_map<std::string, std::string> origin2KoopaName;
    std::unordered_map<std::string, VarSymbolItem> var_table;
    std::unordered_map<std::string, BasicBlock*> label_table;
};

struct Scope {
    Scope(Scope* parent = nullptr) : parent(parent) { }



    Scope* parent;

    std::string func_name;
    std::vector<KoopaVar> func_param;
    KoopaVarType func_ret_type;

    // basicblocks
    std::vector<BasicBlock*> basicBlocks;
    
    // symbol_table
    SymbolTable symbolTable;
};

struct KoopaIR {
    Scope global_scope;
    std::vector<Scope*> func_scopes;
    Scope* curScope = &global_scope;
    BasicBlock* curBlock = nullptr;

    Scope* AddNewScope() {
        Scope* new_scope = new Scope(curScope);
        curScope = new_scope;
        func_scopes.push_back(new_scope);
        return new_scope;
    }

    Scope* ReturnBackScope() {
        curScope = curScope->parent;
        return curScope;
    }

    void SetScope(Scope* scope) {
        curScope = scope;
        curBlock = nullptr;
    }

    void SetBlock(BasicBlock* block) {
        curBlock = block;
    }

    void SetScopeFunction(std::string func_name, KoopaVarType func_ret_type) {
        curScope->func_name = func_name;
        curScope->func_ret_type = func_ret_type;
    }

    void SetScopeFuntion(std::string func_name, KoopaVarType func_ret_type,
                            std::vector<KoopaVar> params) {
        curScope->func_name = func_name;
        curScope->func_ret_type = func_ret_type;
        curScope->func_param = params;
    }

    void ScopeAddBasicBlock(BasicBlock* block) {
        curScope->basicBlocks.push_back(block);
    }


    BasicBlock* NewBasicBlockAndSetCur(std::string origin_name) {
        std::string name = curScope->symbolTable.GetUniqueName(origin_name);
        BasicBlock* block = new BasicBlock();
        block->label = name;
        curScope->symbolTable.AddNewLabelSymbol(origin_name, name, block);
        curBlock = block;
        return block;
    }

    Statement* AddOperationStatement(std::string op, KoopaSymbol input1, KoopaSymbol input2, KoopaVar ret) {
        Statement* stmt = new Statement();
        stmt->type = Statement::OPERATION;
        stmt->binaryOpStmt.op.SetOp(op);
        stmt->binaryOpStmt.input1 = input1;
        stmt->binaryOpStmt.input2 = input2;
        stmt->binaryOpStmt.ret_var = ret;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddReturnStatement(KoopaSymbol ret) {
        Statement* stmt = new Statement();
        stmt->type = Statement::RETURN;
        stmt->returnStmt.ret = ret;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddCallStatement(KoopaSymbol func_name, std::vector<KoopaSymbol> params, KoopaVar ret) {
        Statement* stmt = new Statement();
        stmt->type = Statement::CALL;
        stmt->callStmt.func_name = func_name;
        stmt->callStmt.params = params;
        stmt->callStmt.ret_var = ret;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddBranchStatement(KoopaSymbol cond, std::string true_label, std::string false_label) {
        Statement* stmt = new Statement();
        stmt->type = Statement::BRANCH;
        stmt->branchStmt.cond = cond;
        stmt->branchStmt.true_label = true_label;
        stmt->branchStmt.false_label = false_label;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddJumpStatement(std::string label) {
        Statement* stmt = new Statement();
        stmt->type = Statement::JUMP;
        stmt->jumpStmt.label = label;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddAllocStatement(KoopaVar var) {
        Statement* stmt = new Statement();
        stmt->type = Statement::ALLOC;
        stmt->allocStmt.var = var;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    KoopaVar NewVar(KoopaVarType type, std::string origin_name) {
        KoopaVar var;
        var.type = type;
        var.varName = curScope->symbolTable.GetUniqueName(origin_name);
        curScope->symbolTable.AddNewVarSymbol(origin_name, var.varName, var);
        return var;
    }

    KoopaVar NewTempVar(KoopaVarType type) {
        KoopaVar var;
        var.type = type;
        var.varName = curScope->symbolTable.GetUniqueTempName();
        curScope->symbolTable.AddNewVarSymbol("", var.varName, var);
        return var;
    }
    
};



    // void AddSymbol(std::string name, SymbolItem* item) {
    //     if(symbol_table.find(name) == symbol_table.end()) {
    //         symbol_table[name] = item;
    //     }
    // }

    // std::string AddVarSymbol(std::string origin_name, KoopaVarType type) {
    //     std::string name = GetUniqueName(origin_name);
    //     SymbolItem* var_item = new SymbolItem();
    //     var_item->InitVarSymbol(type);
    //     AddSymbol(name, var_item);
    //     return name;
    // }





    // void InitOperationStatement(std::string op, std::string input1, std::string input2, std::string ret) {
    //     type = OPERATION;
    //     if(!KoopaValidBinaryOp(op)) {
    //         std::cerr << "Invalid operation: " << op << std::endl;
    //         exit(1);
    //     }
    //     if(GetKoopaSymbolType(input1) == KOOPA_undef || GetKoopaSymbolType(input2) == KOOPA_undef) {
    //         std::cerr << "Invalid input: " << op << " " << input1 << " " << input2 << std::endl;
    //         exit(1);
    //     }
    //     OPERATION_op = op;
    //     OPERATION_input1 = input1;
    //     OPERATION_input2 = input2;
    //     OPERATION_ret_var = ret;
    // }
    // void InitReturnStatement(std::string ret) {
    //     type = RETURN;
    //     if(GetKoopaSymbolType(ret) == KOOPA_undef) {
    //         std::cerr << "Invalid return: " << ret << std::endl;
    //         exit(1);
    //     }
    //     RETURN_ret_var = ret;
    // }
    // void InitCallStatement(std::string func_name, std::vector<std::string> params, std::string ret) {
    //     type = CALL;
    //     CALL_func_name = func_name;
    //     CALL_params = params;
    //     CALL_ret_var = ret;
    // }
    // void InitBranchStatement(std::string cond_var, std::string true_label, std::string false_label) {
    //     type = BRANCH;
    //     BRANCH_cond_var = cond_var;
    //     BRANCH_true_label = true_label;
    //     BRANCH_false_label = false_label;
    // }
    // void InitJumpStatement(std::string label) {
    //     type = JUMP;
    //     JUMP_label = label;
    // }