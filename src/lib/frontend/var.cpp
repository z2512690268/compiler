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

// array-init helpers
std::vector<KoopaInitList> ZeroInit_List(std::vector<SysyFrontend::ConstExp_Struct *> &lengths, int index)
{
    std::vector<KoopaInitList> list;
    if (index == lengths.size() - 1)
    {
        for (int i = 0; i < lengths.at(index)->value.GetImm(); i++)
        {
            list.push_back(KoopaInitList(0));
        }
    }
    else
    {
        for (int i = 0; i < lengths.at(index)->value.GetImm(); i++)
        {
            list.push_back(KoopaInitList(ZeroInit_List(lengths, index + 1)));
        }
    }
    return list;
}

bool allZero(int *arr, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        if (arr[i] != 0)
            return false;
    }
    return true;
}

KoopaInitList CreateInitList(std::vector<SysyFrontend::ConstExp_Struct *> &lengths, KoopaInitList raw_list)
{
    // preparing
    int val_ptrs[lengths.size()];
    int init_ptrs[lengths.size()];
    for (int i = 0; i < lengths.size(); i++)
    {
        val_ptrs[i] = 0;
        init_ptrs[i] = 0;
    }
    int level = 0;
    KoopaInitList new_list = KoopaInitList(ZeroInit_List(lengths, 0));

    std::string length = "";
    for (int i = 0; i < lengths.size(); i++)
    {
        length += std::to_string(lengths.at(i)->value.GetImm()) + " ";
    }
    std::cout << "length: " << length << std::endl;

    // initializing
    while (init_ptrs[0] < lengths.at(0)->value.GetImm())
    {
        int val = 0;
        // find val & place
        std::vector<KoopaInitList> cur_scope = raw_list.initList;
        for (int i = 0; i < level; i++)
        {
            cur_scope = cur_scope.at(val_ptrs[i]).initList;
        }
        while (level >= 0 && val_ptrs[level] >= cur_scope.size())
        {
            val_ptrs[level] = 0;
            if (!allZero(init_ptrs, level, lengths.size()))
            {
                for (int i = level; i < lengths.size(); i++)
                {
                    init_ptrs[i] = 0;
                }
                init_ptrs[level - 1]++;
            }
            level--;
            val_ptrs[level]++;
            cur_scope = raw_list.initList;
            for (int i = 0; i < level; i++)
            {
                cur_scope = cur_scope.at(val_ptrs[i]).initList;
            }
        }
        if (level < 0)
            break;

        std::cout << std::endl;

        KoopaInitList cur_val = cur_scope.at(val_ptrs[level]);
        while (cur_val.GetInitListType() != "int")
        {
            level++;
            cur_val = cur_val.initList.at(0);
            if (!allZero(init_ptrs, level, lengths.size()))
            {
                for (int i = level; i < lengths.size(); i++)
                {
                    init_ptrs[i] = 0;
                }
                init_ptrs[level - 1]++;
            }
        }
        val = cur_val.initInt;

        std::cout << "val_ptr: ";
        for (int i = 0; i < lengths.size(); i++)
        {
            std::cout << val_ptrs[i] << " ";
        }
        val_ptrs[level]++;

        std::cout << std::endl;
        std::cout << "index: ";
        for (int i = 0; i < lengths.size(); i++)
        {
            std::cout << init_ptrs[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "val: " << val << std::endl << std::endl;
        // fill val
        KoopaInitList* holder = &new_list;
        for (int i = 0; i < lengths.size(); i++)
        {
            holder = &holder->initList.at(init_ptrs[i]);
        }
        holder->initInt = val;
        // go next
        int focus = lengths.size() - 1;
        init_ptrs[focus]++;
        while (focus >= 0 && init_ptrs[focus] >= lengths.at(focus)->value.GetImm())
        {
            init_ptrs[focus] = 0;
            focus--;
            init_ptrs[focus]++;
        }
        if (focus < 0)
            break;
    }

    std::cout << new_list.GetInitString() << std::endl;
    return new_list;
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
                for (int i = ret_ptr->Lengths.size() - 1; i >= 0; i--)
                {
                    type = KoopaVarType::ARRAY_Type(type, ret_ptr->Lengths[i]->value.GetImm());
                }
                koopaIR->NewVarWithInit(type, ir_name, CreateInitList(ret_ptr->Lengths, ret_ptr->ConstInitVal->initList));
            }
            else
            {
                KoopaVarType type = KoopaVarType::KOOPA_INT32;
                for (int i = ret_ptr->Lengths.size() - 1; i >= 0; i--)
                {
                    type = KoopaVarType::ARRAY_Type(type, ret_ptr->Lengths[i]->value.GetImm());
                }
                KoopaVar var = koopaIR->NewVar(type, ir_name);
                koopaIR->AddAllocStatement(var);
                ret_ptr->ConstInitVal = ConstInitVal_func();
                koopaIR->AddStoreStatement(var, CreateInitList(ret_ptr->Lengths, ret_ptr->ConstInitVal->initList));
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
        ret_ptr->initList.initListType = KoopaInitList::KOOPA_INIT_AGGREGATE;
        for (int i = 0; i < ret_ptr->subStructPointer.InitList->size(); i++)
        {
            ConstInitVal_Struct *initVal = ret_ptr->subStructPointer.InitList->at(i);
            if (initVal->type == SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_Exp)
            {
                if (!initVal->value.IsImm())
                {
                    std::cerr << "Unsupported: Initializing array with variables" << std::endl;
                    exit(1);
                }
                ret_ptr->initList.initList.push_back(initVal->value.GetImm());
            }
            else if (initVal->type == SysyFrontend::ConstInitVal_Struct::ConstInitValType::ConstInitValType_InitList)
            {
                ret_ptr->initList.initList.push_back(initVal->initList.initList);
            }
        }
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
                    koopaIR->NewVarWithInit(type, ir_name, CreateInitList(ret_ptr->VarDefLeft->Lengths, ret_ptr->InitVal->initList));
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
                    ret_ptr->InitVal = InitVal_func();
                    koopaIR->AddStoreStatement(var, CreateInitList(ret_ptr->VarDefLeft->Lengths, ret_ptr->InitVal->initList));
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
        ret_ptr->initList.initListType = KoopaInitList::KOOPA_INIT_AGGREGATE;
        for (int i = 0; i < ret_ptr->subStructPointer.InitList->size(); i++)
        {
            InitVal_Struct *initVal = ret_ptr->subStructPointer.InitList->at(i);
            if (initVal->type == SysyFrontend::InitVal_Struct::InitValType::InitValType_Exp)
            {
                if (!initVal->value.IsImm())
                {
                    std::cerr << "Unsupported: Initializing array with variables" << std::endl;
                    exit(1);
                }
                ret_ptr->initList.initList.push_back(initVal->value.GetImm());
            }
            else if (initVal->type == SysyFrontend::InitVal_Struct::InitValType::InitValType_InitList)
            {
                ret_ptr->initList.initList.push_back(initVal->initList.initList);
            }
        }
    }
    else
    {
        std::cerr << "InitVal_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}