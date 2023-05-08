#pragma once

#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <string>
#include <bits/unique_ptr.h>

enum KoopaVarType {
    KOOPA_INT32
};
union KoopaVarValue {
    int INT32;
};

struct BasicBlock;

struct SymbolItem  {
    enum SymbolType {
        VAR,
        FUNC,
        LABEL
    };

    void InitVarSymbol(KoopaVarType type) {
        this->type = VAR;
        VAR_type = type;
    }

    void InitFuncSymbol(KoopaVarType ret_type, std::vector<KoopaVarType> params_type) {
        this->type = FUNC;
        FUNC_ret_type = ret_type;
        FUNC_params = params_type;
    }

    void InitLabelSymbol(BasicBlock* block) {
        this->type = LABEL;
        this->block = block;
    }

    std::string origin_name;
    std::string name;
    SymbolType type;
    // VAR
    KoopaVarType VAR_type;
    // FUNC
    KoopaVarType FUNC_ret_type;
    std::vector<KoopaVarType> FUNC_params;
    // LABEL
    BasicBlock* block;
};

struct Statement {
    enum StatementType{
        OPRATION,
        RETURN,
        CALL,
        BRANCH,
        JUMP
    };

    void InitOperationStatement(std::string op, std::string input1, std::string input2, std::string ret) {
        type = OPRATION;
        OPERATION_op = op;
        OPERATION_input1_var = input1;
        OPERATION_input2_var = input2;
        OPERATION_ret_var = ret;
    }
    void InitReturnStatement(std::string ret) {
        type = RETURN;
        RETURN_ret_var = ret;
    }
    void InitCallStatement(std::string func_name, std::vector<std::string> params, std::string ret) {
        type = CALL;
        CALL_func_name = func_name;
        CALL_params = params;
        CALL_ret_var = ret;
    }
    void InitBranchStatement(std::string cond_var, std::string true_label, std::string false_label) {
        type = BRANCH;
        BRANCH_cond_var = cond_var;
        BRANCH_true_label = true_label;
        BRANCH_false_label = false_label;
    }
    void InitJumpStatement(std::string label) {
        type = JUMP;
        JUMP_label = label;
    }

    StatementType type;
    //OPERATION 
    std::string OPERATION_op;
    std::string OPERATION_input1_var;
    std::string OPERATION_input2_var;
    std::string OPERATION_ret_var;
    // RETURN
    std::string RETURN_ret_var;
    // CALL
    std::string CALL_func_name;
    std::vector<std::string> CALL_params;
    std::string CALL_ret_var;
    // BRANCH
    std::string BRANCH_cond_var;
    std::string BRANCH_true_label;
    std::string BRANCH_false_label;
    // JUMP
    std::string JUMP_label;
};

struct BasicBlock {
    BasicBlock* parent;
    std::string label;
    std::vector<std::string> block_params;
    std::vector<Statement*> statements;
};

struct Scope {
    // symbol_table
    std::unordered_map<std::string, SymbolItem*> symbol_table;

    // statements
    std::vector<BasicBlock*> basicBlocks;

    std::string func_name;
    std::vector<KoopaVarType> func_params_type;
    std::vector<std::string> func_params_name;
    KoopaVarType func_ret_type;
    
    Scope* parent;
    Scope(Scope* parent = nullptr) : parent(parent) { }

    bool FindSymbol(std::string name, SymbolItem* &item) {
        Scope* table = this;
        while(table) {
            auto it = table->symbol_table.find(name);
            if(it != table->symbol_table.end()) {
                item = it->second;
                return true;
            }
            table = table->parent;
        }
        return false;
    }

    void AddSymbol(std::string name, SymbolItem* item) {
        if(symbol_table.find(name) == symbol_table.end()) {
            symbol_table[name] = item;
        }
    }

    std::string GetUniqueName(std::string name) {
        int i = 0;
        std::string new_name = name;
        while(symbol_table.find(new_name) != symbol_table.end()) {
            new_name = name + "_" + std::to_string(i);
            i++;
        }
        return new_name;
    }
};

struct KoopaGenerator {
    Scope global_scope;
};