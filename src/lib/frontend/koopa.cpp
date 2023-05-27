#include "frontend.h"
#include "koopa.h"
#include "transfer.h"
//******************************************************************************

inline std::string KoopaVarTypeToString(KoopaVarType type)
{
    std::cout << "KoopaVarTypeToString" << std::endl;
    std::cout << "#####################" << type.topType << std::endl;
    std::string ret;
    switch (type.topType)
    {
    case KoopaVarType::KOOPA_INT32:
        std::cout << "KOOPA_INT32" << std::endl;
        return "i32";
    case KoopaVarType::KOOPA_ARRAY:
        return "[" + KoopaVarTypeToString(*type.arrayType.type) + ", " + std::to_string(type.arrayType.size) + "]";
    case KoopaVarType::KOOPA_PTR:
        return "*" + KoopaVarTypeToString(*type.ptrType.type);
    case KoopaVarType::KOOPA_func:
        ret = "(";
        for (int i = 0; i < type.funcType.paramsType.size(); i++)
        {
            ret += KoopaVarTypeToString(*type.funcType.paramsType[i]);
            if (i != type.funcType.paramsType.size() - 1)
            {
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
// CompUnits ::= CompUnit {CompUnit};
KoopaFrontend::CompUnits_Struct *KoopaFrontend::CompUnits_func()
{
    ENTRY_GRAMMER(KoopaFrontend::CompUnits_Struct);
    CompUnits_Struct *compUnits_ptr = new CompUnits_Struct();
    int count = 0;
    std::cout << "............................CompUnit" << curToken.rule.size() << std::endl;
    for (auto rule : curToken.rule)
    {
        if (rule == "CompUnit")
        {
            count++;
            std::cout << "............................CompUnitttt" << std::endl;
            CompUnit_Struct *compUnit_ptr = CompUnit_func();
            // if (count == 2)
            //     std::cout << "............................CompUnit" << compUnit_ptr->FunDef->SYMBOL->SYMBOL << std::endl;
            // if (count == 1)
            //     std::cout << "............................CompUnit" << compUnit_ptr->GlobalSymbolDef->SYMBOL->SYMBOL << std::endl;
            compUnits_ptr->CompUnit.push_back(compUnit_ptr);
            std::cout << "............................CompUnitttt" << std::endl;
        }
    }

    return compUnits_ptr;
}

// CompUnit  ::= GlobalSymbolDef | FunDecl |FunDef;
KoopaFrontend::CompUnit_Struct *KoopaFrontend::CompUnit_func()
{
    ENTRY_GRAMMER(KoopaFrontend::CompUnit_Struct);
    CompUnit_Struct *compUnit_ptr = new CompUnit_Struct();
    std::cout << "............................CompUnit" << std::endl;
    if (curToken.rule[0] == "GlobalSymbolDef")
    {
        compUnit_ptr->GlobalSymbolDef = GlobalSymbolDef_func();
        return compUnit_ptr;
    }
    if (curToken.rule[0] == "FunDecl")
    {
        compUnit_ptr->FunDecl = FunDecl_func();
        std::cout << "............................CompUnit::fundecl end" << std::endl;

        return compUnit_ptr;
    }
    if (curToken.rule[0] == "FunDef")
    {
        std::cout << "............................CompUnit::fundef" << std::endl;
        compUnit_ptr->FunDef = FunDef_func();
        return compUnit_ptr;
    }

    return compUnit_ptr;
}

// FunDef ::= "fun" SYMBOL "(" [FunParams] ")" [":" Type] "{" {Block} "}";
KoopaFrontend::FunDef_Struct *KoopaFrontend::FunDef_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunDef_Struct);
    FunDef_Struct *funDef_ptr = new FunDef_Struct();
    // 创建作用域
    Scope *func_scope = koopaIR->AddNewScope();
    int i = 0;
    RESERVED_func(); // fun
    funDef_ptr->SYMBOL = SYMBOL_func();
    RESERVED_func(); // (
    if (curToken.rule[3] == "FunParams")
    {
        funDef_ptr->FunParams = FunParams_func();
        std::vector<KoopaVar> *params = new std::vector<KoopaVar>();
        std::vector<SYMBOL_Struct *>::iterator its = funDef_ptr->FunParams->SYMBOL.begin();
        std::vector<Type_Struct *>::iterator itt = funDef_ptr->FunParams->Type.begin();
        while (its != funDef_ptr->FunParams->SYMBOL.end())
        {
            KoopaVar *new_kv = new KoopaVar();
            new_kv->varName = (*its)->SYMBOL;
            new_kv->type = (*itt)->Type;
            params->push_back(*new_kv);
            its++;
            itt++;
        }
        func_scope->func_param = *params;
        i++;
    }
    RESERVED_func(); // )
    i += 4;
    if (curToken.rule[i] == "\":\"")
    {
        RESERVED_func(); // :
        funDef_ptr->Type = Type_func();
        func_scope->func_ret_type = funDef_ptr->Type->Type;
        i += 2;
    }
    RESERVED_func(); // {
    i++;
    while (curToken.rule[i] == "Block")
    {
        funDef_ptr->Block.push_back(Block_func());
        i++;
    }

    RESERVED_func(); // }
    std::cout << i << std::endl;
    // 退出作用域
    func_scope->func_name = funDef_ptr->SYMBOL->SYMBOL;

    for (auto &block : funDef_ptr->Block)
    {
        func_scope->basicBlocks.push_back(block->block);
    }
    std::cout << "SSSSSScopde" << (koopaIR->curScope == &koopaIR->global_scope) << std::endl;
    koopaIR->ReturnBackScope();
    std::cout << "SSSSSScopde" << (koopaIR->curScope == &koopaIR->global_scope) << std::endl;
    std::cout << "funcdef end" << std::endl;
    return funDef_ptr;
}
// Type ::= "i32" | ArrayType | PointerType | FunType;
KoopaFrontend::Type_Struct *KoopaFrontend::Type_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Type_Struct);
    Type_Struct *type_ptr = new Type_Struct();
    if (curToken.rule[0] == "\"i32\"")
    {
        RESERVED_func(); // i32
        std::cout << "--------------------------i32Type:" << std::endl;
        type_ptr->Type = KoopaVarType::KOOPA_INT32;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << type_ptr->Type.topType << std::endl;
        // type_ptr->Type = *(new KoopaVarType(KoopaVarType::TopType::KOOPA_INT32));
        std::cout << KoopaVarTypeToString(type_ptr->Type) << std::endl;

        return type_ptr;
    }
    if (curToken.rule[0] == "ArrayType")
    {
        type_ptr->ArrayType = ArrayType_func();
        std::cout << "--------------------------ArrayType:" << std::endl;
        std::cout << KoopaVarTypeToString(type_ptr->ArrayType->Type->Type) << std::endl;
        type_ptr->Type = type_ptr->Type.ARRAY_Type(type_ptr->ArrayType->Type->Type, type_ptr->ArrayType->INT->INT);
        return type_ptr;
    }
    if (curToken.rule[0] == "PointerType")
    {
        type_ptr->PointerType = PointerType_func();
        std::cout << "--------------------------PointerType:" << std::endl;
        std::cout << KoopaVarTypeToString(type_ptr->PointerType->Type->Type) << std::endl;
        type_ptr->Type = type_ptr->Type.PTR_Type(type_ptr->PointerType->Type->Type);
        return type_ptr;
    }
    if (curToken.rule[0] == "FunType")
    {
        type_ptr->FunType = FunType_func();
        int size = type_ptr->FunType->Type.size();
        std::vector<KoopaVarType> *params = new std::vector<KoopaVarType>();
        for (int i = 0; i < size - 1; i++)
        {
            params->push_back(type_ptr->FunType->Type[i]->Type);
        }
        std::cout << "-----------------------------FunType:dsadasd" << std::endl;
        std::cout << type_ptr->FunType->Type[size - 1]->Type.topType << std::endl;
        std::cout << KoopaVarTypeToString(type_ptr->FunType->Type[size - 1]->Type) << std::endl;
        type_ptr->Type = type_ptr->Type.FUNC_Type(type_ptr->FunType->Type[size - 1]->Type, *params);
        std::cout << KoopaVarTypeToString(type_ptr->Type) << std::endl;
        return type_ptr;
    }

    return type_ptr;
}

// ArrayType ::= "[" Type "," INT "]";
KoopaFrontend::ArrayType_Struct *KoopaFrontend::ArrayType_func()
{
    ENTRY_GRAMMER(KoopaFrontend::ArrayType_Struct);
    ArrayType_Struct *arrayType_ptr = new ArrayType_Struct();
    if (curToken.rule[0] == "\"[\"")
    {
        RESERVED_func(); // [
        arrayType_ptr->Type = Type_func();
        RESERVED_func(); // ,
        arrayType_ptr->INT = INT_func();
        RESERVED_func(); // ]
        return arrayType_ptr;
    }

    return arrayType_ptr;
}

// PointerType ::= "*" Type;
KoopaFrontend::PointerType_Struct *KoopaFrontend::PointerType_func()
{
    ENTRY_GRAMMER(KoopaFrontend::PointerType_Struct);
    PointerType_Struct *pointerType_ptr = new PointerType_Struct();
    if (curToken.rule[0] == "\"*\"")
    {
        RESERVED_func(); // *

        pointerType_ptr->Type = Type_func();
        return pointerType_ptr;
    }

    return pointerType_ptr;
}

// FunType ::= "(" [Type {"," Type}] ")" [":" Type];
KoopaFrontend::FunType_Struct *KoopaFrontend::FunType_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunType_Struct);
    FunType_Struct *funType_ptr = new FunType_Struct();
    if (curToken.rule[0] == "\"(\"")
    {
        RESERVED_func(); // (
        int i = 1;
        if (curToken.rule[i] == "Type")
        {
            i++;
            funType_ptr->Type.push_back(Type_func());
            while (curToken.rule[i] == "\",\"")
            {
                RESERVED_func(); // ,
                funType_ptr->Type.push_back(Type_func());
                i += 2;
            }
        }
        RESERVED_func(); // )
        i++;
        if (curToken.rule[i] == "\":\"")
        {
            RESERVED_func(); // :
            funType_ptr->Type.push_back(Type_func());
        }
        std::cout << "))))))))))))))))))" << funType_ptr->Type.size() << std::endl;
        return funType_ptr;
    }

    return funType_ptr;
}

// Block ::= SYMBOL [BlockParamList] ":" {Statement} EndStatement;
// Block ::= SYMBOL ":" {Statement ";"} EndStatement ";";
KoopaFrontend::Block_Struct *KoopaFrontend::Block_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Block_Struct);
    Block_Struct *block_ptr = new Block_Struct();
    int i = 0;
    std::cout << "sda" << i << curToken.rule[0] << std::endl;
    if (curToken.rule[i] == "SYMBOL")
    {
        block_ptr->SYMBOL = SYMBOL_func();
        // 创建基本块
        std::string block_name = koopaIR->GetUniqueName(block_ptr->SYMBOL->SYMBOL);
        BasicBlock *block = koopaIR->NewBasicBlockAndSetCur(block_name);
        block_ptr->block = block;
        // block->parent = curBlock;
        // curBlock = block;
        // 基本块命名
        // block->label = block_ptr->SYMBOL->SYMBOL;
        // SymbolItem *block_item = new SymbolItem();
        // block_item->InitLabelSymbol(block);
        // curScope->AddSymbol(block->label, block_item);

        if (curToken.rule[i + 1] == "BlockParamList")
        {
            block_ptr->BlockParamList = BlockParamList_func();
            i++;
        }
        RESERVED_func(); // :
        i += 2;
        while (curToken.rule[i] == "Statement")
        {
            block_ptr->Statement.push_back(Statement_func());
            i += 1;
        }
        block_ptr->EndStatement = EndStatement_func();
        std::cout << "block end" << std::endl;
        block_ptr->block = block;
        return block_ptr;
    }

    return block_ptr;
}

// Statement ::= SymbolDef | Store | FunCall
KoopaFrontend::Statement_Struct *KoopaFrontend::Statement_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Statement_Struct);
    Statement_Struct *statement_ptr = new Statement_Struct();
    if (curToken.rule[0] == "SymbolDef")
    {
        std::cout << "statement:SymbolDef" << std::endl;
        statement_ptr->SymbolDef = SymbolDef_func();
        return statement_ptr;
    }
    else if (curToken.rule[0] == "Store")
    {
        statement_ptr->Store = Store_func();
        return statement_ptr;
    }
    else if (curToken.rule[0] == "FunCall")
    {
        std::cout << "statement:FunCall" << std::endl;
        statement_ptr->FunCall = FunCall_func();
        KoopaVar new_ptr = koopaIR->NewVar(KoopaVarType::KOOPA_undef, koopaIR->GetUniqueName(""));
        std::vector<KoopaSymbol> *params = new std::vector<KoopaSymbol>();
        for (auto v : statement_ptr->FunCall->Value)
        {
            KoopaSymbol k;
            if (v->type == Value_Struct::INT_TYPE)
                k = *(new KoopaSymbol(v->INT->INT));
            else if (v->type == Value_Struct::SYMBOL_TYPE)
                k = v->SYMBOL->SYMBOL;
            params->push_back(k);
        }
        std::cout << ".................statement:FunCallend000" << std::endl;
        koopaIR->AddCallStatement(statement_ptr->FunCall->SYMBOL->SYMBOL, *params, new_ptr);
        std::cout << ".................statement:FunCallend" << std::endl;
        return statement_ptr;
    }

    return statement_ptr;
}
// SymbolDef ::= SYMBOL "=" (MemoryDeclaration | Load | GetPointer | GetElementPointer | BinaryExpr | FunCall);
KoopaFrontend::SymbolDef_Struct *KoopaFrontend::SymbolDef_func()
{
    ENTRY_GRAMMER(KoopaFrontend::SymbolDef_Struct);
    SymbolDef_Struct *symbolDef_ptr = new SymbolDef_Struct();
    std::cout << "SymbolDef " << std::endl;

    if (curToken.rule[0] == "SYMBOL")
    {
        symbolDef_ptr->SYMBOL = SYMBOL_func();
        symbolDef_ptr->SymbolName = symbolDef_ptr->SYMBOL->SYMBOL;
        RESERVED_func(); // =
        if (curToken.rule[2] == "MemoryDeclaration")
        {
            symbolDef_ptr->MemoryDeclaration = MemoryDeclaration_func();
            KoopaVar new_ptr = koopaIR->NewVar(symbolDef_ptr->MemoryDeclaration->Type->Type, symbolDef_ptr->SymbolName);
            koopaIR->AddAllocStatement(new_ptr);
        }
        if (curToken.rule[2] == "Load")
        {
            symbolDef_ptr->Load = Load_func();
            KoopaVar new_ptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->SymbolName);
            koopaIR->AddLoadStatement(symbolDef_ptr->Load->SYMBOL->SYMBOL, new_ptr);
        }
        if (curToken.rule[2] == "GetPointer")
        {
            symbolDef_ptr->GetPointer = GetPointer_func();
            std::cout << "statement:FunCall" << std::endl;
            KoopaVar varptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->GetPointer->SYMBOL->SYMBOL);
            KoopaVar new_ptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->SymbolName);
            std::cout << "aaaa" << std::endl;
            koopaIR->AddGetptrStatement(varptr, symbolDef_ptr->GetPointer->Value->INT->INT, new_ptr);
        }
        if (curToken.rule[2] == "GetElementPointer")
        {
            symbolDef_ptr->GetElementPointer = GetElementPointer_func();
            KoopaVar arrptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->GetElementPointer->SYMBOL->SYMBOL);
            KoopaVar new_ptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->SymbolName);
            koopaIR->AddGetelementptrStatement(arrptr, symbolDef_ptr->GetElementPointer->Value->INT->INT, new_ptr);
        }
        if (curToken.rule[2] == "FunCall")
        {
            symbolDef_ptr->FunCall = FunCall_func();
            std::cout << "FunCall aaaaa" << std::endl;
            KoopaVar new_ptr = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->SymbolName);
            std::cout << "FunCall bbbbb" << std::endl;
            std::vector<KoopaSymbol> *params = new std::vector<KoopaSymbol>();
            std::cout << "FunCall hhhh" << std::endl;
            for (auto v : symbolDef_ptr->FunCall->Value)
            {
                std::cout << "FunCall oooo" << v->INT->INT << std::endl;
                KoopaSymbol k;
                if (v->type == Value_Struct::SYMBOL_TYPE)
                    k = v->SYMBOL->SYMBOL;
                else if (v->type == Value_Struct::INT_TYPE)
                    k = std::to_string(v->INT->INT);
                params->push_back(k);
                std::cout << "FunCall oooo" << std::endl;
            }
            std::cout << "FunCall gggg" << std::endl;
            if (symbolDef_ptr->FunCall->Value.size() == 0)
            {
                std::cout << "FunCall dddd" << std::endl;
                KoopaVar np = koopaIR->NewVar(KoopaVarType::KOOPA_undef, symbolDef_ptr->SymbolName);
                koopaIR->AddCallStatement(symbolDef_ptr->FunCall->SYMBOL->SYMBOL, *params, np);
            }
            else
            {
                std::cout << "FunCall eeee" << std::endl;
                koopaIR->AddCallStatement(symbolDef_ptr->FunCall->SYMBOL->SYMBOL, *params, new_ptr);
            }

            std::cout << "FunCall cccc" << std::endl;
        }
        if (curToken.rule[2] == "BinaryExpr")
        {
            symbolDef_ptr->BinaryExpr = BinaryExpr_func();
            std::string op = symbolDef_ptr->BinaryExpr->BINARY_OP->BINARY_OP;
            std::string value1;
            std::string value2;
            KoopaVar type = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, symbolDef_ptr->SymbolName);

            // 创建符号
            // koopaIR->curScope->symbolTable.AddNewVarSymbol(symbolDef_ptr->SymbolName, type);
            //  SymbolItem *symbol_item = new SymbolItem();
            //  symbol_item->InitVarSymbol(KOOPA_INT32);
            //  curScope->AddSymbol(symbolDef_ptr->SymbolName, symbol_item);

            // Statement *statement = new Statement();

            if (symbolDef_ptr->BinaryExpr->Value1->type == symbolDef_ptr->BinaryExpr->Value1->INT_TYPE)
            {
                value1 = std::to_string(symbolDef_ptr->BinaryExpr->Value1->INT->INT);
            }
            else if (symbolDef_ptr->BinaryExpr->Value1->type == symbolDef_ptr->BinaryExpr->Value1->SYMBOL_TYPE)
            {
                value1 = symbolDef_ptr->BinaryExpr->Value1->SYMBOL->SYMBOL;
            }
            else if (symbolDef_ptr->BinaryExpr->Value1->type == symbolDef_ptr->BinaryExpr->Value1->UNDEF_TYPE)
            {
                value1 = nullptr;
            }
            if (symbolDef_ptr->BinaryExpr->Value2->type == symbolDef_ptr->BinaryExpr->Value2->INT_TYPE)
            {
                value2 = std::to_string(symbolDef_ptr->BinaryExpr->Value2->INT->INT);
            }
            else if (symbolDef_ptr->BinaryExpr->Value2->type == symbolDef_ptr->BinaryExpr->Value2->SYMBOL_TYPE)
            {
                value2 = symbolDef_ptr->BinaryExpr->Value2->SYMBOL->SYMBOL;
            }
            else if (symbolDef_ptr->BinaryExpr->Value2->type == symbolDef_ptr->BinaryExpr->Value2->UNDEF_TYPE)
            {
                value2 = nullptr;
            }
            // std::string value1 = std::to_string(symbolDef_ptr->BinaryExpr->Value1->INT->INT);
            // std::string value2 = std::to_string(symbolDef_ptr->BinaryExpr->Value2->INT->INT);
            koopaIR->AddOperationStatement(op, value1, value2, type);
            // statement->InitOperationStatement(op,
            //                                   value1,
            //                                   value2,
            //                                   symbolDef_ptr->SymbolName);
            // curBlock->statements.push_back(statement);
        }
        return symbolDef_ptr;
    }

    return symbolDef_ptr;
}

// GlobalSymbolDef ::= "global" SYMBOL "=" GlobalMemoryDeclaration;
KoopaFrontend::GlobalSymbolDef_Struct *KoopaFrontend::GlobalSymbolDef_func()
{
    ENTRY_GRAMMER(KoopaFrontend::GlobalSymbolDef_Struct);
    GlobalSymbolDef_Struct *globalSymbolDef_ptr = new GlobalSymbolDef_Struct();
    if (curToken.rule[0] == "\"global\"")
    {
        RESERVED_func(); // global
        globalSymbolDef_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func(); // =
        globalSymbolDef_ptr->GlobalMemoryDeclaration = GlobalMemoryDeclaration_func();
        KoopaVar kv = koopaIR->NewVarWithInit(globalSymbolDef_ptr->GlobalMemoryDeclaration->Type->Type, globalSymbolDef_ptr->SYMBOL->SYMBOL, *(globalSymbolDef_ptr->GlobalMemoryDeclaration->Initializer->initList));
        std::cout << "............................globalsymboldef end" << std::endl;
        return globalSymbolDef_ptr;
    }

    return globalSymbolDef_ptr;
}

// GetPointer ::= "getptr" SYMBOL "," Value;
KoopaFrontend::GetPointer_Struct *KoopaFrontend::GetPointer_func()
{
    ENTRY_GRAMMER(KoopaFrontend::GetPointer_Struct);
    GetPointer_Struct *getPointer_ptr = new GetPointer_Struct();

    if (curToken.rule[0] == "\"getptr\"")
    {
        RESERVED_func(); // getptr
        getPointer_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func(); // ,
        getPointer_ptr->Value = Value_func();
        std::cout << "getpointer" << std::endl;
        return getPointer_ptr;
    }

    return getPointer_ptr;
}

// GetElementPointer ::= "getelemptr" SYMBOL "," Value;
KoopaFrontend::GetElementPointer_Struct *KoopaFrontend::GetElementPointer_func()
{
    ENTRY_GRAMMER(KoopaFrontend::GetElementPointer_Struct);
    GetElementPointer_Struct *getElementPointer_ptr = new GetElementPointer_Struct();
    if (curToken.rule[0] == "\"getelemptr\"")
    {
        RESERVED_func(); // getelemptr
        getElementPointer_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func(); // ,
        getElementPointer_ptr->Value = Value_func();
        return getElementPointer_ptr;
    }

    return getElementPointer_ptr;
}

// BinaryExpr ::= BINARY_OP Value "," Value;
KoopaFrontend::BinaryExpr_Struct *KoopaFrontend::BinaryExpr_func()
{
    ENTRY_GRAMMER(KoopaFrontend::BinaryExpr_Struct);
    BinaryExpr_Struct *binaryExpr_ptr = new BinaryExpr_Struct();
    if (curToken.rule[0] == "BINARY_OP")
    {
        binaryExpr_ptr->BINARY_OP = BINARY_OP_func();
        binaryExpr_ptr->Value1 = Value_func();
        RESERVED_func(); // ,
        binaryExpr_ptr->Value2 = Value_func();
        std::cout << "WWWW" << std::endl;
        return binaryExpr_ptr;
    }

    return binaryExpr_ptr;
}

// Branch ::= "br" Value "," SYMBOL [BlockArgList] "," SYMBOL [BlockArgList];
KoopaFrontend::Branch_Struct *KoopaFrontend::Branch_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Branch_Struct);
    Branch_Struct *branch_ptr = new Branch_Struct();
    if (curToken.rule[0] == "\"br\"")
    {
        int count = 0;
        RESERVED_func(); // br
        branch_ptr->Value = Value_func();
        RESERVED_func(); // ,
        branch_ptr->SYMBOL1 = SYMBOL_func();
        if (curToken.rule[5] == "BlockArgList")
        {
            branch_ptr->BlockArgList1 = BlockArgList_func();
            count++;
        }
        RESERVED_func(); // ,
        branch_ptr->SYMBOL2 = SYMBOL_func();
        std::cout << "branch end" << std::endl;
        if (6+count < curToken.rule.size() && curToken.rule[6 + count] == "BlockArgList")
        {
            branch_ptr->BlockArgList2 = BlockArgList_func();
        }
        std::cout << "branch end" << std::endl;
        return branch_ptr;
    }

    return branch_ptr;
}

// Jump ::= "jump" SYMBOL [BlockArgList];
KoopaFrontend::Jump_Struct *KoopaFrontend::Jump_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Jump_Struct);
    Jump_Struct *jump_ptr = new Jump_Struct();
    if (curToken.rule[0] == "\"jump\"")
    {
        RESERVED_func(); // jump
        std::cout << "jump" << std::endl;
        jump_ptr->SYMBOL = SYMBOL_func();
        if (curToken.rule[2] == "BlockArgList")
            jump_ptr->BlockArgList = BlockArgList_func();
        return jump_ptr;
    }

    return jump_ptr;
}

// BlockArgList ::= "(" Value {"," Value}")";
KoopaFrontend::BlockArgList_Struct *KoopaFrontend::BlockArgList_func()
{
    ENTRY_GRAMMER(KoopaFrontend::BlockArgList_Struct);
    BlockArgList_Struct *blockArgList_ptr = new BlockArgList_Struct();
    std::cout << "ba" << std::endl;
    if (curToken.rule[0] == "\"(\"")
    {
        RESERVED_func(); // (
        if (curToken.rule[1] == "Value")
        {
            blockArgList_ptr->Value = Value_func();
            int i = 2;
            while (curToken.rule[i] == "\",\"")
            {
                RESERVED_func(); // ,
                blockArgList_ptr->ValueList.push_back(Value_func());
                i += 2;
            }
        }
        RESERVED_func(); // )
        return blockArgList_ptr;
    }

    return blockArgList_ptr;
}

// EndStatement ::= Branch| Jump |Return;
KoopaFrontend::EndStatement_Struct *KoopaFrontend::EndStatement_func()
{
    ENTRY_GRAMMER(KoopaFrontend::EndStatement_Struct);
    EndStatement_Struct *endStatement_ptr = new EndStatement_Struct();
    if (curToken.rule[0] == "Branch")
    {
        endStatement_ptr->Branch = Branch_func();
        koopaIR->AddBranchStatement(endStatement_ptr->Branch->Value->SYMBOL->SYMBOL, endStatement_ptr->Branch->SYMBOL1->SYMBOL, endStatement_ptr->Branch->SYMBOL2->SYMBOL);
        std::cout << "endstatement:branch end" << std::endl;
        return endStatement_ptr;
    }
    if (curToken.rule[0] == "Jump")
    {
        endStatement_ptr->Jump = Jump_func();
        koopaIR->AddJumpStatement(endStatement_ptr->Jump->SYMBOL->SYMBOL);
        return endStatement_ptr;
    }
    if (curToken.rule[0] == "Return")
    {
        endStatement_ptr->Return = Return_func();
        std::cout << "endstatement:return" << std::endl;
        return endStatement_ptr;
    }

    return endStatement_ptr;
}

// FunCall ::= "call" SYMBOL "(" [Value {"," Value}] ")";
KoopaFrontend::FunCall_Struct *KoopaFrontend::FunCall_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunCall_Struct);
    FunCall_Struct *funCall_ptr = new FunCall_Struct();
    if (curToken.rule[0] == "\"call\"")
    {
        RESERVED_func(); // call
        funCall_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func(); // (
        if (curToken.rule[3] == "Value")
        {
            std::cout << "call:value" << std::endl;
            funCall_ptr->Value.push_back(Value_func());
            int i = 4;
            while (curToken.rule[i] == "\",\"")
            {
                RESERVED_func(); // ,
                funCall_ptr->Value.push_back(Value_func());
                i += 2;
            }
        }
        RESERVED_func(); // )
        std::cout << "funcall end" << std::endl;
        return funCall_ptr;
    }

    return funCall_ptr;
}

// FunParams ::= SYMBOL ":" Type {"," SYMBOL ":" Type};
KoopaFrontend::FunParams_Struct *KoopaFrontend::FunParams_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunParams_Struct);
    FunParams_Struct *funParams_ptr = new FunParams_Struct();
    if (curToken.rule[0] == "SYMBOL")
    {
        funParams_ptr->SYMBOL.push_back(SYMBOL_func());
        RESERVED_func(); // :
        funParams_ptr->Type.push_back(Type_func());
        int i = 3;
        while (curToken.rule[i] == "\",\"")
        {
            RESERVED_func(); // ,
            funParams_ptr->SYMBOL.push_back(SYMBOL_func());
            RESERVED_func(); // :
            funParams_ptr->Type.push_back(Type_func());
            i += 4;
        }
        return funParams_ptr;
    }

    return funParams_ptr;
}

// BlockParamList ::= "(" SYMBOL ":" Type {"," SYMBOL ":" Type} ")";
KoopaFrontend::BlockParamList_Struct *KoopaFrontend::BlockParamList_func()
{
    ENTRY_GRAMMER(KoopaFrontend::BlockParamList_Struct);
    BlockParamList_Struct *blockParamList_ptr = new BlockParamList_Struct();
    std::cout << "bpl" << std::endl;
    if (curToken.rule[0] == "\"(\"")
    {
        RESERVED_func(); // (
        blockParamList_ptr->SYMBOL.push_back(SYMBOL_func());
        RESERVED_func(); // :
        blockParamList_ptr->Type.push_back(Type_func());
        int i = 4;
        while (curToken.rule[i] == "\",\"")
        {
            RESERVED_func(); // ,
            blockParamList_ptr->SYMBOL.push_back(SYMBOL_func());
            RESERVED_func(); // :
            blockParamList_ptr->Type.push_back(Type_func());
            i += 4;
        }
        RESERVED_func(); // )
        return blockParamList_ptr;
    }

    return blockParamList_ptr;
}

// FunDecl ::= "decl" SYMBOL "(" [FunDeclParams] ")" [":" Type];
KoopaFrontend::FunDecl_Struct *KoopaFrontend::FunDecl_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunDecl_Struct);
    FunDecl_Struct *funDecl_ptr = new FunDecl_Struct();

    if (curToken.rule[0] == "\"decl\"")
    {
        RESERVED_func(); // decl
        funDecl_ptr->SYMBOL = SYMBOL_func();
        RESERVED_func(); // (
        int count = 0;
        if (curToken.rule[3] == "FunDeclParams")
        {
            funDecl_ptr->FunDeclParams = FunDeclParams_func();
            count++;
        }
        RESERVED_func(); // )

        if (curToken.rule[4 + count] == "\":\"")
        {
            RESERVED_func(); // :
            funDecl_ptr->Type = Type_func();
            std::vector<KoopaVarType> *params = new std::vector<KoopaVarType>();
            for (auto t : funDecl_ptr->FunDeclParams->Type)
            {
                params->push_back(t->Type);
            }
            KoopaVarType koopaVarType = koopaVarType.FUNC_Type(funDecl_ptr->Type->Type, *params);
            std::cout << "-----------------------------FundelType:" << std::endl;
            std::cout << "HHHHHH" << KoopaVarTypeToString(koopaVarType) << std::endl;
            std::cout << "sdsa" << std::endl;
            koopaIR->NewVar(koopaVarType, funDecl_ptr->SYMBOL->SYMBOL);
            std::cout << "............fundelend" << std::endl;
            return funDecl_ptr;
        }
        std::vector<KoopaVarType> *params = new std::vector<KoopaVarType>();
        for (auto t : funDecl_ptr->FunDeclParams->Type)
        {
            params->push_back(t->Type);
        }
        koopaIR->NewVar(KoopaVarType::KOOPA_undef, funDecl_ptr->SYMBOL->SYMBOL);
        return funDecl_ptr;
    }

    return funDecl_ptr;
}

// FunDeclParams ::= Type {"," Type};
KoopaFrontend::FunDeclParams_Struct *KoopaFrontend::FunDeclParams_func()
{
    ENTRY_GRAMMER(KoopaFrontend::FunDeclParams_Struct);
    FunDeclParams_Struct *funDeclParams_ptr = new FunDeclParams_Struct();
    if (curToken.rule[0] == "Type")
    {
        funDeclParams_ptr->Type.push_back(Type_func());
        int i = 1;
        while (curToken.rule[i] == "\",\"")
        {
            RESERVED_func(); // ,
            funDeclParams_ptr->Type.push_back(Type_func());
            i += 2;
        }
        return funDeclParams_ptr;
    }

    return funDeclParams_ptr;
}

// TAB
KoopaFrontend::TAB_Struct *KoopaFrontend::TAB_func()
{
    ENTRY_GRAMMER(KoopaFrontend::TAB_Struct);
    TAB_Struct *tab_ptr = new TAB_Struct();
    std::cout << "tab" << std::endl;
    std::cout << "tab" << std::endl;

    return tab_ptr;
}

// Return ::= "ret" (Value | TAB);
KoopaFrontend::Return_Struct *KoopaFrontend::Return_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Return_Struct);
    Return_Struct *return_ptr = new Return_Struct();
    RESERVED_func(); // ret

    if (curToken.rule[1] == "Value")
    {
        std::cout << "return" << std::endl;
        return_ptr->Value = Value_func();
        std::cout << "return" << std::endl;
        if (return_ptr->Value->type == Value_Struct::SYMBOL_TYPE)
            koopaIR->AddReturnStatement(return_ptr->Value->SYMBOL->SYMBOL);
        else if (return_ptr->Value->type == Value_Struct::INT_TYPE)
            koopaIR->AddReturnStatement(std::to_string(return_ptr->Value->INT->INT));
        else if (return_ptr->Value->type == Value_Struct::UNDEF_TYPE)
            koopaIR->AddReturnStatement(KoopaSymbol());
        std::cout << "return_func" << std::endl;
    }
    if (curToken.rule[1] == "TAB")
    {
        TAB_func();
        koopaIR->AddReturnStatement(KoopaSymbol());
    }
    // Statement *statement = new Statement();
    // statement->InitReturnStatement(return_ptr->Value->SYMBOL->SYMBOL);
    // curBlock->statements.push_back(statement);

    return return_ptr;
}

// Value ::= SYMBOL | INT | "undef";
KoopaFrontend::Value_Struct *KoopaFrontend::Value_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Value_Struct);
    Value_Struct *value_ptr = new Value_Struct();
    if (curToken.rule[0] == "SYMBOL")
    {
        value_ptr->type = Value_Struct::SYMBOL_TYPE;
        value_ptr->SYMBOL = SYMBOL_func();
        return value_ptr;
    }
    if (curToken.rule[0] == "INT")
    {
        std::cout << "value::int" << std::endl;
        value_ptr->type = Value_Struct::INT_TYPE;
        value_ptr->INT = INT_func();
        std::cout << "value::int" << std::endl;
        return value_ptr;
    }
    if (curToken.rule[0] == "\"undef\"")
    {
        value_ptr->type = Value_Struct::UNDEF_TYPE;
        return value_ptr;
    }

    return value_ptr;
}

// Initializer   ::= INT | "undef" | Aggregate | "zeroinit";
KoopaFrontend::Initializer_Struct *KoopaFrontend::Initializer_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Initializer_Struct);
    Initializer_Struct *initializer_ptr = new Initializer_Struct();
    if (curToken.rule[0] == "INT")
    {
        initializer_ptr->type = Initializer_Struct::INT_TYPE;
        initializer_ptr->INT = INT_func();
        initializer_ptr->initList = new KoopaInitList(initializer_ptr->INT->INT);
        return initializer_ptr;
    }
    if (curToken.rule[0] == "\"undef\"")
    {
        initializer_ptr->type = Initializer_Struct::UNDEF_TYPE;
        initializer_ptr->initList = new KoopaInitList();
        return initializer_ptr;
    }
    if (curToken.rule[0] == "Aggregate")
    {
        initializer_ptr->type = Initializer_Struct::AGGREGATE_TYPE;
        initializer_ptr->Aggregate = Aggregate_func();
        std::cout << "Initializer:Aggregate" << std::endl;
        std::vector<KoopaInitList> *init_list = new std::vector<KoopaInitList>();
        for (auto i : initializer_ptr->Aggregate->InitializerList)
        {
            init_list->push_back(*(i->initList));
        }
        initializer_ptr->initList = new KoopaInitList(*init_list);
        std::cout << "Initializer:Aggregate end" << std::endl;
        return initializer_ptr;
    }
    if (curToken.rule[0] == "\"zeroinit\"")
    {
        RESERVED_func(); // zeroinit
        initializer_ptr->type = Initializer_Struct::ZEROINIT_TYPE;
        initializer_ptr->initList = new KoopaInitList("zeroinit");
        return initializer_ptr;
    }

    return initializer_ptr;
}

// Aggregate     ::= "{" [Initializer {"," Initializer}] "}";
KoopaFrontend::Aggregate_Struct *KoopaFrontend::Aggregate_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Aggregate_Struct);
    Aggregate_Struct *aggregate_ptr = new Aggregate_Struct();
    RESERVED_func(); // {
    if (curToken.rule[1] == "Initializer")
    {
        aggregate_ptr->InitializerList.push_back(Initializer_func());
        int i = 2;
        while (curToken.rule[i] == "\",\"")
        {
            RESERVED_func(); // ,
            aggregate_ptr->InitializerList.push_back(Initializer_func());
            i += 2;
        }
    }
    RESERVED_func(); // }

    return aggregate_ptr;
}

// MemoryDeclaration ::= "alloc" Type
KoopaFrontend::MemoryDeclaration_Struct *KoopaFrontend::MemoryDeclaration_func()
{
    ENTRY_GRAMMER(KoopaFrontend::MemoryDeclaration_Struct);
    MemoryDeclaration_Struct *memoryDeclaration_ptr = new MemoryDeclaration_Struct();
    if (curToken.rule[0] == "\"alloc\"")
    {
        RESERVED_func(); // alloc
        memoryDeclaration_ptr->Type = Type_func();
        return memoryDeclaration_ptr;
    }

    return memoryDeclaration_ptr;
}

// GlobalMemoryDeclaration ::= "alloc" Type "," Initializer;
KoopaFrontend::GlobalMemoryDeclaration_Struct *KoopaFrontend::GlobalMemoryDeclaration_func()
{
    ENTRY_GRAMMER(KoopaFrontend::GlobalMemoryDeclaration_Struct);
    GlobalMemoryDeclaration_Struct *globalMemoryDeclaration_ptr = new GlobalMemoryDeclaration_Struct();
    if (curToken.rule[0] == "\"alloc\"")
    {
        RESERVED_func(); // alloc
        globalMemoryDeclaration_ptr->Type = Type_func();
        RESERVED_func(); // ,
        globalMemoryDeclaration_ptr->Initializer = Initializer_func();
        std::cout << "............................globalmemorydeclaration end" << std::endl;
        return globalMemoryDeclaration_ptr;
    }

    return globalMemoryDeclaration_ptr;
}

// Load ::= "load" SYMBOL
KoopaFrontend::Load_Struct *KoopaFrontend::Load_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Load_Struct);
    Load_Struct *load_ptr = new Load_Struct();
    if (curToken.rule[0] == "\"load\"")
    {
        RESERVED_func(); // load
        load_ptr->SYMBOL = SYMBOL_func();
        return load_ptr;
    }

    return load_ptr;
}

// Store ::= "store" (SYMBOL | Initializer) "," SYMBOL;
KoopaFrontend::Store_Struct *KoopaFrontend::Store_func()
{
    ENTRY_GRAMMER(KoopaFrontend::Store_Struct);
    Store_Struct *store_ptr = new Store_Struct();
    if (curToken.rule[0] == "\"store\"")
    {
        RESERVED_func(); // store
        if (curToken.rule[1] == "SYMBOL")
        {
            store_ptr->SYMBOL0 = SYMBOL_func();
            RESERVED_func(); // ,
            store_ptr->SYMBOL = SYMBOL_func();
            koopaIR->AddStoreStatement(store_ptr->SYMBOL->SYMBOL, *(new KoopaSymbol(store_ptr->SYMBOL0->SYMBOL)));
        }

        else if (curToken.rule[1] == "Initializer")
        {
            store_ptr->Initializer = Initializer_func();
            RESERVED_func(); // ,
            store_ptr->SYMBOL = SYMBOL_func();
            koopaIR->AddStoreStatement(store_ptr->SYMBOL->SYMBOL, *(store_ptr->Initializer->initList));
        }

        return store_ptr;
    }

    return store_ptr;
}

// SYMBOL
KoopaFrontend::SYMBOL_Struct *KoopaFrontend::SYMBOL_func()
{
    ENTRY_GRAMMER(KoopaFrontend::SYMBOL_Struct);
    SYMBOL_Struct *symbol_ptr = new SYMBOL_Struct();
    std::cout << "Symbold" << std::endl;
    if (curToken.token == "SYMBOL")
    {
        std::cout << "Symbold" << std::endl;
        symbol_ptr->SYMBOL = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        return symbol_ptr;
    }

    return symbol_ptr;
}

// INT
KoopaFrontend::INT_Struct *KoopaFrontend::INT_func()
{
    ENTRY_GRAMMER(KoopaFrontend::INT_Struct);
    INT_Struct *int_ptr = new INT_Struct();
    std::cout << "sdada" << curToken.rule[0] << std::endl;
    if (curToken.token == "INT")
    {
        std::cout << "sdada" << curToken.rule[0] << std::endl;
        std::string int_num = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        int_ptr->INT = std::stoi(int_num, nullptr, 10);
        return int_ptr;
    }

    return int_ptr;
}

// BINARY_OP
KoopaFrontend::BINARY_OP_Struct *KoopaFrontend::BINARY_OP_func()
{
    ENTRY_GRAMMER(KoopaFrontend::BINARY_OP_Struct);
    BINARY_OP_Struct *binary_op_ptr = new BINARY_OP_Struct();
    if (curToken.token == "BINARY_OP")
    {
        binary_op_ptr->BINARY_OP = curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        return binary_op_ptr;
    }

    return binary_op_ptr;
}

// RESERVED
KoopaFrontend::RESERVED_Struct *KoopaFrontend::RESERVED_func()
{
    ENTRY_GRAMMER(KoopaFrontend::RESERVED_Struct);
    RESERVED_Struct *reserved_ptr = new RESERVED_Struct();
    reserved_ptr->RESERVED = curToken.token;
    return reserved_ptr;

    return reserved_ptr;
}
//******************************************************************************
