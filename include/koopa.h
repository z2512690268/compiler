#pragma once

#include <cstddef>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "unique_ptr.h"

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
        unique_ptr<KoopaVarType> type;
        int size;
    };
    struct KoopaPtrType {
        unique_ptr<KoopaVarType> type;
    };
    struct KoopaFuncType {
        unique_ptr<KoopaVarType> retType;
        std::vector<unique_ptr<KoopaVarType>> paramsType;
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
        if(type == KOOPA_INT32 || type == KOOPA_undef) {
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
                arrayType.type = unique_ptr<KoopaVarType>(
                    new KoopaVarType(*other.arrayType.type));
                arrayType.size = other.arrayType.size;
                break;
            case KOOPA_PTR:
                ptrType.type = unique_ptr<KoopaVarType>(
                    new KoopaVarType(*(other.ptrType.type)));
                break;
            case KOOPA_func:
                funcType.retType = unique_ptr<KoopaVarType>(
                    new KoopaVarType(*(other.funcType.retType)));
                for(auto& param : other.funcType.paramsType) {
                    funcType.paramsType.push_back(unique_ptr<KoopaVarType>(
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
        type.arrayType.type = unique_ptr<KoopaVarType>(new KoopaVarType(element_type));
        type.arrayType.size = size;
        return type;
    }

    KoopaVarType PTR_Type(KoopaVarType element_type) {
        KoopaVarType type;
        type.topType = KOOPA_PTR;
        type.ptrType.type = unique_ptr<KoopaVarType>(new KoopaVarType(element_type));
        return type;
    }

    KoopaVarType FUNC_Type(KoopaVarType ret_type, std::vector<KoopaVarType> params_type) {
        KoopaVarType type;
        type.topType = KOOPA_func;
        type.funcType.retType = unique_ptr<KoopaVarType>(new KoopaVarType(ret_type));
        for(auto& param : params_type) {
            type.funcType.paramsType.push_back(unique_ptr<KoopaVarType>(new KoopaVarType(param)));
        }
        return type;
    }

    int Size() {
        switch(topType) {
            case KoopaVarType::KOOPA_INT32:
                return 4;
            case KoopaVarType::KOOPA_ARRAY:
                return (*arrayType.type).Size() * arrayType.size;
            case KoopaVarType::KOOPA_PTR:
                return 4;
            case KoopaVarType::KOOPA_func:
                return 0;
            case KoopaVarType::KOOPA_undef:
                return 0;
        }
        return 0;
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
        bool valid = IsValidBinaryOp();
        if(!valid) {
            std::cerr << "Invalid KoopaBinaryOperation: " << op << std::endl;
            std::cerr << "Valid operations: ne eq gt lt ge le add sub mul div mod and or xor shl shr sar" << std::endl;
        }
        return valid;
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
    struct Return_Statement{
        KoopaSymbol ret;
    } returnStmt;
    
    // CALL
    struct Call_Statement{
        KoopaSymbol func_name;
        std::vector<KoopaSymbol> params;
        KoopaVar ret_var;
    } callStmt;
    
    // BRANCH
    struct Branch_Statement{
        KoopaSymbol cond;
        std::string true_label;
        std::string false_label;
    } branchStmt;
    
    // JUMP
    struct Jump_Statement{
        std::string label;
    } jumpStmt;
    
    // ALLOC
    struct Alloc_Statement{
        KoopaVar var;
    } allocStmt;

    // LOAD
    struct Load_Statement{
        KoopaSymbol addr;
        KoopaVar var;
    } loadStmt;

    // STORE
    struct Store_Statement{
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
    struct Getptr_Statement{
        KoopaVar varptr;
        KoopaSymbol offset;
        KoopaVar ret_var;
    } getptrStmt;

    // GETELEMENTPTR
    struct GetElementptr_Statement{
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

    bool AddNewLabelSymbol(std::string label, BasicBlock* block) {
        label_table[label] = block;
        return true;
    }

    bool AddNewVarSymbol(std::string name, KoopaVar var) {
        VarSymbolItem item;
        item.name = name;
        item.var = var;
        var_table[name] = item;
        return true;
    }

    std::unordered_map<std::string, VarSymbolItem> var_table;
    std::unordered_map<std::string, BasicBlock*> label_table;
};

struct Scope {
    Scope(Scope* parent = nullptr) : parent(parent) { }

    Scope* parent;

    std::string func_name;
    std::vector<KoopaVar> func_param;
    KoopaVarType func_ret_type;
    int nested_call = 0;
    int max_nested_call_varnum = 0;

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

    void SetScopeFunction(std::string func_name, KoopaVarType func_ret_type,
                            std::vector<KoopaVar> params) {
        curScope->func_name = func_name;
        curScope->func_ret_type = func_ret_type;
        curScope->func_param = params;
    }

    void ScopeAddBasicBlock(BasicBlock* block) {
        curScope->basicBlocks.push_back(block);
    }

    std::string GetUniqueName(std::string name) {
        int i = -1;
        std::string new_name = name;
        while(1) {
            Scope* scope = curScope;
            if(i != -1) {
                new_name = name + "_" + std::to_string(i);
            }
            while(scope != nullptr) {
                if(scope->symbolTable.CheckSymbol(new_name)) {
                    i++;
                    break;
                }
                scope = scope->parent;
            }
            if(scope == nullptr) {
                break;
            }
        }
        return new_name;
    }

    std::string GetUniqueTempName() {
        int i = 0;
        std::string new_name;
        while(1) {
            Scope* scope = curScope;
            new_name = "%" + std::to_string(i);
            while(scope != nullptr) {
                if(scope->symbolTable.CheckSymbol(new_name)) {
                    i++;
                    break;
                }
                scope = scope->parent;
            }
            if(scope == nullptr) {
                break;
            }
        }
        return new_name;
    }

    BasicBlock* NewBasicBlockAndSetCur(std::string name) {
        BasicBlock* block = new BasicBlock();
        block->label = name;
        curScope->symbolTable.AddNewLabelSymbol(name, block);
        curBlock = block;
        return block;
    }

    BasicBlock* NewBasicBlock(std::string name) {
        BasicBlock* block = new BasicBlock();
        block->label = name;
        curScope->symbolTable.AddNewLabelSymbol(name, block);
        return block;
    }

    void SetCurBlock(BasicBlock* block) {
        curBlock = block;
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
        curScope->nested_call++;
        curScope->max_nested_call_varnum = std::max(curScope->max_nested_call_varnum, (int)params.size());
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

    Statement* AddLoadStatement(KoopaSymbol addr, KoopaVar var) {
        Statement* stmt = new Statement();
        stmt->type = Statement::LOAD;
        stmt->loadStmt.addr = addr;
        stmt->loadStmt.var = var;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddStoreStatement(KoopaSymbol addr, KoopaSymbol symbol) {
        Statement* stmt = new Statement();
        stmt->type = Statement::STORE;
        stmt->storeStmt.storeType = Statement::Store_Statement::STORE_SYMBOL;
        stmt->storeStmt.addr = addr;
        stmt->storeStmt.symbol = symbol;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddStoreStatement(KoopaSymbol addr, KoopaInitList initList) {
        Statement* stmt = new Statement();
        stmt->type = Statement::STORE;
        stmt->storeStmt.storeType = Statement::Store_Statement::STORE_INIT;
        stmt->storeStmt.addr = addr;
        stmt->storeStmt.initList = initList;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddGetptrStatement(KoopaVar varptr, KoopaSymbol offset, KoopaVar ret_var) {
        Statement* stmt = new Statement();
        stmt->type = Statement::GETPTR;
        stmt->getptrStmt.varptr = varptr;
        stmt->getptrStmt.offset = offset;
        stmt->getptrStmt.ret_var = ret_var;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    Statement* AddGetelementptrStatement(KoopaVar arrayptr, KoopaSymbol index, KoopaVar ret_var) {
        Statement* stmt = new Statement();
        stmt->type = Statement::GETELEMENTPTR;
        stmt->getelementptrStmt.arrayptr = arrayptr;
        stmt->getelementptrStmt.index = index;
        stmt->getelementptrStmt.ret_var = ret_var;
        curBlock->statements.push_back(stmt);
        return stmt;
    }

    KoopaVar NewVar(KoopaVarType type, std::string name) {
        KoopaVar var;
        var.type = type;
        var.varName = name;
        curScope->symbolTable.AddNewVarSymbol(var.varName, var);
        return var;
    }

    KoopaVar NewVarWithInit(KoopaVarType type, std::string name, KoopaInitList list)
    {
        KoopaVar var;
        var.type = type;
        var.varName = name;
        var.initList = list;
        curScope->symbolTable.AddNewVarSymbol(var.varName, var);
        return var;
    }

    KoopaVar NewTempVar(KoopaVarType type) {
        KoopaVar var;
        var.type = type;
        var.varName = GetUniqueTempName();
        curScope->symbolTable.AddNewVarSymbol(var.varName, var);
        return var;
    }

    KoopaVar GetVar(std::string name) {
        Scope* scope = curScope;
        while(scope != nullptr) {
            if(scope->symbolTable.var_table.find(name) != scope->symbolTable.var_table.end()) {
                return scope->symbolTable.var_table[name].var;
            }
            scope = scope->parent;
        }
        std::cerr << "GetVar: " << name << " not found" << std::endl;
        exit(1);
    }

    BasicBlock* GetBlock(std::string name) {
        Scope* scope = curScope;
        while(scope != nullptr) {
            if(scope->symbolTable.label_table.find(name) != scope->symbolTable.label_table.end()) {
                return scope->symbolTable.label_table[name];
            }
            scope = scope->parent;
        }
        std::cerr << "GetBlock: " << name << " not found" << std::endl;
        exit(1);
    }
};