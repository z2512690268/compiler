#pragma once
#include "koopa.h"
#include "defs.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
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
            std::cout << item.second.var.varName << " " << KoopaVarTypeToString(item.second.var.type) << std::endl;
            if(item.second.var.type.topType != KoopaVarType::KOOPA_func) {
                code += "global " + item.second.var.varName + " = alloc " + KoopaVarTypeToString(item.second.var.type) + ", " + item.second.var.initList.GetInitString() + "\n";
            } else {
                code += "decl " + item.second.var.varName + "(";
                for(int i = 0; i < item.second.var.type.funcType.paramsType.size(); i++) {
                    code += KoopaVarTypeToString(*item.second.var.type.funcType.paramsType[i]);
                    if(i != item.second.var.type.funcType.paramsType.size() - 1) {
                        code += ", ";
                    }
                }
                code += ")";
                if(item.second.var.type.funcType.retType->topType != KoopaVarType::KOOPA_undef) {
                    code += " : " + KoopaVarTypeToString(*item.second.var.type.funcType.retType) + "\n";
                } else {
                    code += "\n";
                }
            }
        }
        for(auto& func : func_scopes) {
            code += "fun " + func->func_name + "(";
            for(int i = 0; i < func->func_param.size(); i++) {
                code += func->func_param[i].varName + " : " + KoopaVarTypeToString(func->func_param[i].type);
                if(i != func->func_param.size() - 1) {
                    code += ", ";
                }
            }
            code += ")";
            if(func->func_ret_type.topType != KoopaVarType::KOOPA_undef) {
                code += " : " + KoopaVarTypeToString(func->func_ret_type) + " {\n";
            } else {
                code += " {\n";
            }
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
                                break;
                            } else {
                                code += "  ret " + stmt->returnStmt.ret.GetSymbol() + "\n";
                            }
                            break;
                        case Statement::CALL:
                            if(stmt->callStmt.ret_var.type.topType != KoopaVarType::KOOPA_undef) {
                                code += "  " + stmt->callStmt.ret_var.varName + " = call " + stmt->callStmt.func_name.GetSymbol() + "(";
                            } else {
                                code += "  call " + stmt->callStmt.func_name.GetSymbol() + "(";
                            }
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
                            code += "  " + stmt->getptrStmt.ret_var.varName + " = getptr " + stmt->getptrStmt.varptr.varName + ", " + stmt->getptrStmt.offset.GetSymbol() + "\n";
                            break;
                        case Statement::GETELEMENTPTR:
                            code += "  " + stmt->getelementptrStmt.ret_var.varName + " = getelemptr " + stmt->getelementptrStmt.arrayptr.varName + ", " + stmt->getelementptrStmt.index.GetSymbol() + "\n";
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
    std::unordered_map<Statement*, int> local_alloc_map;
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
        return "";
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

    int GetStackOffset(std::string var) {
        int offset = 0;
        for(int i = 0; i < var.size(); i++) {
            if(var[i] == '(') {
                offset = std::stoi(var.substr(0, i));
                break;
            }
        }
        return offset;
    }

    bool CheckMapReg_Gloabl(std::string var) {
        return var[0] == '@';
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
        int imm_num = std::stoi(imm);
        if(imm_num < -2048 || imm_num > 2047) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, imm);
            code += "\t" + op.substr(0, op.size() - 1) + " " + rd + ", " + rs1 + ", " + temp_reg + "\n";
            FreeTempReg();
        } else {
            code += "\t" + op + " " + rd + ", " + rs1 + ", " + imm + "\n";
        }
    }

    // S-Type : sw, sh, sb
    void EmitSTypeOperation(std::string op, std::string rs1, std::string rs2, std::string imm) {
        int imm_num = std::stoi(imm);
        if(imm_num < -2048 || imm_num > 2047) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, imm);
            EmitRTypeOperation("add", temp_reg, temp_reg, rs2);
            code += "\t" + op + " " + rs1 + ", 0(" + temp_reg + ")\n";
            FreeTempReg();
        } else {
            code += "\t" + op + " " + rs1 + ", " + imm + "(" + rs2 + ")\n";
        }
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

    void EmitLoadGlobl(std::string reg, std::string var) {
        code += "\tla " + reg + ", " + var + "\n";
        code += "\tlw " + reg + ", 0(" + reg + ")\n";
    }

    void EmitStoreGlobl(std::string reg, std::string var) {
        std::string temp_reg = GetTempReg();
        code += "\tla " + temp_reg + ", " + var + "\n";
        code += "\tsw " + reg + ", 0(" + temp_reg + ")\n";
        FreeTempReg();
    }

    void EmitStore(std::string reg, std::string stack_sp) {
        code += "\tsw " + reg + ", " + stack_sp + "\n";
    }

    std::string GetLabel(std::string func_name, std::string label) {
        if(label[0] == '@') {
            return func_name.substr(1) + "_u_" + label.substr(1);
        } else {
            return func_name.substr(1) + "_t_" + label.substr(1);
        }
    }

    void EmitLabel(std::string func_name, std::string label) {
        code += GetLabel(func_name, label) + ":\n";
    }

    void EmitExplain(std::string explain) {
        code += "# " + explain + "\n";
    }

    void Emit2RegInst(std::string op, std::string reg1, std::string reg2) {
        code += "\t" + op + " " + reg1 + ", " + reg2 + "\n";
    }

    void EmitUsedRegMap_IfStackVar(std::string& value_map) {
        if(CheckMapReg_Stack(value_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value_map);
            value_map = temp_reg;
        } else if(CheckMapReg_Gloabl(value_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoadGlobl(temp_reg, value_map.substr(1));
            value_map = temp_reg;
        }
    }

    void EmitUsedRegMap_All(std::string& value_map) {
        if(CheckMapReg_Stack(value_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, value_map);
            value_map = temp_reg;
        } else if(CheckMapReg_Gloabl(value_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoadGlobl(temp_reg, value_map.substr(1));
            value_map = temp_reg;
        } else if(CheckMapReg_Imm(value_map)) {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, value_map);
            value_map = temp_reg;
        }
    }

    // add, and, or, xor
    bool IsSymmetryOp(std::string op) {
        return op == "add" || op == "and" || op == "or" || op == "xor";
    }
    void EmitSymmetryOp(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = GetRegMap(ret_var);
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

        EmitUsedRegMap_IfStackVar(value1_map);
        EmitUsedRegMap_IfStackVar(value2_map);

        if(CheckMapReg_Reg(value1_map) && CheckMapReg_Reg(value2_map)) {
            EmitRTypeOperation(op, ret_reg, value1_map, value2_map);
        } else if(CheckMapReg_Reg(value1_map) && CheckMapReg_Imm(value2_map)) {
            int imm = std::stoi(value2_map);
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
        std::string ret_map = GetRegMap(ret_var);
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
        EmitUsedRegMap_IfStackVar(value1_map);
        EmitUsedRegMap_IfStackVar(value2_map);

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
        std::string ret_map = GetRegMap(ret_var);
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
        EmitUsedRegMap_IfStackVar(value1_map);
        EmitUsedRegMap_IfStackVar(value2_map);

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
        std::string ret_map = GetRegMap(ret_var);
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

        int temp_reg_count = GetTempRegCount();
        EmitUsedRegMap_IfStackVar(value1_map);
        EmitUsedRegMap_IfStackVar(value2_map);

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
            EmitITypeOperation("xori", ret_reg, ret_reg, "1");
        } else if(op == "ge") {
            EmitRTypeOperation("slt", ret_reg, ret_reg, "x0");
            EmitITypeOperation("xori", ret_reg, ret_reg, "1");
        }
        ResetTempReg(temp_reg_count);
    }

    // mul, div, rem
    bool IsMulOp(std::string op) {
        return op == "mul" || op == "div" || op == "mod";
    }
    void EmitMul(std::string op, std::string ret_var, KoopaSymbol value1, KoopaSymbol value2) {
        std::string ret_map = reg_map[ret_var];
        if(op == "mod") op = "rem";
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
        EmitUsedRegMap_IfStackVar(value1_map);
        EmitUsedRegMap_IfStackVar(value2_map);

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

    void EmitBranch(KoopaSymbol cond, std::string true_label, std::string false_label) {
        std::string cond_map = GetRegMap(cond);
        if(CheckMapReg_Reg(cond_map)) {
            EmitBranch_Reg(cond_map, true_label, false_label);
        } else if(CheckMapReg_Stack(cond_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, cond_map);
            EmitBranch_Reg(temp_reg, true_label, false_label);
            FreeTempReg();
        } else if(CheckMapReg_Imm(cond_map)) {
            // TODO: 优化常数条件的情况
        }
    }
    void EmitBranch_Reg(std::string cond_reg, std::string true_label, std::string false_label) {
        EmitSBTypeOperation("bne", cond_reg, "x0", true_label);
        EmitUJTypeOperation("jal", "x0", false_label);
    }


    void EmitKoopaLoadStmt(std::string ret_var, std::string addr) {
        std::string ret_map = GetRegMap(ret_var);
        if(CheckMapReg_Reg(ret_map)) {
            EmitKoopaLoadStmt_RetReg(ret_map, addr);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitKoopaLoadStmt_RetReg(temp_reg, addr);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }

    void EmitKoopaLoadStmt_RetReg(std::string ret_reg, std::string addr) {
        // std::cout << addr << std::endl;
        std::string addr_map = GetRegMap(addr);
        // std::cout << addr_map << std::endl;

        if(CheckMapReg_Stack(addr_map)) {
            EmitLoad(ret_reg, addr_map);
            EmitLoad(ret_reg, "0(" + ret_reg + ")");
        } else if(CheckMapReg_Gloabl(addr_map)) {
            EmitLoadGlobl(ret_reg, addr_map.substr(1));
        } else {
            std::cerr << "EmitKoopaLoadStmt_RetReg: addr_map is not stack var" << std::endl;
        }
    }

    void EmitKoopaGetptrStmt(KoopaVar ret_var, KoopaVar varptr, KoopaSymbol offset, int unit) {
        std::string ret_map = GetRegMap(ret_var);
        if(CheckMapReg_Reg(ret_map)) {
            EmitKoopaGetptrStmt_RetReg(ret_map, varptr, offset, unit);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitKoopaGetptrStmt_RetReg(temp_reg, varptr, offset, unit);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }

    void EmitKoopaGetptrStmt_RetReg(std::string ret_reg, KoopaVar varptr, KoopaSymbol offset, int unit) {
        std::string varptr_map = GetRegMap(varptr);
        std::string offset_map = GetRegMap(offset);

        int temp_reg_count = GetTempRegCount();
        EmitUsedRegMap_IfStackVar(varptr_map);

        // offset * stmt->getptrStmt.varptr.type.ptrType.type->Size()
        EmitUsedRegMap_IfStackVar(offset_map);
        
        if(CheckMapReg_Imm(offset_map)) {
            int off_int = offset.GetImm() * unit;
            if(off_int < -2048 || off_int > 2047) {
                std::string temp_reg = GetTempReg();
                EmitImm(temp_reg, std::to_string(off_int));
                EmitRTypeOperation("add", ret_reg, varptr_map, temp_reg);
                FreeTempReg();
            } else {
                EmitITypeOperation("addi", ret_reg, varptr_map, std::to_string(off_int));
            }
        } else {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, std::to_string(unit));
            EmitRTypeOperation("mul", ret_reg, offset_map, temp_reg);
            EmitRTypeOperation("add", ret_reg, ret_reg, varptr_map);
        }

        ResetTempReg(temp_reg_count);
    }

    void EmitKoopaGetelementptrStmt(KoopaVar ret_var, KoopaVar arrayptr, KoopaSymbol offset, int unit) {
        std::string ret_map = GetRegMap(ret_var);
        if(CheckMapReg_Reg(ret_map)) {
            EmitKoopaGetelementptrStmt_RetReg(ret_map, arrayptr, offset, unit);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitKoopaGetelementptrStmt_RetReg(temp_reg, arrayptr, offset, unit);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }

    void EmitKoopaGetelementptrStmt_RetReg(std::string ret_reg, KoopaVar arrayptr, KoopaSymbol offset, int unit) {
        std::string arrayptr_map = GetRegMap(arrayptr);

        int temp_reg_count = GetTempRegCount();
        // EmitUsedRegMap_IfStackVar(arrayptr_map);
        if(CheckMapReg_Stack(arrayptr_map)) {
            std::string temp_reg = GetTempReg();
            EmitLoad(temp_reg, arrayptr_map);
            arrayptr_map = temp_reg;
        } else if(CheckMapReg_Gloabl(arrayptr_map)) {
            std::string temp_reg = GetTempReg();
            // EmitLoadGlobl(, );
            code += "\tla " + temp_reg + ", " + arrayptr_map.substr(1) + "\n";
            arrayptr_map = temp_reg;
        }
        // offset * stmt->getelementptrStmt.arrayptr.type.ptrType.type->Size()
        std::string offset_map = GetRegMap(offset);
        EmitUsedRegMap_IfStackVar(offset_map);

        if(CheckMapReg_Imm(offset_map)) {
            int off_int = offset.GetImm() * unit;
            if(off_int < -2048 || off_int > 2047) {
                std::string temp_reg = GetTempReg();
                EmitImm(temp_reg, std::to_string(off_int));
                EmitRTypeOperation("add", ret_reg, arrayptr_map, temp_reg);
                FreeTempReg();
            } else {
                EmitITypeOperation("addi", ret_reg, arrayptr_map, std::to_string(off_int));
            }
        } else {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, std::to_string(unit));
            EmitRTypeOperation("mul", ret_reg, offset_map, temp_reg);
            EmitRTypeOperation("add", ret_reg, ret_reg, arrayptr_map);
        }


        ResetTempReg(temp_reg_count);
    }

    void EmitKoopaAllocStmt(std::string ret_var, int stack_sp) {
        std::string ret_map = GetRegMap(ret_var);
        if(CheckMapReg_Reg(ret_map)) {
            EmitKoopaAllocStmt_RetReg(ret_map, stack_sp);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitKoopaAllocStmt_RetReg(temp_reg, stack_sp);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }

    void EmitKoopaAllocStmt_RetReg(std::string ret_reg, int stack_sp) {
        EmitITypeOperation("addi", ret_reg, "sp", std::to_string(stack_sp));
    }

    void EmitKoopaStoreStmt_Symbol(std::string addr, std::string val_var) {
        std::string addr_map = GetRegMap(addr);
        std::string val_map = GetRegMap(val_var);

        if(CheckMapReg_Stack(addr_map)) {
            std::string addr_temp_reg = GetTempReg();
            EmitLoad(addr_temp_reg, addr_map);
            std::string addr_stack = "0(" + addr_temp_reg + ")";
            if(CheckMapReg_Reg(val_map)) {
                EmitStore(val_map, addr_stack);
            } else if(CheckMapReg_Stack(val_map)) {
                std::string temp_reg = GetTempReg();
                EmitLoad(temp_reg, val_map);
                EmitStore(temp_reg, addr_stack);
                FreeTempReg();
            } else if(CheckMapReg_Imm(val_map)) {
                std::string temp_reg = GetTempReg();
                EmitImm(temp_reg, val_map);
                EmitStore(temp_reg, addr_stack);
                FreeTempReg();               
            }
            FreeTempReg();               
        } else if(CheckMapReg_Gloabl(addr)) {
            if(CheckMapReg_Reg(val_map)) {
                EmitStoreGlobl(val_map, addr_map.substr(1));
            } else if(CheckMapReg_Stack(val_map)) {
                std::string temp_reg = GetTempReg();
                EmitLoad(temp_reg, val_map);
                EmitStoreGlobl(temp_reg, addr_map.substr(1));
                FreeTempReg();
            } else if(CheckMapReg_Imm(val_map)) {
                std::string temp_reg = GetTempReg();
                EmitImm(temp_reg, val_map);
                EmitStoreGlobl(temp_reg, addr_map.substr(1));
                FreeTempReg();               
            }
        }
        else {
            std::cerr << "EmitKoopaStoreStmt: addr_map is not stack var" << std::endl;
        }
    }

    int EmitKoopaStoreStmt_InitList_Recursive(int offset, std::string addr_stack, KoopaInitList init_list) {
        int ret = 0;
        if(init_list.GetInitListType() == "int") {
            std::string temp_reg = GetTempReg();
            EmitImm(temp_reg, std::to_string(init_list.initInt));
            EmitStore(temp_reg, std::to_string(offset) + addr_stack);
            FreeTempReg();
            ret = 4;
        } else if(init_list.GetInitListType() == "zeroinit") {
            EmitStore("0", std::to_string(offset) + addr_stack);
            ret = 4;
        } else if(init_list.GetInitListType() == "aggregate") {
            for(auto& list : init_list.initList) {
                ret += EmitKoopaStoreStmt_InitList_Recursive(offset + ret, addr_stack, list);
            }
        }
        return ret;
    }

    void EmitKoopaStoreStmt_InitList(std::string addr, KoopaInitList init_list) {
        std::string addr_map = GetRegMap(addr);

        std::cout << init_list.GetInitString() << std::endl;
        int temp_reg_count = GetTempRegCount();

        if(CheckMapReg_Stack(addr_map)) {
            std::string addr_temp_reg = GetTempReg();
            EmitLoad(addr_temp_reg, addr_map);
            std::string addr_stack = "(" + addr_temp_reg + ")";
            int cur_offset = 0;
            EmitKoopaStoreStmt_InitList_Recursive(cur_offset, addr_stack, init_list);
        } else {
            std::cerr << "EmitKoopaStoreStmt: addr_map is not stack var" << std::endl;
        }
        ResetTempReg(temp_reg_count);
    }

    void EmitGlobalInitList(KoopaInitList initList) {
        if(initList.GetInitListType() == "int") {
            code += "\t.word " + initList.GetInitString() + "\n";
        } else if(initList.GetInitListType() == "zeroinit") {
            code += "\t.word 0\n";
        } else if(initList.GetInitListType() == "aggregate") {
            for(auto& list : initList.initList) {
                EmitGlobalInitList(list);
            }
        }
    } 

    void EmitCallStmt(KoopaVar ret_var, std::string func_name, std::vector<KoopaSymbol> args) {
        std::string ret_map = GetRegMap(ret_var);
        if(CheckMapReg_Reg(ret_map)) {
            EmitCallStmt_RetReg(ret_map, func_name, args);
        } else if(CheckMapReg_Stack(ret_map)) {
            std::string temp_reg = GetTempReg();
            EmitCallStmt_RetReg(temp_reg, func_name, args);
            EmitStore(temp_reg, ret_map);
            FreeTempReg();
        }
    }

    void EmitCallStmt_RetReg(std::string ret_reg, std::string func_name, std::vector<KoopaSymbol> args) {
        int temp_reg_count = GetTempRegCount();
        std::string arg_reg;
        for(int i = 0; i < args.size(); ++i) {
            if(i < 8) {
                arg_reg = GetRegMap(args[i]);
                if(CheckMapReg_Reg(arg_reg)) {
                    Emit2RegInst("mv", "a" + std::to_string(i), arg_reg);
                } else if(CheckMapReg_Stack(arg_reg)) {
                    EmitLoad("a" + std::to_string(i), arg_reg);
                } else if(CheckMapReg_Imm(arg_reg)) {
                    EmitImm("a" + std::to_string(i), arg_reg);
                }
            } else {
                arg_reg = GetRegMap(args[i]);
                if(CheckMapReg_Reg(arg_reg)) {
                    EmitStore(arg_reg, std::to_string((i - 8) * 4) + "(sp)");
                } else if(CheckMapReg_Stack(arg_reg)) {
                    std::string temp_reg = GetTempReg();
                    EmitLoad(temp_reg, arg_reg);
                    EmitStore(temp_reg, std::to_string((i - 8) * 4) + "(sp)");
                    FreeTempReg();
                } else if(CheckMapReg_Imm(arg_reg)) {
                    std::string temp_reg = GetTempReg();
                    EmitImm(temp_reg, arg_reg);
                    EmitStore(temp_reg, std::to_string((i - 8) * 4) + "(sp)");
                    FreeTempReg();
                }
            }
        }
        EmitUJTypeOperation("jal", "ra", func_name.substr(1));
        Emit2RegInst("mv", ret_reg, "a0");
        ResetTempReg(temp_reg_count);
    }

    virtual std::string GenerateCode(KoopaIR* ir) {
        std::cout << "AAAAA" << std::endl;
        global_scope = ir->global_scope;
        func_scopes = ir->func_scopes;

        // 全局变量定义和全局函数声明
        code += "\t.data\n";
        for(auto& item : global_scope.symbolTable.var_table) {
            if(item.second.var.type.topType != KoopaVarType::KOOPA_func) {
                std::string name = item.second.var.varName.substr(1);
                code += "\t.global " + name + "\n";
                code += name + ":\n";
                EmitGlobalInitList(item.second.var.initList);
                reg_map[item.second.var.varName] = item.second.var.varName;
            } 
        }
        code += "\t.text\n";  // .text

        for(auto& func : func_scopes) {
            std::unordered_map<std::string, std::string> reg_map_backup = reg_map;
            local_alloc_map.clear();
            code += "\t.global " + func->func_name.substr(1) + "\n";
            code += func->func_name.substr(1) + ":\n";
            
            int stack_pos = 0;
            int ret_stack_count = func->nested_call > 0 ? 4 : 0;

            int nested_arg_count = func->max_nested_call_varnum > 8 ? (func->max_nested_call_varnum - 8) * 4: 0;
            
            // 多余函数参数栈空间
            stack_pos += nested_arg_count;
            // alloc分配的栈空间
            for(auto& block : func->basicBlocks) {
                for(auto& stmt : block->statements) {
                    if(stmt->type == Statement::ALLOC) {
                        local_alloc_map[stmt] = stack_pos;
                        stack_pos += stmt->allocStmt.var.type.Size();
                    }
                }
            }

            // 局部变量栈空间
            for(auto& item : func->symbolTable.var_table) {
                reg_map[item.second.var.varName] = 
                                    std::to_string(stack_pos) + "(sp)";
                stack_pos += 4;
            }

            // 先空出ra寄存器空间
            stack_pos += ret_stack_count;
            
            // 向上取整16
            stack_pos = (stack_pos + 15) / 16 * 16;

            //映射八个参数
            for(int i = 0; i < 8 && i < func->func_param.size(); ++i) {
                reg_map[func->func_param[i].varName] = "a" + std::to_string(i);
            }

            // 映射大于8的函数参数
            for(int i = 8; i < func->func_param.size(); ++i) {
                reg_map[func->func_param[i].varName] = 
                                    std::to_string(stack_pos + i * 4 - 32) + "(sp)";
            }

            if(stack_pos > 0) {
                EmitITypeOperation("addi", "sp", "sp", "-" + std::to_string(stack_pos));
            }
            if(ret_stack_count) {
                EmitStore("ra", std::to_string(stack_pos - 4) + "(sp)");
            }

            for(auto& item : reg_map) {
                std::cout << item.first << " " << item.second << std::endl;
            }
            for(auto& item : local_alloc_map) {
                std::cout << "~~~" << item.first->allocStmt.var.varName << " " << item.second << std::endl;
            }
            for(auto& block : func->basicBlocks) {
                std::cout << block->label << std::endl;
                ++label_count;
                label_map[block->label] = label_count;
                EmitLabel(func->func_name, block->label);
                // OPERATION
                std::string op;
                std::string value1;
                std::string value2;
                std::string ret_var;
                for(auto& stmt : block->statements) {
                    std::cout << stmt->type << std::endl;
                    std::string explain;
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
                            if(stmt->returnStmt.ret.GetSymbol() == "") {
                                EmitExplain("  ret\t");
                            } else {
                                EmitExplain("  ret " + stmt->returnStmt.ret.GetSymbol());
                            }
                            if(stmt->returnStmt.ret.GetSymbol() != "") {
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
                            }
                            if(ret_stack_count) {
                                EmitLoad("ra", std::to_string(stack_pos - 4) + "(sp)");
                            }
                            if(stack_pos > 0) {
                                EmitRTypeOperation("addi", "sp", "sp", std::to_string(stack_pos));
                            }
                            code += "\tret\n";
                            break;
                        case Statement::CALL:
                            explain += "  " + stmt->callStmt.ret_var.varName + " = call " + stmt->callStmt.func_name.GetSymbol() + "(";
                            for(int i = 0; i < stmt->callStmt.params.size(); i++) {
                                explain += stmt->callStmt.params[i].GetSymbol();
                                if(i != stmt->callStmt.params.size() - 1) {
                                    explain += ", ";
                                }
                            }
                            explain += ")";
                            EmitExplain(explain);
                            EmitCallStmt(stmt->callStmt.ret_var, stmt->callStmt.func_name.GetSymbol(), stmt->callStmt.params);
                            break;
                        case Statement::BRANCH:
                            EmitExplain("  br " + stmt->branchStmt.cond.GetSymbol() + ", " + stmt->branchStmt.true_label + ", " + stmt->branchStmt.false_label);
                            EmitBranch(stmt->branchStmt.cond, 
                                GetLabel(func->func_name, stmt->branchStmt.true_label), 
                                GetLabel(func->func_name, stmt->branchStmt.false_label)); 
                            break;
                        case Statement::JUMP:
                            explain = "  jump " + stmt->jumpStmt.label;
                            EmitExplain(explain);
                            EmitUJTypeOperation("jal", "x0", GetLabel(func->func_name, stmt->jumpStmt.label));
                            break;
                        case Statement::ALLOC:
                            explain = "  " + stmt->allocStmt.var.varName + " = alloc " + KoopaVarTypeToString(stmt->allocStmt.var.type);
                            EmitExplain(explain);
                            EmitKoopaAllocStmt(stmt->allocStmt.var.varName, local_alloc_map[stmt]);
                            break;
                        case Statement::LOAD:
                            explain = "  " + stmt->loadStmt.var.varName + " = load " + stmt->loadStmt.addr.GetSymbol();
                            std::cout << explain << std::endl;
                            EmitExplain(explain);
                            EmitKoopaLoadStmt(stmt->loadStmt.var.varName, stmt->loadStmt.addr.GetSymbol());
                            break;
                        case Statement::STORE:
                            if(stmt->storeStmt.IsSymbol()){
                                explain += "  store " + stmt->storeStmt.symbol.GetSymbol() + ", " + stmt->storeStmt.addr.GetSymbol();
                            } else if(stmt->storeStmt.IsInit()) {
                                explain += "  store " + stmt->storeStmt.initList.GetInitString() + ", " + stmt->storeStmt.addr.GetSymbol();
                            }
                            EmitExplain(explain);
                            if(stmt->storeStmt.IsSymbol()) {
                                EmitKoopaStoreStmt_Symbol(stmt->storeStmt.addr.GetSymbol(), stmt->storeStmt.symbol.GetSymbol());
                            } else if(stmt->storeStmt.IsInit()) {
                                EmitKoopaStoreStmt_InitList(stmt->storeStmt.addr.GetSymbol(), stmt->storeStmt.initList);
                            }
                            break;
                        case Statement::GETPTR:
                            explain = "  " + stmt->getptrStmt.ret_var.varName + " = getptr " + stmt->getptrStmt.varptr.varName + ", " + stmt->getptrStmt.offset.GetSymbol();

                            EmitExplain(explain);
                            EmitKoopaGetptrStmt(stmt->getptrStmt.ret_var, stmt->getptrStmt.varptr, stmt->getptrStmt.offset, stmt->getptrStmt.varptr.type.ptrType.type->Size());
                            break;
                        case Statement::GETELEMENTPTR:
                            explain = "  " + stmt->getelementptrStmt.ret_var.varName + " = getelemptr " + stmt->getelementptrStmt.arrayptr.varName + ", " + stmt->getelementptrStmt.index.GetSymbol();
                            EmitExplain(explain);
                            EmitKoopaGetelementptrStmt(stmt->getelementptrStmt.ret_var, stmt->getelementptrStmt.arrayptr, stmt->getelementptrStmt.index, stmt->getelementptrStmt.arrayptr.type.arrayType.type->Size());
                            break;
                    }
                }
            }
            reg_map = reg_map_backup;
        }
        return code;
    }
};