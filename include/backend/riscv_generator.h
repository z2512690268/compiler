#pragma once

#include "koopa.h"
#include <cstdint>
#include <string>
#include <unordered_map>

struct RiscvGenerator : public KoopaGenerator {

    std::unordered_map<std::string, uint32_t> label_map;
    int label_count = 0;
    std::unordered_map<std::string, std::string> reg_map;

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
                            code += "\t" + stmt->OPERATION_op + " " + stmt->OPERATION_input1_var + ", " + stmt->OPERATION_input2_var + "\n";
                            break;
                        case Statement::RETURN:
                            code += "\tret " + reg_map[stmt->RETURN_ret_var] + "\n";
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