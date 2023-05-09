#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace RISCV {

    struct RiscvInstruction { 
        virtual uint32_t GetCode() = 0;
    };

    struct R_Type_Instruction : public RiscvInstruction{
        uint32_t func7 : 7;
        uint32_t rs2 : 5;
        uint32_t rs1 : 1;
        uint32_t func3 : 3;
        uint32_t rd : 5;
        uint32_t opcode : 7;

        uint32_t GetCode() {
            uint32_t code = 0;
            code |= opcode;
            code |= (rd << 7);
            code |= (func3 << 12);
            code |= (rs1 << 15);
            code |= (rs2 << 20);
            code |= (func7 << 25);
            return code;
        }
    };

    struct ADD_Instruciton : public R_Type_Instruction {
        ADD_Instruciton() {
            opcode = 0x33;
            func3 = 0x0;
            func7 = 0x0;
        }
    };

    struct SUB_Instruciton : public R_Type_Instruction {
        SUB_Instruciton() {
            opcode = 0x33;
            func3 = 0x0;
            func7 = 0x20;
        }
    };

    struct XOR_Instruciton : public R_Type_Instruction {
        XOR_Instruciton() {
            opcode = 0x33;
            func3 = 0x4;
            func7 = 0x0;
        }
    };

    struct OR_Instruciton : public R_Type_Instruction {
        OR_Instruciton() {
            opcode = 0x33;
            func3 = 0x6;
            func7 = 0x0;
        }
    };

    struct AND_Instruciton : public R_Type_Instruction {
        AND_Instruciton() {
            opcode = 0x33;
            func3 = 0x7;
            func7 = 0x0;
        }
    };

    struct SLL_Instruciton : public R_Type_Instruction {
        SLL_Instruciton() {
            opcode = 0x33;
            func3 = 0x1;
            func7 = 0x0;
        }
    };

    struct SRL_Instruciton : public R_Type_Instruction {
        SRL_Instruciton() {
            opcode = 0x33;
            func3 = 0x5;
            func7 = 0x0;
        }
    };

    struct SRA_Instruciton : public R_Type_Instruction {
        SRA_Instruciton() {
            opcode = 0x33;
            func3 = 0x5;
            func7 = 0x20;
        }
    };

    struct SLT_Instruciton : public R_Type_Instruction {
        SLT_Instruciton() {
            opcode = 0x33;
            func3 = 0x2;
            func7 = 0x0;
        }
    };

    struct SLTU_Instruciton : public R_Type_Instruction {
        SLTU_Instruciton() {
            opcode = 0x33;
            func3 = 0x3;
            func7 = 0x0;
        }
    };

    struct I_Type_Instruction : public RiscvInstruction{
        uint32_t imm : 12;
        uint32_t rs1 : 5;
        uint32_t func3 : 3;
        uint32_t rd : 5;
        uint32_t opcode : 7;

        uint32_t GetCode() {
            uint32_t code = 0;
            code |= opcode;
            code |= (rd << 7);
            code |= (func3 << 12);
            code |= (rs1 << 15);
            code |= (imm << 20);
            return code;
        }
    };

    struct ADDI_Instruciton : public I_Type_Instruction {
        ADDI_Instruciton() {
            opcode = 0x13;
            func3 = 0x0;
        }
    };

    struct XORI_Instruciton : public I_Type_Instruction {
        XORI_Instruciton() {
            opcode = 0x13;
            func3 = 0x4;
        }
    };

    struct ORI_Instruciton : public I_Type_Instruction {
        ORI_Instruciton() {
            opcode = 0x13;
            func3 = 0x6;
        }
    };

    struct ANDI_Instruciton : public I_Type_Instruction {
        ANDI_Instruciton() {
            opcode = 0x13;
            func3 = 0x7;
        }
    };

    struct SLLI_Instruciton : public I_Type_Instruction {
        SLLI_Instruciton() {
            opcode = 0x13;
            func3 = 0x1;
        }
    };

    struct SRLI_Instruciton : public I_Type_Instruction {
        SRLI_Instruciton() {
            opcode = 0x13;
            func3 = 0x5;
        }
    };

    struct SRAI_Instruciton : public I_Type_Instruction {
        SRAI_Instruciton() {
            opcode = 0x13;
            func3 = 0x5;
            imm = 0x400;
        }
    };

    struct SLTI_Instruciton : public I_Type_Instruction {
        SLTI_Instruciton() {
            opcode = 0x13;
            func3 = 0x2;
        }
    };

    struct SLTIU_Instruciton : public I_Type_Instruction {
        SLTIU_Instruciton() {
            opcode = 0x13;
            func3 = 0x3;
        }
    };

    struct ECALL_Instruction : public I_Type_Instruction {
        ECALL_Instruction() {
            opcode = 0x73;
            func3 = 0x0;
            imm = 0x0;
            rs1 = 0x0;
            rd = 0x0;
        }
    };

    struct EBREAK_Instruction : public I_Type_Instruction {
        EBREAK_Instruction() {
            opcode = 0x73;
            func3 = 0x0;
            imm = 0x1;
            rs1 = 0x0;
            rd = 0x0;
        }
    };

    struct S_Type_Instruction : public RiscvInstruction{
        uint32_t imm : 12;
        uint32_t rs2 : 5;
        uint32_t rs1 : 5;
        uint32_t func3 : 3;
        uint32_t imm2 : 7;
        uint32_t opcode : 7;

        uint32_t GetCode() {
            uint32_t code = 0;
            code |= opcode;
            code |= (imm << 7);
            code |= (func3 << 12);
            code |= (rs1 << 15);
            code |= (rs2 << 20);
            code |= (imm2 << 25);
            return code;
        }
    };

    struct LB_Instruciton : public S_Type_Instruction {
        LB_Instruciton() {
            opcode = 0x03;
            func3 = 0x0;
        }
    };

    struct LH_Instruciton : public S_Type_Instruction {
        LH_Instruciton() {
            opcode = 0x03;
            func3 = 0x1;
        }
    };

    struct LW_Instruciton : public S_Type_Instruction {
        LW_Instruciton() {
            opcode = 0x03;
            func3 = 0x2;
        }
    };

    struct LBU_Instruciton : public S_Type_Instruction {
        LBU_Instruciton() {
            opcode = 0x03;
            func3 = 0x4;
        }
    };

    struct LHU_Instruciton : public S_Type_Instruction {
        LHU_Instruciton() {
            opcode = 0x03;
            func3 = 0x5;
        }
    };

    struct B_Type_Instruction : public RiscvInstruction{
        uint32_t imm : 12;
        uint32_t rs2 : 5;
        uint32_t rs1 : 5;
        uint32_t func3 : 3;
        uint32_t imm2 : 7;
        uint32_t opcode : 7;
    };

    struct BEQ_Instruciton : public B_Type_Instruction {
        BEQ_Instruciton() {
            opcode = 0x63;
            func3 = 0x0;
        }
    };

    struct BNE_Instruciton : public B_Type_Instruction {
        BNE_Instruciton() {
            opcode = 0x63;
            func3 = 0x1;
        }
    };

    struct BLT_Instruciton : public B_Type_Instruction {
        BLT_Instruciton() {
            opcode = 0x63;
            func3 = 0x4;
        }
    };

    struct BGE_Instruciton : public B_Type_Instruction {
        BGE_Instruciton() {
            opcode = 0x63;
            func3 = 0x5;
        }
    };

    struct BLTU_Instruciton : public B_Type_Instruction {
        BLTU_Instruciton() {
            opcode = 0x63;
            func3 = 0x6;
        }
    };

    struct BGEU_Instruciton : public B_Type_Instruction {
        BGEU_Instruciton() {
            opcode = 0x63;
            func3 = 0x7;
        }
    };

    struct U_Type_Instruction : public RiscvInstruction{
        uint32_t imm : 20;
        uint32_t rd : 5;
        uint32_t opcode : 7;

        uint32_t GetCode() {
            uint32_t code = 0;
            code |= opcode;
            code |= (rd << 7);
            code |= (imm << 12);
            return code;
        }
    };

    struct LUI_Instruciton : public U_Type_Instruction {
        LUI_Instruciton() {
            opcode = 0x37;
        }
    };

    struct AUIPC_Instruciton : public U_Type_Instruction {
        AUIPC_Instruciton() {
            opcode = 0x17;
        }
    };

    struct J_Type_Instruction : public RiscvInstruction{
        uint32_t imm : 20;
        uint32_t rd : 5;
        uint32_t opcode : 7;

        uint32_t GetCode() {
            uint32_t code = 0;
            code |= opcode;
            code |= (rd << 7);
            code |= (imm << 12);
            return code;
        }
    };

    struct JAL_Instruciton : public J_Type_Instruction {
        JAL_Instruciton() {
            opcode = 0x6F;
        }
    };

    struct JALR_Instruciton : public I_Type_Instruction {
        JALR_Instruciton() {
            opcode = 0x67;
            func3 = 0x0;
        }
    };

    struct RiscvInstructionBlock {
        std::vector<std::string> fake_codes;
        std::vector<RiscvInstruction*> codes;
    };

    struct RiscvAssembly
    {
        std::vector<RiscvInstructionBlock*> blocks;
    };

};