#pragma once
#include "koopa.h"
#include "defs.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_set>

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
                            code += "  " + stmt->binaryOpStmt.ret_var.varName + " = " + stmt->binaryOpStmt.op.op + " " + stmt->binaryOpStmt.input1.GetSymbol() + ", " + stmt->binaryOpStmt.input2.GetSymbol() + "\n";
                            break;
                        case Statement::RETURN:
                            code += "  ret " + stmt->returnStmt.ret.GetSymbol() + "\n";
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

    std::unordered_map<std::string, uint32_t> label_map;
    int label_count = 0;
    std::unordered_map<std::string, std::string> reg_map;
    std::string code;
    int cur_temp_reg = 0;
    std::unordered_set<std::string> regs_set = {
        "zero", "ra", "sp", "gp", "tp", "fp",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", 
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
        "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
        "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
        "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29",
        "x30", "x31"
    };

    bool CheckNumber(std::string input) {
        if(input[0] != '-' && !std::isdigit(input[0])) {
            return false;
        }
        for(int i = 1; i < input.size(); i++) {
            if(!std::isdigit(input[i])) {
                return false;
            }
        }
        return true;
    }

    std::string GetRegMap(KoopaSymbol input) {
        if(input.IsSymbol()) {
            return reg_map[input.GetSymbol()];
        } else if(input.IsImm()) {
            return input.GetSymbol();
        }
    }


    void EmitPush(std::string reg) {
        code += "\taddi sp, sp, -4\n";
        code += "\tsw " + reg + ", 0(sp)\n";
    }

    void EmitPop(std::string reg) {
        code += "\tlw " + reg + ", 0(sp)\n";
        code += "\taddi sp, sp, 4\n";
    }
    
    std::string GetTempReg() {
        return "t" + std::to_string(cur_temp_reg++);
    }

    int GetTempRegCount() {
        return cur_temp_reg;
    }

    void FreeTempReg() {
        cur_temp_reg--;
    }

    void ResetTempReg(int count) {
        cur_temp_reg = count;
    }
    
    bool CheckMapReg_Reg(std::string var) {
        return regs_set.find(var) != regs_set.end();
    }

    bool CheckMapReg_Stack(std::string var) {
        return var[var.size() - 1] == ')';
    }

    bool CheckMapReg_Imm(std::string var) {
        return (std::isdigit(var[0]) || var[0] == '-') && !CheckMapReg_Stack(var);
    }

  // R-Type : add, sub, and, or, xor, sll, srl, sra
    void EmitRTypeOperation(std::string op, std::string rd, std::string rs1, std::string rs2) {
        code += "\t" + op + " " + rd + ", " + rs1 + ", " + rs2 + "\n";
    }

    // I-Type : addi, andi, ori, xori, slli, srli, srai; lb, lh, lw, ld; jalr
    void EmitITypeOperation(std::string op, std::string rd, std::string rs1, std::string imm) {
        code += "\t" + op + " " + rd + ", " + rs1 + ", " + imm + "\n";
    }

    // S-Type : sw, sh, sb
    void EmitSTypeOperation(std::string op, std::string rs1, std::string rs2, std::string imm) {
        code += "\t" + op + " " + rs1 + ", " + imm + "(" + rs2 + ")\n";
    }

    // SB-Type : beq, bne, blt, bge, bltu, bgeu
    void EmitSBTypeOperation(std::string op, std::string rs1, std::string rs2, std::string imm) {
        code += "\t" + op + " " + rs1 + ", " + rs2 + ", " + imm + "\n";
    }

    // U-Type
    void EmitUTypeOperation(std::string op, std::string rd, std::string imm) {
        code += "\t" + op + " " + rd + ", " + imm + "\n";
    }

    // UJ-Type : jal
    void EmitUJTypeOperation(std::string op, std::string rd, std::string imm) {
        code += "\t" + op + " " + rd + ", " + imm + "\n";
    }

    void EmitImm(std::string reg, std::string imm) {
        code += "\tli " + reg + ", " + imm + "\n";
    }

    void EmitLoad(std::string reg, std::string stack_sp) {
        code += "\tlw " + reg + ", " + stack_sp + "\n";
    }

    void EmitStore(std::string reg, std::string stack_sp) {
        code += "\tsw " + reg + ", " + stack_sp + "\n";
    }

    void EmitLabel(std::string func_name, std::string label) {
        if(label[0] == '@') {
            code += label.substr(1) + ":\n";    
        } else {
            code += func_name.substr(1) + "_" + label.substr(1) + ":\n";
        }
    }

    void EmitExplain(std::string explain) {
        code += "// " + explain + "\n";
    }

    void Emit2RegInst(std::string op, std::string reg1, std::string reg2) {
        code += "\t" + op + " " + reg1 + ", " + reg2 + "\n";
    }

    // add, and, or, xor
    bool IsSymmetryOp(std::string op) {
        return op == "add" || op == "and" || op == "or" || op == "xor";
    }
    void EmitSymmetryOp(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(CheckMapReg_Reg(ret_map)) {
            EmitSymmetryOp_RetReg(op, ret_map, value1, value2);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitSymmetryOp_RetReg(op, temp_reg, value1, value2);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }
    void EmitSymmetryOp_RetReg(std::string op, std::string ret_reg, KoopaSymbol value1, KoopaSymbol value2) {
        std::string value1_map = GetRegMap(value1);
        std::string value2_map = GetRegMap(value2);

        int temp_reg_count = GetTempRegCount();
        std::cout << "SDAD" << temp_reg_count << std::endl;
        if(CheckMapReg_Stack(value1_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value1_map);
            value1_map = temp_reg;
        }

        if(CheckMapReg_Stack(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value2_map);
            value2_map = temp_reg;
        }

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation(op, ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            EmitITypeOperation(op + "i", ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitITypeOperation(op + "i", ret_reg, value2_map, value1_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitITypeOperation(op + "i", ret_reg, temp_reg, value2_map);
        }
        
        ResetTempReg(temp_reg_count);
    }

    // sub
    bool IsSubOp(std::string op) {
        return op == "sub";
    }
    void EmitSub(std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(CheckMapReg_Reg(ret_map)) {
            EmitSub_RetReg(ret_map, value1, value2);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitSub_RetReg(temp_reg, value1, value2);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }
    void EmitSub_RetReg(std::string ret_reg, KoopaSymbol value1, KoopaSymbol value2) {
        std::string value1_map = GetRegMap(value1);
        std::string value2_map = GetRegMap(value2);

        int temp_reg_count = GetTempRegCount();
        if(CheckMapReg_Stack(value1_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value1_map);
            value1_map = temp_reg;
        }

        if(CheckMapReg_Stack(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value2_map);
            value2_map = temp_reg;
        }

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation("sub", ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            EmitITypeOperation("addi", ret_reg, value1_map, "-" + value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Reg(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitRTypeOperation("sub", ret_reg, temp_reg, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitITypeOperation("addi", ret_reg, temp_reg, "-" + value2_map);
        }
        ResetTempReg(temp_reg_count);
    }
    
    // shl, shr, sar
    bool IsShiftOp(std::string op) {
        return op == "shl" || op == "shr" || op == "sar";
    }
    void EmitShift(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(CheckMapReg_Reg(ret_map)) {
            EmitShift_RetReg(op, ret_map, value1, value2);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitShift_RetReg(op, temp_reg, value1, value2);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }
    void EmitShift_RetReg(std::string op, std::string ret_reg, KoopaSymbol value1, KoopaSymbol value2) {
        std::string value1_map = GetRegMap(value1);
        std::string value2_map = GetRegMap(value2);

        int temp_reg_count = GetTempRegCount();
        if(CheckMapReg_Stack(value1_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value1_map);
            value1_map = temp_reg;
        }

        if(CheckMapReg_Stack(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value2_map);
            value2_map = temp_reg;
        }

        if(op == "shl") op = "sll";
        if(op == "shr") op = "srl";
        if(op == "sar") op = "sra";

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation(op, ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            EmitITypeOperation(op + "i", ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Reg(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitITypeOperation(op, ret_reg, temp_reg, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitITypeOperation(op + "i", ret_reg, temp_reg, value2_map);
        }
        ResetTempReg(temp_reg_count);
    }

    // eq, ne, lt, gt, le, ge
    bool IsCmpOp(std::string op) {
        return op == "eq" || op == "ne" || op == "lt" || op == "gt" || op == "le" || op == "ge";
    }
    void EmitCmp(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(CheckMapReg_Reg(ret_map)) {
            EmitCmp_RetReg(op, ret_map, value1, value2);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitCmp_RetReg(op, temp_reg, value1, value2);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }
    void EmitCmp_RetReg(std::string op, std::string ret_reg, KoopaSymbol value1, KoopaSymbol value2) {
        std::string value1_map = GetRegMap(value1);
        std::string value2_map = GetRegMap(value2);

        if(op == "le" || op == "ge") {
            std::swap(value1_map, value2_map);
        }

        int temp_reg_count = GetTempRegCount();
        if(CheckMapReg_Stack(value1_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value1_map);
            value1_map = temp_reg;
        }

        if(CheckMapReg_Stack(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value2_map);
            value2_map = temp_reg;
        }

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation("sub", ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            EmitITypeOperation("addi", ret_reg, value1_map, "-" + value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Reg(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitRTypeOperation("sub", ret_reg, temp_reg, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitITypeOperation("addi", ret_reg, temp_reg, "-" + value2_map);
        }

        if(op == "eq") {
            Emit2RegInst("seqz", ret_reg, ret_reg);
        } else if(op == "ne") {
            Emit2RegInst("snez", ret_reg, ret_reg);
        } else if(op == "lt") {
            EmitRTypeOperation("slt", ret_reg, ret_reg, "x0");
        } else if(op == "gt") {
            EmitRTypeOperation("sgt", ret_reg, ret_reg, "x0");
        } else if(op == "le") {
            EmitRTypeOperation("sgt", ret_reg, ret_reg, "x0");
        } else if(op == "ge") {
            EmitRTypeOperation("slt", ret_reg, ret_reg, "x0");
        }
        ResetTempReg(temp_reg_count);
    }

    // mul, div, rem
    bool IsMulOp(std::string op) {
        return op == "mul" || op == "div" || op == "rem";
    }
    void EmitMul(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(CheckMapReg_Reg(ret_map)) {
            EmitMul_RetReg(op, ret_map, value1, value2);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitMul_RetReg(op, temp_reg, value1, value2);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }
    void EmitMul_RetReg(std::string op, std::string ret_reg, KoopaSymbol value1, KoopaSymbol value2) {
        std::string value1_map = GetRegMap(value1);
        std::string value2_map = GetRegMap(value2);

        int temp_reg_count = GetTempRegCount();
        if(CheckMapReg_Stack(value1_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value1_map);
            value1_map = temp_reg;
        }

        if(CheckMapReg_Stack(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value2_map);
            value2_map = temp_reg;
        }

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation(op, ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value2_map);
            EmitRTypeOperation(op, ret_reg, value1_map, temp_reg);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Reg(value2_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value1_map);
            EmitRTypeOperation(op, ret_reg, temp_reg, value2_map);
        } else if(CheckMapReg_Imm(value1_map) && CheckMapReg_Imm(value2_map)) {
            std::string temp_reg1 = GetTempReg();
            std::string temp_reg2 = GetTempReg();
            EmitImm(temp_reg1, value1_map);
            EmitImm(temp_reg2, value2_map);
            EmitRTypeOperation(op, ret_reg, temp_reg1, temp_reg2);
        }
        ResetTempReg(temp_reg_count);
    }

    virtual std::string GenerateCode(KoopaIR* ir) {
        global_scope = ir->global_scope;
        func_scopes = ir->func_scopes;
        code += "\t.text\n";  // .text
        for(auto& func : func_scopes) {
            code += "\t.global " + func->func_name.substr(1) + "\n";
            code += func->func_name.substr(1) + ":\n";
            // 不多于8个局部变量的函数，使用寄存器存储
            int local_count = 0;
            int local_stack_count = 0;
            int nested_arg_count = func->max_nested_call_varnum > 8 ? (func->max_nested_call_varnum - 8) : 0;

            for(auto& item : func->symbolTable.var_table) {
                reg_map[item.second.var.varName] = 
                                    std::to_string((nested_arg_count + local_stack_count) * 4) + "(sp)";
                ++local_stack_count;
                // ++local_count;
            }
            int ret_stack_count = func->nested_call > 0;

            int stack_size = (local_stack_count + ret_stack_count + nested_arg_count) * 4;
            // 向上取整16v
            stack_size = (stack_size + 15) / 16 * 16;

            if(stack_size > 0) {
                EmitITypeOperation("addi", "sp", "sp", "-" + std::to_string(stack_size));
            }
            if(ret_stack_count) {
                EmitStore("ra", std::to_string(stack_size - 4) + "(sp)");
            }


            for(auto& block : func->basicBlocks) {
                ++label_count;
                label_map[block->label] = label_count;
                EmitLabel(func->func_name, block->label);
                // OPERATION
                std::string op;
                std::string value1;
                std::string value2;
                std::string ret_var;
                for(auto& stmt : block->statements) {
                    switch(stmt->type) {
                        case Statement::OPERATION:
                            EmitExplain("  " + stmt->binaryOpStmt.ret_var.varName + " = " + stmt->binaryOpStmt.op.op + " " + stmt->binaryOpStmt.input1.GetSymbol() + ", " + stmt->binaryOpStmt.input2.GetSymbol());
                            op = stmt->binaryOpStmt.op.op;
                            if(IsSymmetryOp(op)) {
                                EmitSymmetryOp(op, stmt->binaryOpStmt.ret_var.varName, stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
                            } else if(IsSubOp(op)) {
                                EmitSub(stmt->binaryOpStmt.ret_var.varName, stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
                            } else if(IsCmpOp(op)) {
                                EmitCmp(op, stmt->binaryOpStmt.ret_var.varName, stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
                            } else if(IsShiftOp(op)) {
                                EmitShift(op, stmt->binaryOpStmt.ret_var.varName, stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
                            } else if(IsMulOp(op)) {
                                EmitMul(op, stmt->binaryOpStmt.ret_var.varName, stmt->binaryOpStmt.input1, stmt->binaryOpStmt.input2);
                            }
                            break;
                        case Statement::RETURN:
                            EmitExplain("  ret " + stmt->returnStmt.ret.GetSymbol());
                            if(stmt->returnStmt.ret.IsSymbol()) {
                                std::string ret_map = GetRegMap(stmt->returnStmt.ret.GetSymbol());
                                if(CheckMapReg_Reg(ret_map)) {
                                    EmitITypeOperation("addi", "a0", ret_map, "0");
                                } else if(CheckMapReg_Stack(ret_map)) {
                                    std::string temp_reg = GetTempReg();
                                    EmitLoad(temp_reg, ret_map);
                                    EmitITypeOperation("addi", "a0", temp_reg, "0");
                                    FreeTempReg();
                                }
                            } else if(stmt->returnStmt.ret.IsImm()) {
                                EmitImm("a0", stmt->returnStmt.ret.GetSymbol());
                            }

                            if(ret_stack_count) {
                                EmitLoad("ra", std::to_string(stack_size - 4) + "(sp)");
                            }
                            if(stack_size > 0) {
                                EmitRTypeOperation("addi", "sp", "sp", std::to_string(stack_size));
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