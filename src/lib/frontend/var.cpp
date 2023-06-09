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

// ConstDecl     ::= "const" Type ConstDef { ',' ConstDef } ';';
SysyFrontend::ConstDecl_Struct *SysyFrontend::ConstDecl_func()
{
    ENTRY_GRAMMER(SysyFrontend::ConstDecl_Struct);

    if (curToken.rule[0] == "\"const\"")
    {
        RESERVED_func(); // 'const'
        ret_ptr->Type = Type_func();
        ret_ptr->ConstDefs.push_back(ConstDef_func(ret_ptr->Type));
        if (curToken.rule[3] == "\",\"")
        {
            for (int i = 3; i < curToken.rule.size() - 1; i += 2)
            {
                RESERVED_func(); // ','
                ret_ptr->ConstDefs.push_back(ConstDef_func(ret_ptr->Type));
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

// ConstDef      ::= IDENT {"[" ConstExp "]"} "=" ConstInitVal;
SysyFrontend::ConstDef_Struct *SysyFrontend::ConstDef_func(SysyFrontend::Type_Struct *Type)
{
    ENTRY_GRAMMER(SysyFrontend::ConstDef_Struct);

    if (Type->type == SysyFrontend::Type_Struct::Type::Type_Void)
    {
        std::cerr << "Can't define void variables" << std::endl;
        exit(1);
    }

    if (curToken.rule[0] == "IDENT")
    {
        ret_ptr->ident = IDENT_func()->identifer;
        RESERVED_func(); // '='
        std::string ir_name = koopaIR->GetUniqueName(ret_ptr->ident);
        if (curToken.rule[1] == "\"[\"")
        {
            for (int i = 1; i < curToken.rule.size() - 2; i += 3)
            {
                RESERVED_func(); // '['
                ret_ptr->Lengths.push_back(ConstExp_func());
                RESERVED_func(); // ']'
            }
            AddName(ret_ptr->ident, ir_name, true, false, true);
            if (koopaIR->curScope == &koopaIR->global_scope)
            {
                ret_ptr->ConstInitVal = ConstInitVal_func();
                KoopaVarType type = KoopaVarType::KOOPA_INT32;
                for (int i = ret_ptr->Lengths.size() -1; i >= 0; i--)
                {
                    type = KoopaVarType::ARRAY_Type(type, ret_ptr->Lengths[i]->value.GetImm());
                }
                koopaIR->NewVarWithInit(type, ir_name, ret_ptr->ConstInitVal->initList);
            }
            else
            {
                KoopaVarType type = KoopaVarType::KOOPA_INT32;
                for (int i = ret_ptr->Lengths.size() -1; i >= 0; i--)
                {
                    type = KoopaVarType::ARRAY_Type(type, ret_ptr->Lengths[i]->value.GetImm());
                }
                KoopaVar var = koopaIR->NewVar(type, ir_name);
                koopaIR->AddAllocStatement(var);
                ret_ptr->ConstInitVal = ConstInitVal_func(&var);
            }
        }
        else
        {
            AddName(ret_ptr->ident, ir_name, true);
            if (koopaIR->curScope == &koopaIR->global_scope)
            {
                ret_ptr->ConstInitVal = ConstInitVal_func();
                koopaIR->NewVarWithInit(KoopaVarType::KOOPA_INT32, ir_name, ret_ptr->ConstInitVal->value.GetImm());
            }
            else
            {
                KoopaVar var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, ir_name);
                koopaIR->AddAllocStatement(var);
                ret_ptr->ConstInitVal = ConstInitVal_func(&var);
            }
        }
    }
    else
    {
        std::cerr << "ConstDef_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
SysyFrontend::ConstInitVal_Struct *SysyFrontend::ConstInitVal_func(KoopaVar *receiver)
{
    ENTRY_GRAMMER(SysyFrontend::ConstInitVal_Struct);

    if (curToken.rule[0] == "ConstExp")
    {
        ret_ptr->type = SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_Exp;
        ret_ptr->subStructPointer.Exp = ConstExp_func(receiver);
        ret_ptr->value = ret_ptr->subStructPointer.Exp->value;
    }
    else if (curToken.rule[0] == "\"{\"")
    {
        ret_ptr->type = SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_InitList;
        RESERVED_func(); // '{'
        ret_ptr->subStructPointer.InitList = new std::vector<ConstInitVal_Struct *>();
        if (curToken.rule[1] == "ConstInitVal")
        {
            ret_ptr->subStructPointer.InitList->push_back(ConstInitVal_func());
            if (curToken.rule[2] == "\",\"")
            {
                for (int i = 2; i < curToken.rule.size() - 1; i += 2)
                {
                    RESERVED_func(); // ','
                    ret_ptr->subStructPointer.InitList->push_back(ConstInitVal_func());
                }
            }
        }
        RESERVED_func(); // '}'
        KoopaInitList *initList = new KoopaInitList();
        for (int i = 0; i < ret_ptr->subStructPointer.InitList->size(); i++)
        {
            ConstInitVal_Struct *initVal = ret_ptr->subStructPointer.InitList->at(i);
            if (initVal->type == SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_Exp)
            {
                initList->initList.push_back(initVal->value.GetImm());
            }
            else if (initVal->type == SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_InitList)
            {
                initList->initList.push_back(initVal->initList);
            }
        }
        koopaIR->AddStoreStatement(*receiver, *initList);
    }
    else
    {
        std::cerr << "ConstInitVal_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// VarDecl       ::= Type VarDef {"," VarDef} ";";
SysyFrontend::VarDecl_Struct *SysyFrontend::VarDecl_func()
{
    ENTRY_GRAMMER(SysyFrontend::VarDecl_Struct);

    if (curToken.rule[0] == "Type")
    {
        ret_ptr->Type = Type_func();
        ret_ptr->VarDefs.push_back(VarDef_func(ret_ptr->Type));
        if (curToken.rule[2] == "\",\"")
        {
            for (int i = 2; i < curToken.rule.size() - 1; i += 2)
            {
                RESERVED_func(); // ','
                ret_ptr->VarDefs.push_back(VarDef_func(ret_ptr->Type));
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

// VarDef        ::= VarDefLeft | VarDefLeft "=" InitVal;
SysyFrontend::VarDef_Struct *SysyFrontend::VarDef_func(SysyFrontend::Type_Struct *Type)
{
    ENTRY_GRAMMER(SysyFrontend::VarDef_Struct);

    if (Type->type == SysyFrontend::Type_Struct::Type::Type_Void)
    {
        std::cerr << "Can't define void variables." << std::endl;
        exit(1);
    }

    if (curToken.rule[0] == "VarDefLeft")
    {
        ret_ptr->VarDefLeft = VarDefLeft_func(Type);
        std::string ir_name = koopaIR->GetUniqueName(ret_ptr->VarDefLeft->ident);
        if (koopaIR->curScope == &koopaIR->global_scope)
        {
            if (curToken.rule.size() > 1 && curToken.rule[1] == "\"=\"")
            {
                RESERVED_func(); // '='
                ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Initialized;
                ret_ptr->InitVal = InitVal_func();
                if (ret_ptr->VarDefLeft->Lengths.size() > 0)
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name, false, false, true);
                    KoopaVarType type = KoopaVarType::KOOPA_INT32;
                    for (int i = ret_ptr->VarDefLeft->Lengths.size() - 1; i >= 0; i--)
                    {
                        type = KoopaVarType::ARRAY_Type(type, ret_ptr->VarDefLeft->Lengths[i]->value.GetImm());
                    }
                    koopaIR->NewVarWithInit(type, ir_name, ret_ptr->InitVal->initList);
                }
                else
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name);
                    koopaIR->NewVarWithInit(KoopaVarType::KOOPA_INT32, ir_name, ret_ptr->InitVal->value.GetImm());
                }
            }
            else
            {
                ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Uninitialized;
                if (ret_ptr->VarDefLeft->Lengths.size() > 0)
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name, false, false, true);
                    KoopaVarType type = KoopaVarType::KOOPA_INT32;
                    for (int i = ret_ptr->VarDefLeft->Lengths.size() - 1; i >= 0; i--)
                    {
                        type = KoopaVarType::ARRAY_Type(type, ret_ptr->VarDefLeft->Lengths[i]->value.GetImm());
                    }
                    koopaIR->NewVarWithInit(type, ir_name, std::string("zeroinit"));
                }
                else
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name);
                    koopaIR->NewVarWithInit(KoopaVarType::KOOPA_INT32, ir_name, std::string("zeroinit"));
                }
            }
        }
        else
        {
            if (curToken.rule.size() > 1 && curToken.rule[1] == "\"=\"")
            {
                RESERVED_func(); // "="
                ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Initialized;
                if (ret_ptr->VarDefLeft->Lengths.size() > 0)
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name, false, false, true);
                    KoopaVarType type = KoopaVarType::KOOPA_INT32;
                    for (int i = ret_ptr->VarDefLeft->Lengths.size() - 1; i >= 0; i--)
                    {
                        type = KoopaVarType::ARRAY_Type(type, ret_ptr->VarDefLeft->Lengths[i]->value.GetImm());
                    }
                    KoopaVar var = koopaIR->NewVar(type, ir_name);
                    koopaIR->AddAllocStatement(var);
                    ret_ptr->InitVal = InitVal_func(&var);
                }
                else
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name);
                    KoopaVar var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, ir_name);
                    koopaIR->AddAllocStatement(var);
                    ret_ptr->InitVal = InitVal_func(&var);
                }
            }
            else
            {
                ret_ptr->type = SysyFrontend::VarDef_Struct::VarDefType::VarDefType_Uninitialized;
                if (ret_ptr->VarDefLeft->Lengths.size() > 0)
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name, false, false, true);
                    KoopaVarType type = KoopaVarType::KOOPA_INT32;
                    for (int i = ret_ptr->VarDefLeft->Lengths.size() - 1; i >= 0; i--)
                    {
                        type = KoopaVarType::ARRAY_Type(type, ret_ptr->VarDefLeft->Lengths[i]->value.GetImm());
                    }
                    KoopaVar var = koopaIR->NewVar(type, ir_name);
                    koopaIR->AddAllocStatement(var);
                }
                else
                {
                    AddName(ret_ptr->VarDefLeft->ident, ir_name);
                    KoopaVar var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, ir_name);
                    koopaIR->AddAllocStatement(var);
                }
            }
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

// VarDefLeft    ::= IDENT {"[" ConstExp "]"};
SysyFrontend::VarDefLeft_Struct *SysyFrontend::VarDefLeft_func(SysyFrontend::Type_Struct *Type)
{
    ENTRY_GRAMMER(SysyFrontend::VarDefLeft_Struct);
    if (curToken.rule[0] == "IDENT")
    {
        ret_ptr->ident = IDENT_func()->identifer;
        if (curToken.rule.size() > 1 && curToken.rule[1] == "\"[\"")
        {
            for (int i = 1; i < curToken.rule.size(); i += 3)
            {
                RESERVED_func(); // "["
                ret_ptr->Lengths.push_back(ConstExp_func());
                RESERVED_func(); // "]"
            }
        }
    }
    else
    {
        std::cerr << "VarDefLeft_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";
SysyFrontend::InitVal_Struct *SysyFrontend::InitVal_func(KoopaVar *receiver)
{
    ENTRY_GRAMMER(SysyFrontend::InitVal_Struct);
    if (curToken.rule[0] == "Exp")
    {
        ret_ptr->type = SysyFrontend::InitVal_Struct::InitValType::InitValType_Exp;
        ret_ptr->subStructPointer.Exp = Exp_func(receiver);
        ret_ptr->value = ret_ptr->subStructPointer.Exp->value;
    }
    else if (curToken.rule[0] == "\"{\"")
    {
        ret_ptr->type = SysyFrontend::InitVal_Struct::InitValType::InitValType_InitList;
        RESERVED_func(); // "{"
        ret_ptr->subStructPointer.InitList = new std::vector<SysyFrontend::InitVal_Struct *>();
        if (curToken.rule.size() > 1 && curToken.rule[1] == "InitVal")
        {
            ret_ptr->subStructPointer.InitList->push_back(InitVal_func());
            if (curToken.rule[2] == "\",\"")
            {
                for (int i = 2; i < curToken.rule.size() - 1; i += 2)
                {
                    RESERVED_func(); // ","
                    ret_ptr->subStructPointer.InitList->push_back(InitVal_func());
                }
            }
        }
        RESERVED_func(); // "}"
        KoopaInitList *initList = new KoopaInitList();
        initList->initListType = KoopaInitList::KOOPA_INIT_AGGREGATE;
        for (int i = 0; i < ret_ptr->subStructPointer.InitList->size(); i++)
        {
            InitVal_Struct *initVal = ret_ptr->subStructPointer.InitList->at(i);
            if (initVal->type == SysyFrontend::InitVal_Struct::InitValType::InitValType_Exp)
            {
                initList->initList.push_back(initVal->value.GetImm());
            }
            else if (initVal->type == SysyFrontend::InitVal_Struct::InitValType::InitValType_InitList)
            {
                initList->initList.push_back(initVal->initList);
            }
        }
        koopaIR->AddStoreStatement(*receiver, *initList);
    }
    else
    {
        std::cerr << "InitVal_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}