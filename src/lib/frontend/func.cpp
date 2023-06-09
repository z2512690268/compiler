#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

// FuncDef     ::= Type IDENT "(" [FuncFParams] ")" Block;
SysyFrontend::FuncDef_Struct *SysyFrontend::FuncDef_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncDef_Struct);

    if (curToken.rule[0] == "Type")
    {
        // 创建作用域
        Scope *scope = koopaIR->AddNewScope();
        PushNameMap();

        ret_ptr->funcRetType = Type_func();
        ret_ptr->funcnameIDENT = IDENT_func();
        RESERVED_func(); //(
        if (curToken.rule[3] == "FuncFParams")
        {
            ret_ptr->funcFParams = FuncFParams_func();
            koopaIR->SetScopeFunction(ret_ptr->funcnameIDENT->identifer,
                                      ret_ptr->funcRetType->koopa_type,
                                      ret_ptr->funcFParams->koopa_params);
        }
        else
        {
            koopaIR->SetScopeFunction(ret_ptr->funcnameIDENT->identifer,
                                      ret_ptr->funcRetType->koopa_type);
        }
        RESERVED_func(); //)
        func_list.push_back(ret_ptr);
        ret_ptr->Block = Block_func("%entry");
        if (ret_ptr->funcRetType->type == SysyFrontend::Type_Struct::Type::Type_Void)
        {
            koopaIR->AddReturnStatement(KoopaSymbol());
        }
        // 退出作用域
        PopNameMap();
        koopaIR->ReturnBackScope();
    }
    else
    {
        std::cerr << "FuncDef_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Type        ::= "void" | "int";
SysyFrontend::Type_Struct *SysyFrontend::Type_func()
{
    ENTRY_GRAMMER(SysyFrontend::Type_Struct);

    if (curToken.rule[0] == "\"int\"")
    {
        ret_ptr->type = SysyFrontend::Type_Struct::Type::Type_Int;
        ret_ptr->koopa_type = KoopaVarType::KOOPA_INT32;
        RESERVED_func(); // "int"
    }
    else if (curToken.rule[0] == "\"void\"")
    {
        ret_ptr->type = SysyFrontend::Type_Struct::Type::Type_Void;
        ret_ptr->koopa_type = KoopaVarType::KOOPA_undef;
        RESERVED_func(); // "void"
    }
    else
    {
        std::cerr << "FuncType_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// FuncFParams ::= FuncFParam {"," FuncFParam};
SysyFrontend::FuncFParams_Struct *SysyFrontend::FuncFParams_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncFParams_Struct);

    if (curToken.rule[0] == "FuncFParam")
    {
        FuncFParam_Struct *param = FuncFParam_func();
        ret_ptr->Params.push_back(param);
        std::string ir_name = koopaIR->GetUniqueName(param->IDENT->identifer);
        KoopaVar koopa_param = koopaIR->NewVar(param->koopa_type, ir_name);
        ret_ptr->koopa_params.push_back(koopa_param);
        if (param->koopa_type == KoopaVarType::KOOPA_INT32)
        {
            AddName(param->IDENT->identifer, ir_name, false, true);
        }
        else
        {
            AddName(param->IDENT->identifer, ir_name, false, true, true);
        }
        if (curToken.rule.size() > 1)
        {
            for (int i = 1; i < curToken.rule.size(); i += 2)
            {
                RESERVED_func(); // ","
                param = FuncFParam_func();
                ret_ptr->Params.push_back(param);
                ir_name = koopaIR->GetUniqueName(param->IDENT->identifer);
                koopa_param = koopaIR->NewVar(param->koopa_type, ir_name);
                ret_ptr->koopa_params.push_back(koopa_param);
                if (param->koopa_type == KoopaVarType::KOOPA_INT32)
                {
                    AddName(param->IDENT->identifer, ir_name, false, true);
                }
                else
                {
                    AddName(param->IDENT->identifer, ir_name, false, true, true);
                }
            }
        }
    }
    else
    {
        std::cerr << "FuncFParams_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// FuncFParam    ::= Type IDENT ["[" "]" {"[" ConstExp "]"}];
SysyFrontend::FuncFParam_Struct *SysyFrontend::FuncFParam_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncFParam_Struct);

    if (curToken.rule[0] == "Type")
    {
        ret_ptr->Type = Type_func();
        ret_ptr->IDENT = IDENT_func();
        if (curToken.rule.size() > 2 && curToken.rule[2] == "\"[\"")
        {
            RESERVED_func(); // "["
            RESERVED_func(); // "]"
            if (curToken.rule.size() > 4)
            {
                std::vector<int> size_list;
                for (int i = 4; i < curToken.rule.size(); i += 3)
                {
                    RESERVED_func(); // "["
                    ConstExp_Struct *exp = ConstExp_func();
                    size_list.push_back(exp->value.GetImm());
                    RESERVED_func(); // "]"
                }
                KoopaVarType type = KoopaVarType::KOOPA_INT32;
                for (int i = size_list.size() - 1; i >= 0; i--)
                {
                    type = KoopaVarType::ARRAY_Type(type, size_list[i]);
                }
                ret_ptr->koopa_type = KoopaVarType::PTR_Type(type);
            }
            else
            {
                ret_ptr->koopa_type = KoopaVarType::PTR_Type(KoopaVarType::KOOPA_INT32);
            }
        }
        else
        {
            ret_ptr->koopa_type = KoopaVarType::KOOPA_INT32;
        }
    }
    else
    {
        std::cerr << "FuncFParam_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// FuncRParams ::= Exp {"," Exp};
SysyFrontend::FuncRParams_Struct *SysyFrontend::FuncRParams_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncRParams_Struct);

    if (curToken.rule[0] == "Exp")
    {
        Exp_Struct *exp = Exp_func();
        ret_ptr->Params.push_back(exp);
        ret_ptr->koopa_params.push_back(exp->value);
        if (curToken.rule.size() > 1)
        {
            for (int i = 1; i < curToken.rule.size(); i += 2)
            {
                RESERVED_func(); // ","
                exp = Exp_func();
                ret_ptr->Params.push_back(exp);
                ret_ptr->koopa_params.push_back(exp->value);
            }
        }
    }
    else
    {
        std::cerr << "FuncRParams_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}