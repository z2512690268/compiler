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
                            code += "\t" + stmt->binaryOpStmt.ret_var.varName + " = " + stmt->binaryOpStmt.op.op + " " + stmt->binaryOpStmt.input1.GetSymbol() + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
                            break;
                        case Statement::RETURN:
                            code += "\tret " + stmt->returnStmt.ret.GetSymbol() + "\n";
                            break;
                        case Statement::CALL:
                            code += "\t" + stmt->callStmt.ret_var.varName + " = call " + stmt->callStmt.func_name.GetSymbol() + "(";
                            for(int i = 0; i < stmt->callStmt.params.size(); i++) {
                                code += stmt->callStmt.params[i].GetSymbol();
                                if(i != stmt->callStmt.params.size() - 1) {
                                    code += ", ";
                                }
                            }
                            code += ")\n";
                            break;
                        case Statement::BRANCH:
                            code += "\tbr " + stmt->branchStmt.cond.GetSymbol() + ", " + stmt->branchStmt.true_label + ", " + stmt->branchStmt.false_label + "\n";
                            break;
                        case Statement::JUMP:
                            code += "\tjump " + stmt->jumpStmt.label + "\n";
                            break;
                        case Statement::ALLOC:
                            code += "\t" + stmt->allocStmt.var.varName + " = alloc " + KoopaVarTypeToString(stmt->allocStmt.var.type) + "\n";
                            break;
                        case Statement::LOAD:
                            code += "\t" + stmt->loadStmt.var.varName + " = load " + stmt->loadStmt.addr.GetSymbol() + "\n";
                            break;
                        case Statement::STORE:
                            if(stmt->storeStmt.IsSymbol()){
                                code += "\tstore " + stmt->storeStmt.symbol.GetSymbol() + ", " + stmt->storeStmt.addr.GetSymbol() + "\n";
                            } else if(stmt->storeStmt.IsInit()) {
                                code += "\tstore " + stmt->storeStmt.initList.GetInitString() + ", " + stmt->storeStmt.addr.GetSymbol() + "\n";
                            }
                            break;
                        case Statement::GETPTR:
                            code += "\tgetptr " + stmt->getptrStmt.ret_var.varName + " = " + stmt->getptrStmt.varptr.varName + ", " + stmt->getptrStmt.offset.GetSymbol() + "\n";
                            break;
                        case Statement::GETELEMENTPTR:
                            code += "\tgetelementptr " + stmt->getelementptrStmt.ret_var.varName + " = " + stmt->getelementptrStmt.arrayptr.varName + ", " + stmt->getelementptrStmt.index.GetSymbol() + "\n";
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

    bool OpLikeADD(std::string op) {
        return op == "add" || op == "sub" || op == "and" || op == "or" || op == "xor" || op == "shl" || op == "shr" || op == "sar";
    }

    std::string EmitOpLikeADD(Statement* stmt) {
        std::string code;
        std::string op = stmt->binaryOpStmt.op.op;
        if(op == "shl") {
            op = "sll";
        } else if(op == "shr") {
            op = "srl";
        } else if(op == "sar") {
            op = "sra";
        }
        if( BinStmtWith2Symbol(stmt)) {
            code += "\t" + op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + reg_map[stmt->binaryOpStmt.input2.GetSymbol()] + "\n";
        } else if( BinStmtWith1Symbol(stmt)) {
            code += "\t" + op + "i " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
        } else if( BinStmtWith2Imm(stmt)) {
            code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
            code += "\t" + op + "i " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0, " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
        }
        return code;
    }
    
    bool OpLikeEQ(std::string op) {
        return op == "eq" || op == "ne" || op == "lt" || op == "gt" || op == "le" || op == "ge";
    }

    std::string EmitOpLikeEQ(Statement* stmt) {
        std::string code;
        std::string op = stmt->binaryOpStmt.op.op;
        if(op == "le")  {
            op = "gt";
        } else if(op == "ge") {
            op = "lt";
        }
        if( BinStmtWith2Symbol(stmt)) {
            code += "\tsub t0, " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + reg_map[stmt->binaryOpStmt.input2.GetSymbol()] + "\n";
            code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0\n";
        } else if( BinStmtWith1Symbol(stmt)) {
            code += "\tsubi t0, " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
            code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t0\n";
        } else if( BinStmtWith2Imm(stmt)) {
            code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
            code += "\tsubi t1, t0, " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
            code += "\ts" + op + "z " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", t1\n";
        }
        if(stmt->binaryOpStmt.op.op == "le" || stmt->binaryOpStmt.op.op == "ge") {
            code += "\txori " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", 1\n";
        }
        return code;
    }

    bool OpLikeMUL(std::string op) {
        return op == "mul" || op == "div" || op == "rem";
    }

    std::string EmitOpLikeMUL(Statement* stmt) {
        std::string code;
        if( BinStmtWith2Symbol(stmt)) {
            code += "\t" + stmt->binaryOpStmt.op.op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", " + reg_map[stmt->binaryOpStmt.input2.GetSymbol()] + "\n";
        } else if( BinStmtWith1Symbol(stmt)) {
            code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
            code += "\t" + stmt->binaryOpStmt.op.op + " " + reg_map[stmt->binaryOpStmt.ret_var.varName] + ", " + reg_map[stmt->binaryOpStmt.input1.GetSymbol()] + ", t0\n";
        } else if( BinStmtWith2Imm(stmt)) {
            code += "\tli t0, " + stmt->binaryOpStmt.input1.GetSymbol() + "\n";
            code += "\tli t1, " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
            code += "\t" + stmt->binaryOpStmt.op.op + " t0, t1\n";
        }
        return code;
    }

    bool BinStmtWith2Symbol(Statement* stmt) {
        return stmt->binaryOpStmt.input1.IsSymbol() && stmt->binaryOpStmt.input2.IsSymbol();
    }
    bool BinStmtWith1Symbol(Statement* stmt) {
        if(stmt->binaryOpStmt.input1.IsSymbol() && stmt->binaryOpStmt.input2.IsImm()) {
            return true;
        } else if(stmt->binaryOpStmt.input1.IsImm() && stmt->binaryOpStmt.input2.IsSymbol()) {
            std::swap(stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
            return true;
        }
        return false;
    }
    bool BinStmtWith2Imm(Statement* stmt) {
        return stmt->binaryOpStmt.input1.IsImm() && stmt->binaryOpStmt.input2.IsImm();
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
            code += "\t.global " + func->func_name.substr(1) + "\n";
            code += func->func_name.substr(1) + ":\n";
            // 不多于8个局部变量的函数，使用寄存器存储
            int s_count = 0;
            for(auto& item : func->symbolTable.var_table) {
                if(item.second.var.type == KoopaVarType(KoopaVarType::KOOPA_INT32)) {
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
                        case Statement::OPERATION:
                            if(OpLikeADD(stmt->binaryOpStmt.op.op)) {
                                code += EmitOpLikeADD(stmt);
                            } else if(OpLikeEQ(stmt->binaryOpStmt.op.op)) {
                                code += EmitOpLikeEQ(stmt);
                            } else if(OpLikeMUL(stmt->binaryOpStmt.op.op)) {
                                code += EmitOpLikeMUL(stmt);
                            }
                            break;
                        case Statement::RETURN:
                            if(stmt->returnStmt.ret.IsSymbol()) {
                                code += "\taddi a0, " + reg_map[stmt->returnStmt.ret.GetSymbol()] + ", 0" + "\n";
                            } else if(stmt->returnStmt.ret.IsImm()) {
                                code += "\tli a0, " + stmt->returnStmt.ret.GetSymbol() + "\n";
                            }
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
};