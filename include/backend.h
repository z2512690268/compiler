#pragma once
#include "koopa.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>

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

    virtual void GenerateBinCode(std::string filename) {};
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

#include <elf.h>

struct RiscvElfGenerator : public KoopaGenerator {

    std::unordered_map<std::string, uint32_t> label_map;
    int label_count = 0;
    std::unordered_map<std::string, std::string> reg_map;

    bool IsArithmeticOperations(std::string op) {
        return op == "add" || op == "sub" || op == "and" || op == "or" || op == "xor" || op == "sll" || op == "srl" || op == "sra";
    }

    virtual void GenerateBinCode(std::string filename) {
        std::ofstream fout(filename, std::ios::binary);

        Elf32_Ehdr ehdr = {0};
        ehdr.e_ident[0] = 0x7f;
        ehdr.e_ident[1] = 'E';
        ehdr.e_ident[2] = 'L';
        ehdr.e_ident[3] = 'F';
        ehdr.e_ident[4] = 1;    // 32位
        ehdr.e_ident[5] = 1;    // 小端
        ehdr.e_ident[6] = 1;    // 版本

        ehdr.e_type = ET_EXEC;        // 可执行文件
        ehdr.e_machine = EM_RISCV;    // RISC-V
        ehdr.e_version = EV_CURRENT;  // 当前版本
        ehdr.e_entry = 0x10000000;    // 入口地址
        ehdr.e_phoff = sizeof(Elf32_Ehdr);  // 程序头表偏移
        ehdr.e_shoff = 0;                    // 节区头表偏移
        ehdr.e_flags = 0;                    // 无特殊标识
        ehdr.e_ehsize = sizeof(Elf32_Ehdr);  // ELF头大小
        ehdr.e_phentsize = sizeof(Elf32_Phdr);  // 程序头表项大小
        ehdr.e_phnum = 1;                       // 程序头表项数目
        ehdr.e_shentsize = sizeof(Elf32_Shdr);  // 节区头表项大小
        ehdr.e_shnum = 0;                       // 节区头表项数目
        ehdr.e_shstrndx = 0;                    // 节区头表项字符串表索引

        // .text段
        Elf32_Phdr phdr = {0};


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
        return ;
    }
};