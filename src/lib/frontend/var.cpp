#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

// ConstExp      ::= Exp;
SysyFrontend::ConstExp_Struct *SysyFrontend::ConstExp_func(KoopaVar *receiver)
{
    ENTRY_GRAMMER(SysyFrontend::ConstExp_Struct);

    if (curToken.rule[0] == "Exp")
    {
        ret_ptr->Exp = Exp_func(receiver);
        ret_ptr->value = ret_ptr->Exp->value;
    }
    else
    {
        std::cerr << "ConstExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Decl          ::= ConstDecl | VarDecl;
SysyFrontend::Decl_Struct *SysyFrontend::Decl_func()
{
    ENTRY_GRAMMER(SysyFrontend::Decl_Struct);

    if (curToken.rule[0] == "ConstDecl")
    {
        ret_ptr->type = SysyFrontend::Decl_Struct::DeclType::DeclType_ConstDecl;
        ret_ptr->subStructPointer.ConstDecl = ConstDecl_func();
    }
    else if (curToken.rule[0] == "VarDecl")
    {
        ret_ptr->type = SysyFrontend::Decl_Struct::DeclType::DeclType_VarDecl;
        ret_ptr->subStructPointer.VarDecl = VarDecl_func();
    }
    else
    {
        std::cerr << "Decl_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// ConstDecl     ::= "const" BType ConstDef { ',' ConstDef } ';';
SysyFrontend::ConstDecl_Struct *SysyFrontend::ConstDecl_func()
{
    ENTRY_GRAMMER(SysyFrontend::ConstDecl_Struct);

    if (curToken.rule[0] == "\"const\"")
    {
        RESERVED_func(); // 'const'
        ret_ptr->BType = BType_func();
        ret_ptr->ConstDefs.push_back(ConstDef_func(ret_ptr->BType));
        if (curToken.rule[3] == ",")
        {
            for (int i = 3; i < curToken.rule.size() - 1; i += 2)
            {
                RESERVED_func(); // ','
                ret_ptr->ConstDefs.push_back(ConstDef_func(ret_ptr->BType));
            }
        }
        RESERVED_func(); // ';'
    }
    else
    {
        std::cerr << "ConstDecl_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// BType         ::= "int";
SysyFrontend::BType_Struct *SysyFrontend::BType_func()
{
    ENTRY_GRAMMER(SysyFrontend::BType_Struct);

    if (curToken.rule[0] == "\"int\"")
    {
        RESERVED_func(); // "int"
        ret_ptr->type = SysyFrontend::BType_Struct::BTypeType::BTypeType_INT;
    }
    else
    {
        std::cerr << "BType_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// ConstDef      ::= IDENT "=" ConstInitVal;
SysyFrontend::ConstDef_Struct *SysyFrontend::ConstDef_func(SysyFrontend::BType_Struct *BType)
{
    ENTRY_GRAMMER(SysyFrontend::ConstDef_Struct);

    if (curToken.rule[0] == "IDENT")
    {
        ret_ptr->ident = IDENT_func()->identifer;
        RESERVED_func(); // '='
        std::string ir_name = koopaIR->GetUniqueName(ret_ptr->ident);
        KoopaVar var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, ir_name);
        AddName(ret_ptr->ident, ir_name, true);
        ret_ptr->ConstInitVal = ConstInitVal_func(&var);
    }
    else
    {
        std::cerr << "ConstDef_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// ConstInitVal  ::= ConstExp;
SysyFrontend::ConstInitVal_Struct *SysyFrontend::ConstInitVal_func(KoopaVar *receiver)
{
    ENTRY_GRAMMER(SysyFrontend::ConstInitVal_Struct);

    if (curToken.rule[0] == "ConstExp")
    {
        ret_ptr->ConstExp = ConstExp_func(receiver);
    }
    else
    {
        std::cerr << "ConstInitVal_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// VarDecl       ::= BType VarDef { ',' VarDef } ';';
SysyFrontend::VarDecl_Struct *SysyFrontend::VarDecl_func()
{
    ENTRY_GRAMMER(SysyFrontend::VarDecl_Struct);

    if (curToken.rule[0] == "BType")
    {
        ret_ptr->BType = BType_func();
        ret_ptr->VarDefs.push_back(VarDef_func(ret_ptr->BType));
        if (curToken.rule[2] == ",")
        {
            for (int i = 3; i < curToken.rule.size() - 1; i += 2)
            {
                RESERVED_func(); // ','
                ret_ptr->VarDefs.push_back(VarDef_func(ret_ptr->BType));
            }
        }
        RESERVED_func(); // ';'
    }
    else
    {
        std::cerr << "VarDecl_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// VarDef        ::= IDENT | IDENT "=" InitVal;
SysyFrontend::VarDef_Struct *SysyFrontend::VarDef_func(SysyFrontend::BType_Struct *BType)
{
    ENTRY_GRAMMER(SysyFrontend::VarDef_Struct);

    if (curToken.rule[0] == "IDENT")
    {
        ret_ptr->ident = IDENT_func()->identifer;
        std::string ir_name = koopaIR->GetUniqueName(ret_ptr->ident);
        KoopaVar var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, ir_name);
        AddName(ret_ptr->ident, ir_name);
        if (curToken.rule.size() > 1 && curToken.rule[1] == "\"=\"")
        {
            RESERVED_func(); // "="
            ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Initialized;
            ret_ptr->InitVal = InitVal_func(&var);
        }
        else
        {
            ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Uninitialized;
        }
    }
    else
    {
        std::cerr << "VarDef_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// InitVal       ::= Exp;
SysyFrontend::InitVal_Struct *SysyFrontend::InitVal_func(KoopaVar *receiver)
{
    ENTRY_GRAMMER(SysyFrontend::InitVal_Struct);
    if (curToken.rule[0] == "Exp")
    {
        ret_ptr->Exp = Exp_func(receiver);
    }
    else
    {
        std::cerr << "InitVal_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}