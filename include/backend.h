#pragma once
#include "koopa.h"

struct KoopaGenerator {
    Scope global_scope;
    std::vector<Scope*> func_scopes;

    virtual std::string GenerateCode() {
        std::string code;
        for(auto& func : func_scopes) {
            code += "fun " + func->func_name + "(";
            for(int i = 0; i < func->func_params_name.size(); i++) {
                code += KoopaVarTypeToString(func->func_params_type[i]) + " " + func->func_params_name[i];
                if(i != func->func_params_name.size() - 1) {
                    code += ", ";
                }
            }
            code += "): " + KoopaVarTypeToString(func->func_ret_type) + " {\n";
            for(auto& block : func->basicBlocks) {
                std::cout << "OK1" << std::endl;
                code += block->label + ":\n";
                for(auto& stmt : block->statements) {
                    switch(stmt->type) {
                        case Statement::OPRATION:
                            code += "\t" + stmt->OPERATION_ret_var + " = " + stmt->OPERATION_op + " " + stmt->OPERATION_input1_var + ", " + stmt->OPERATION_input2_var + ";\n";
                            break;
                        case Statement::RETURN:
                            code += "\tret " + stmt->RETURN_ret_var + ";\n";
                            break;
                        case Statement::CALL:
                            code += "\t" + stmt->CALL_ret_var + " = call " + stmt->CALL_func_name + "(";
                            for(int i = 0; i < stmt->CALL_params.size(); i++) {
                                code += stmt->CALL_params[i];
                                if(i != stmt->CALL_params.size() - 1) {
                                    code += ", ";
                                }
                            }
                            code += ");\n";
                            break;
                        case Statement::BRANCH:
                            code += "\tif " + stmt->BRANCH_cond_var + " goto " + stmt->BRANCH_true_label + " else goto " + stmt->BRANCH_false_label + ";\n";
                            break;
                        case Statement::JUMP:
                            code += "\tgoto " + stmt->JUMP_label + ";\n";
                            break;
                    }
                }
            }
            code += "}\n";
        }
        return code;
    }
};

struct RiscvGenerator : public KoopaGenerator {

    std::unordered_map<std::string, uint32_t> label_map;
    int label_count = 0;
    std::unordered_map<std::string, std::string> reg_map;

    bool IsArithmeticOperations(std::string op) {
        return op == "add" || op == "sub" || op == "and" || op == "or" || op == "xor" || op == "sll" || op == "srl" || op == "sra";
    }

    virtual std::string GenerateCode() {
        std::string code;
        code += "\t.text\n";  // .text
        for(auto& func : func_scopes) {
            code += "\t.global " + func->func_name.substr(1) + "\n";
            code += func->func_name.substr(1) + ":\n";
            // 不多于8个局部变量的函数，使用寄存器存储
            int s_count = 0;
            for(auto& item : func->symbol_table) {
                if(item.second->type == SymbolItem::VAR) {
                    if(s_count < 8) {
                        reg_map[item.first] = "s" + std::to_string(s_count);
                    }
                }
            }
            for(auto& block : func->basicBlocks) {
                ++label_count;
                label_map[block->label] = label_count;
                code += std::to_string(label_count) + ":\n";
                for(auto& stmt : block->statements) {
                    switch(stmt->type) {
                        case Statement::OPRATION:
                            if(IsArithmeticOperations(stmt->OPERATION_op)) {
                                if( GetKoopaVarType(stmt->OPERATION_input1_var) == KOOPA_SYMBOL &&
                                    GetKoopaVarType(stmt->OPERATION_input2_var) == KOOPA_SYMBOL) {
                                    code += "\t" + stmt->OPERATION_op + " " + reg_map[stmt->OPERATION_ret_var] + ", " + reg_map[stmt->OPERATION_input1_var] + ", " + reg_map[stmt->OPERATION_input2_var] + "\n";
                                } else 
                                if( GetKoopaVarType(stmt->OPERATION_input1_var) == KOOPA_SYMBOL &&
                                    GetKoopaVarType(stmt->OPERATION_input2_var) == KOOPA_IMM) {
                                    code += "\t" + stmt->OPERATION_op + "i " + reg_map[stmt->OPERATION_ret_var] + ", " + reg_map[stmt->OPERATION_input1_var] + ", " + stmt->OPERATION_input2_var + "\n";
                                } else
                                if( GetKoopaVarType(stmt->OPERATION_input1_var) == KOOPA_IMM &&
                                    GetKoopaVarType(stmt->OPERATION_input2_var) == KOOPA_IMM) {
                                    code += "\tli t0, " + stmt->OPERATION_input1_var + "\n";
                                    code += "\t" + stmt->OPERATION_op + "i " + reg_map[stmt->OPERATION_ret_var] + ", t0, " + stmt->OPERATION_input2_var + "\n";
                                }
                            }
                            break;
                        case Statement::RETURN:
                            if(GetKoopaVarType(stmt->RETURN_ret_var) == KOOPA_SYMBOL) {
                                code += "\taddi a0, " + reg_map[stmt->RETURN_ret_var] + ", 0" + "\n";
                            } else if(GetKoopaVarType(stmt->RETURN_ret_var) == KOOPA_IMM) {
                                code += "\tli a0, " + stmt->RETURN_ret_var + "\n";
                            }
                            code += "\tret\n";
                            break;
                        case Statement::CALL:
                            break;
                        case Statement::BRANCH:
                            break;
                        case Statement::JUMP:
                            break;
                    }
                }
            }
        }
        return code;
    }
};