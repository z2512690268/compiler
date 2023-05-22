#pragma once
#include "koopa.h"
#include "defs.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

struct KoopaGenerator {
    Scope global_scope;
    std::vector<Scope*> func_scopes;

    inline std::string KoopaVarTypeToString(KoopaVarType type) {
        std::string ret;
        switch(type.topType) {
            case KoopaVarType::KOOPA_INT32:
                return "i32";
            case KoopaVarType::KOOPA_ARRAY:
                return "[" + KoopaVarTypeToString(*type.arrayType.type) + ", " + std::to_string(type.arrayType.size) + "]";
            case KoopaVarType::KOOPA_PTR:
                return "*" + KoopaVarTypeToString(*type.ptrType.type);
            case KoopaVarType::KOOPA_func:
                ret = "(";
                for(int i = 0; i < type.funcType.paramsType.size(); i++) {
                    ret += KoopaVarTypeToString(*type.funcType.paramsType[i]);
                    if(i != type.funcType.paramsType.size() - 1) {
                        ret += ", ";
                    }
                }
                ret += ") : " + KoopaVarTypeToString(*type.funcType.retType);
                return ret;
            case KoopaVarType::KOOPA_undef:
                return "undef";
        }
        return "";
    }

    inline int KoopaVarTypeSize(KoopaVarType type) {
        switch(type.topType) {
            case KoopaVarType::KOOPA_INT32:
                return 4;
            case KoopaVarType::KOOPA_ARRAY:
                return KoopaVarTypeSize(*type.arrayType.type) * type.arrayType.size;
            case KoopaVarType::KOOPA_PTR:
                return 4;
            case KoopaVarType::KOOPA_func:
                return 0;
            case KoopaVarType::KOOPA_undef:
                return 0;
        }
        return 0;
    }

    virtual std::string GenerateCode(KoopaIR* ir) {
        global_scope = ir->global_scope;
        func_scopes = ir->func_scopes;
        std::string code;
        for(auto& item : global_scope.symbolTable.var_table) {
            if(item.second.var.type != KoopaVarType(KoopaVarType::KOOPA_func)) {
                code += "global " + item.second.var.varName + " = " + KoopaVarTypeToString(item.second.var.type) + ", " + item.second.var.initList.GetInitString() + ";\n";
            } else {
                code += "decl " + item.second.var.varName + "(";
                for(int i = 0; i < item.second.var.type.funcType.paramsType.size(); i++) {
                    code += KoopaVarTypeToString(*item.second.var.type.funcType.paramsType[i]);
                    if(i != item.second.var.type.funcType.paramsType.size() - 1) {
                        code += ", ";
                    }
                }
                code += ") : " + KoopaVarTypeToString(*item.second.var.type.funcType.retType) + ";\n";
            }
        }
        for(auto& func : func_scopes) {
            code += "fun " + func->func_name + "(";
            for(int i = 0; i < func->func_param.size(); i++) {
                code += KoopaVarTypeToString(func->func_param[i].type) + " " + func->func_param[i].varName;
                if(i != func->func_param.size() - 1) {
                    code += ", ";
                }
            }
            code += "): " + KoopaVarTypeToString(func->func_ret_type) + " {\n";
            for(auto& block : func->basicBlocks) {;
                code += block->label + ":\n";
                for(auto& stmt : block->statements) {
                    switch(stmt->type) {
                        case Statement::OPERATION:
                            code += "  " + stmt->binaryOpStmt.ret_var.varName + " = " + stmt->binaryOpStmt.op.op + " " + stmt->binaryOpStmt.input1.GetSymbol() + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
                            break;
                        case Statement::RETURN:
                            if(stmt->returnStmt.ret.GetSymbol() == "") {
                                code += "  ret\t\n";
                            } else {
                                code += "  ret " + stmt->returnStmt.ret.GetSymbol() + "\n";
                            }
                            break;
                        case Statement::CALL:
                            code += "  " + stmt->callStmt.ret_var.varName + " = call " + stmt->callStmt.func_name.GetSymbol() + "(";
                            for(int i = 0; i < stmt->callStmt.params.size(); i++) {
                                code += stmt->callStmt.params[i].GetSymbol();
                                if(i != stmt->callStmt.params.size() - 1) {
                                    code += ", ";
                                }
                            }
                            code += ")\n";
                            break;
                        case Statement::BRANCH:
                            code += "  br " + stmt->branchStmt.cond.GetSymbol() + ", " + stmt->branchStmt.true_label + ", " + stmt->branchStmt.false_label + "\n";
                            break;
                        case Statement::JUMP:
                            code += "  jump " + stmt->jumpStmt.label + "\n";
                            break;
                        case Statement::ALLOC:
                            code += "  " + stmt->allocStmt.var.varName + " = alloc " + KoopaVarTypeToString(stmt->allocStmt.var.type) + "\n";
                            break;
                        case Statement::LOAD:
                            code += "  " + stmt->loadStmt.var.varName + " = load " + stmt->loadStmt.addr.GetSymbol() + "\n";
                            break;
                        case Statement::STORE:
                            if(stmt->storeStmt.IsSymbol()){
                                code += "  store " + stmt->storeStmt.symbol.GetSymbol() + ", " + stmt->storeStmt.addr.GetSymbol() + "\n";
                            } else if(stmt->storeStmt.IsInit()) {
                                code += "  store " + stmt->storeStmt.initList.GetInitString() + ", " + stmt->storeStmt.addr.GetSymbol() + "\n";
                            }
                            break;
                        case Statement::GETPTR:
                            code += "  getptr " + stmt->getptrStmt.ret_var.varName + " = " + stmt->getptrStmt.varptr.varName + ", " + stmt->getptrStmt.offset.GetSymbol() + "\n";
                            break;
                        case Statement::GETELEMENTPTR:
                            code += "  getelementptr " + stmt->getelementptrStmt.ret_var.varName + " = " + stmt->getelementptrStmt.arrayptr.varName + ", " + stmt->getelementptrStmt.index.GetSymbol() + "\n";
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
    enum RiscvOperandType{
        RISCV_REG,
        RISCV_IMM,
        RISCV_STACK
    };
    struct RiscvOperand {
        RiscvOperandType type;
        std::string symbol;

        RiscvOperand(std::string str, RiscvOperandType t) : symbol(str), type(t) {}
    
        std::string GetSymbol() {
            return symbol;
        }

        bool IsReg() {
            return type == RISCV_REG;
        }

        bool IsImm() {
            return type == RISCV_IMM;
        }

        bool IsStack() {
            return type == RISCV_STACK;
        }

    };

    struct RiscvStack {
        int ra;
        std::vector<std::string> local_variable;

    };

    // R-Type : add, sub, and, or, xor, sll, srl, sra
    std::string EmitRTypeOperation(std::string op, std::string rd, std::string rs1, std::string rs2) {
        std::string code;
        code += "\t" + op + " " + rd + ", " + rs1 + ", " + rs2 + "\n";
        return code;
    }

    // I-Type : addi, andi, ori, xori, slli, srli, srai; lb, lh, lw, ld; jalr
    std::string EmitITypeOperation(std::string op, std::string rd, std::string rs1, std::string imm) {
        std::string code;
        code += "\t" + op + " " + rd + ", " + rs1 + ", " + imm + "\n";
        return code;
    }

    // S-Type : sw, sh, sb
    std::string EmitSTypeOperation(std::string op, std::string rs1, std::string rs2, std::string imm) {
        std::string code;
        code += "\t" + op + " " + rs1 + ", " + imm + "(" + rs2 + ")\n";
        return code;
    }

    // SB-Type : beq, bne, blt, bge, bltu, bgeu
    std::string EmitSBTypeOperation(std::string op, std::string rs1, std::string rs2, std::string imm) {
        std::string code;
        code += "\t" + op + " " + rs1 + ", " + rs2 + ", " + imm + "\n";
        return code;
    }

    // U-Type
    std::string EmitUTypeOperation(std::string op, std::string rd, std::string imm) {
        std::string code;
        code += "\t" + op + " " + rd + ", " + imm + "\n";
        return code;
    }

    // UJ-Type : jal
    std::string EmitUJTypeOperation(std::string op, std::string rd, std::string imm) {
        std::string code;
        code += "\t" + op + " " + rd + ", " + imm + "\n";
        return code;
    }

    std::string EmitImm(std::string reg, std::string imm) {
        std::string code;
        code += "\tli " + reg + ", " + imm + "\n";
        return code;
    }

    std::string EmitLoad(std::string reg, std::string stack_sp) {
        std::string code;
        code += "\tlw " + reg + ", " + stack_sp + "\n";
        return code;
    }

    std::string PreOutput() {
        std::string tmp = PROJECT_ROOT_DIR;
        std::string code;
        std::ifstream fin(tmp + "src/res/lib_riscv.s");
        if(!fin.is_open()) {
            std::cout << "lib_riscv.s open failed" << std::endl;
            return "";
        }
        std::string line;
        while(std::getline(fin, line)) {
            code += line + "\n";
        }
        return code;
    }

    virtual std::string GenerateCode(KoopaIR* ir) {
        global_scope = ir->global_scope;
        func_scopes = ir->func_scopes;
        std::string code;
        code += "\t.text\n";  // .text
        for(auto& func : func_scopes) {

            // 函数标签
            std::string func_label_prefix = func->func_name.substr(1) + "_LBB";
            code += "\t.global " + func->func_name.substr(1) + "\n";
            code += func->func_name.substr(1) + ":\n";

            // 寄存器分配
            // 不多于8个局部变量的函数，使用寄存器存储
            std::string reg;
            int a_count = ir->curScope->func_param.size();
            int stack_arg = 0;
            int stack_local = 0;  
            for(auto& item : func->symbolTable.var_table) {
                if(!reg_manager.GetLocalReg(reg)) {
                    stack_local++;
                } {
                    reg_map[item.second.var.varName] = RiscvOperand(reg, RISCV_REG);
                }
            }
            int stack_size = s_count * 4;
            // 保存寄存器
            code += "\taddi sp, sp, -" + std::to_string(s_count * 4) + "\n";
            for(int i = 0; i < s_count; ++i) {
                code += "\tsw s" + std::to_string(i) + ", " + std::to_string(i * 4) + "(sp)\n";
            }

            label_count = 0;
            for(auto& block : func->basicBlocks) {
                ++label_count;
                label_map[block->label] = label_count;
                code += func_label_prefix + std::to_string(label_count) + ":\n";
                for(auto& stmt : block->statements) {
                    switch(stmt->type) {
                        case Statement::OPERATION:
                            if(OpLikeADD(stmt->binaryOpStmt.op.op)) {
                                code += EmitOpLikeADD(stmt);
                            } else if(OpLikeEQ(stmt->binaryOpStmt.op.op)) {
                                code += EmitOpLikeEQ(stmt);
                            } else if(op == "mul" || op == "div" || op == "rem") {
                                code += EmitOpLikeMUL(stmt);
                            }
                            break;
                        case Statement::RETURN:
                            if(stmt->returnStmt.ret.IsSymbol()) {
                                code += "\taddi a0, " + reg_map[stmt->returnStmt.ret.GetSymbol()] + ", 0" + "\n";
                            } else if(stmt->returnStmt.ret.IsImm()) {
                                code += "\tli a0, " + stmt->returnStmt.ret.GetSymbol() + "\n";
                            }
                            for(int i = 0; i < s_count; ++i) {
                                code += "\tlw s" + std::to_string(i) + ", " + std::to_string(i * 4) + "(sp)\n";
                            }
                            code += "\taddi sp, sp, " + std::to_string(s_count * 4) + "\n";
                            code += "\tret\n";
                            break;
                        case Statement::CALL:
                            break;
                        case Statement::BRANCH:
                            break;
                        case Statement::JUMP:
                            break;
                        case Statement::ALLOC:
                            break;
                        case Statement::LOAD:
                            break;
                        case Statement::STORE:
                            break;
                        case Statement::GETPTR:
                            break;
                        case Statement::GETELEMENTPTR:
                            break;
                    }
                }
            }
        }
        return code;
    }

public:
    std::unordered_map<std::string, uint32_t> label_map;
    int label_count = 0;
    std::unordered_map<std::string, RiscvOperand> reg_map;
    RegManager reg_manager;
};


    // bool OpLikeADD(std::string op) {
    //     return op == "add" || op == "sub" || op == "and" || op == "or" || op == "xor" || op == "shl" || op == "shr" || op == "sar";
    // }

    // std::string EmitOpLikeADD(Statement* stmt) {
    //     std::string code;
    //     std::string op;
    //     std::string value1;
    //     std::string value2;
    //     std::string ret_var;

    //     BinStmtVarString(stmt, op, value1, value2, ret_var);

    //     // shl, shr, sar的转换
    //     if(op == "shl") {
    //         op = "sll";
    //     } else if(op == "shr") {
    //         op = "srl";
    //     } else if(op == "sar") {
    //         op = "sra";
    //     }

    //     // sub不存在subi的情况，所以需要转换为addi
    //     if(op == "sub") {
    //         if(BinStmtWith2Imm(stmt)) {
    //             value2 = "-" + value2;
    //             op = "add";
    //         }
    //     }
    //     if( BinStmtWith2Imm(stmt)) {
    //         code += "\tli t0, " + value1 + "\n";
    //         code += "\t" + op + "i " + ret_var + ", t0, " + value2 + "\n";
    //     } else if( BinStmtWith1Symbol(stmt)) {
    //         code += "\t" + op + "i " + ret_var + ", " + value1 + ", " + value2 + "\n";
    //     } else if( BinStmtWith2Symbol(stmt)) {
    //         code += "\t" + op + " " + ret_var + ", " + value1 + ", " + value2 + "\n";
    //     }
    //     return code;
    // }
    
    // bool OpLikeEQ(std::string op) {
    //     return op == "eq" || op == "ne" || op == "lt" || op == "gt" || op == "le" || op == "ge";
    // }

    // std::string EmitOpLikeEQ(Statement* stmt) {
    //     std::string code;
    //     std::string op = stmt->binaryOpStmt.op.op;
    //     if(op == "le")  {
    //         op = "gt";
    //     } else if(op == "ge") {
    //         op = "lt";
    //     }
    //     if( BinStmtWith2Symbol(stmt)) {
    //         code += "\tsub t0, " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + reg_map[stmt->binaryOpStmt.input2.GetSymbol()] + "\n";
    //         code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0\n";
    //     } else if( BinStmtWith1Symbol(stmt)) {
    //         code += "\tsubi t0, " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
    //         code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0\n";
    //     } else if( BinStmtWith2Imm(stmt)) {
    //         code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
    //         code += "\tsubi t1, t0, " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
    //         code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t1\n";
    //     }
    //     if(stmt->binaryOpStmt.op.op == "le" || stmt->binaryOpStmt.op.op == "ge") {
    //         code += "\txori " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", 1\n";
    //     }
    //     return code;
    // }


    // std::string EmitOpLikeMUL(Statement* stmt) {
    //     std::string code;
    //     if( BinStmtWith2Symbol(stmt)) {
    //         code += "\t" + stmt->binaryOpStmt.op.op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + reg_map[stmt->binaryOpStmt.input2.GetSymbol()] + "\n";
    //     } else if( BinStmtWith1Symbol(stmt)) {
    //         code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
    //         code += "\t" + stmt->binaryOpStmt.op.op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", t0\n";
    //     } else if( BinStmtWith2Imm(stmt)) {
    //         code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
    //         code += "\tli t1, " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
    //         code += "\t" + stmt->binaryOpStmt.op.op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0, t1\n";
    //     }
    //     return code;
    // }

    // bool BinStmtWith2Symbol(Statement* stmt) {
    //     return stmt->binaryOpStmt.input1.IsSymbol() && stmt->binaryOpStmt.input2.IsSymbol();
    // }
    // bool BinStmtWith1Symbol(Statement* stmt) {
    //     if(stmt->binaryOpStmt.input1.IsSymbol() && stmt->binaryOpStmt.input2.IsImm()) {
    //         return true;
    //     } else if(stmt->binaryOpStmt.input1.IsImm() && stmt->binaryOpStmt.input2.IsSymbol()) {
    //         std::swap(stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
    //         return true;
    //     }
    //     return false;
    // }
    // bool BinStmtWith2Imm(Statement* stmt) {
    //     return stmt->binaryOpStmt.input1.IsImm() && stmt->binaryOpStmt.input2.IsImm();
    // }

    // void BinStmtVarString(Statement* stmt, std::string& op, std::string& value1, std::string& value2, std::string& ret_var) {
    //     op = stmt->binaryOpStmt.op.op;
    //     if(stmt->binaryOpStmt.input1.IsSymbol()) {
    //         value1 = reg_map[stmt->binaryOpStmt.input1.GetSymbol()];
    //     } else if(stmt->binaryOpStmt.input1.IsImm()) {
    //         value1 = stmt->binaryOpStmt.input1.GetSymbol();
    //     }
    //     if(stmt->binaryOpStmt.input2.IsSymbol()) {
    //         value2 = reg_map[stmt->binaryOpStmt.input2.GetSymbol()];
    //     } else if(stmt->binaryOpStmt.input2.IsImm()) {
    //         value2 = stmt->binaryOpStmt.input2.GetSymbol();
    //     }
    //     ret_var = reg_map[stmt->binaryOpStmt.ret_var.varName];
    // }