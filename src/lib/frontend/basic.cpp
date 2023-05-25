#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

// CompUnits      ::= CompUnit {CompUnit};
SysyFrontend::CompUnits_Struct *SysyFrontend::CompUnits_func()
{
    ENTRY_GRAMMER(SysyFrontend::CompUnits_Struct);

    if (curToken.rule[0] == "CompUnit")
    {
        for (auto rule : curToken.rule)
        {
            if (rule == "CompUnit")
            {
                ret_ptr->CompUnit.push_back(CompUnit_func());
            }
        }
    }
    else
    {
        std::cerr << "CompUnits_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// CompUnit       ::= FuncDef;
SysyFrontend::CompUnit_Struct *SysyFrontend::CompUnit_func()
{
    ENTRY_GRAMMER(SysyFrontend::CompUnit_Struct);

    if (curToken.rule[0] == "FuncDef")
    {
        ret_ptr->FuncDef = FuncDef_func();
    }
    else
    {
        std::cerr << "CompUnit_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// FuncDef   ::= FuncType IDENT "(" ")" Block;
SysyFrontend::FuncDef_Struct *SysyFrontend::FuncDef_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncDef_Struct);

    // 创建函数
    // FuncDef   ::= FuncType IDENT "(" ")" Block;
    if (curToken.rule[0] == "FuncType")
    {
        // 创建作用域
        Scope *scope = koopaIR->AddNewScope();

        ret_ptr->funcRetType = FuncType_func();
        ret_ptr->funcnameIDENT = IDENT_func();
        RESERVED_func(); //(
        RESERVED_func(); //)
        ret_ptr->Block = Block_func("%entry");

        // 退出作用域
        koopaIR->SetScopeFunction(ret_ptr->funcnameIDENT->identifer,
                                  ret_ptr->funcRetType->type);
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

// FuncType  ::= "int";
SysyFrontend::FuncType_Struct *SysyFrontend::FuncType_func()
{
    ENTRY_GRAMMER(SysyFrontend::FuncType_Struct);

    // INT32
    // FuncType  ::= "int";
    if (curToken.rule[0] == "\"int\"")
    {
        ret_ptr->type.topType = KoopaVarType::KOOPA_INT32;
        RESERVED_func(); // "int"
    }
    else
    {
        std::cerr << "FuncType_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Block         ::= "{" {BlockItem} "}";
SysyFrontend::Block_Struct *SysyFrontend::Block_func(std::string block_name)
{
    ENTRY_GRAMMER(SysyFrontend::Block_Struct);

    // Block         ::= "{" {BlockItem} "}";
    if (curToken.rule[0] == "\"{\"")
    {
        // 创建基本块，并加入符号表,更新当前块
        BasicBlock *block = koopaIR->NewBasicBlockAndSetCur(block_name);
        koopaIR->ScopeAddBasicBlock(block);
        PushNameMap();

        // 创建语句
        RESERVED_func(); //{
        for (int i = 1; i < curToken.rule.size() - 1; i++)
        {
            if (curToken.rule[i] == "BlockItem")
            {
                ret_ptr->BlockItems.push_back(BlockItem_func());
            }
        }
        RESERVED_func(); //}

        // 属性赋值
        ret_ptr->block = block;
        PopNameMap();
    }
    else
    {
        std::cerr << "Block_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// BlockItem     ::= Decl | Stmt;
SysyFrontend::BlockItem_Struct *SysyFrontend::BlockItem_func()
{
    ENTRY_GRAMMER(SysyFrontend::BlockItem_Struct);

    // BlockItem     ::= Decl | Stmt;
    if (curToken.rule[0] == "Decl")
    {
        ret_ptr->type = BlockItem_Struct::BlockItemType::BlockItemType_Decl;
        ret_ptr->subStructPointer.Decl = Decl_func();
    }
    else if (curToken.rule[0] == "Stmt")
    {
        ret_ptr->type = BlockItem_Struct::BlockItemType::BlockItemType_Stmt;
        ret_ptr->subStructPointer.Stmt = Stmt_func();
    }
    else
    {
        std::cerr << "BlockItem_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Stmt          ::= LVal "=" Exp ";"
//                 | [Exp] ";"
//                 | Block
//                 | "return" [Exp] ";";
SysyFrontend::Stmt_Struct *SysyFrontend::Stmt_func()
{
    ENTRY_GRAMMER(SysyFrontend::Stmt_Struct);

    // Stmt          ::= LVal "=" Exp ";"
    //                 | [Exp] ";"
    //                 | Block
    //                 | "return" [Exp] ";";
    if (curToken.rule[0] == "\"return\"")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_Return;
        RESERVED_func(); // return
        if (curToken.rule[1] == "\";\"")
        {
            koopaIR->AddReturnStatement(KoopaSymbol());
        }
        else
        {
            ret_ptr->subStructPointer.Return = Exp_func();
            koopaIR->AddReturnStatement(ret_ptr->subStructPointer.Return->value);
        }
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "Exp")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_Exp;
        ret_ptr->subStructPointer.Return = Exp_func();
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "\";\"")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_Exp;
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "Block")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_Block;
        std::string inner_block = koopaIR->GetUniqueName("%entry");
        koopaIR->AddJumpStatement(inner_block);
        ret_ptr->subStructPointer.Block = Block_func(inner_block);

        std::string outer_block = koopaIR->GetUniqueName("%entry");
        koopaIR->AddJumpStatement(outer_block);

        BasicBlock* block = koopaIR->NewBasicBlockAndSetCur(outer_block);
        koopaIR->ScopeAddBasicBlock(block);
    }
    else if (curToken.rule[0] == "LVal")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_Assign;
        ret_ptr->subStructPointer.Assign.LVal = LVal_func();
        if (IsConst(ret_ptr->subStructPointer.Assign.LVal->ident))
        {
            std::cerr << "Assign to const variable: " << ret_ptr->subStructPointer.Assign.LVal->ident << std::endl;
            exit(1);
        }
        RESERVED_func(); // =
        KoopaVar receiver = koopaIR->GetVar(GetIRName(ret_ptr->subStructPointer.Assign.LVal->ident));
        ret_ptr->subStructPointer.Assign.Exp = Exp_func(&receiver);
        RESERVED_func(); //;
    }
    else
    {
        std::cerr << "Stmt_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Number    ::= Integer;
SysyFrontend::Number_Struct *SysyFrontend::Number_func()
{
    ENTRY_GRAMMER(SysyFrontend::Number_Struct)

    // Number    ::= Integer;
    if (curToken.rule[0] == "Integer")
    {
        Integer_Struct *integer_ptr = Integer_func();

        ret_ptr->Integer = integer_ptr;
        ret_ptr->value.SetImm(integer_ptr->value);
    }
    else
    {
        std::cerr << "Number_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Integer     ::= DEC_INTEGER | HEX_INTEGER | OCT_INTEGER;
SysyFrontend::Integer_Struct *SysyFrontend::Integer_func()
{
    ENTRY_GRAMMER(SysyFrontend::Integer_Struct);

    if (curToken.rule[0] == "OCT_INTEGER")
    {
        OCT_INTEGER_Struct *octInteger_ptr = OCT_INTEGER_func();

        ret_ptr->type = Integer_Struct::IntegerType::IntegerType_OCT_INTEGER;
        ret_ptr->subStructPointer.OCT_INTEGER = octInteger_ptr;
        ret_ptr->value = octInteger_ptr->value_int;
    }
    else if (curToken.rule[0] == "HEX_INTEGER")
    {
        HEX_INTEGER_Struct *hexInteger_ptr = HEX_INTEGER_func();

        ret_ptr->type = Integer_Struct::IntegerType::IntegerType_HEX_INTEGER;
        ret_ptr->subStructPointer.HEX_INTEGER = hexInteger_ptr;
        ret_ptr->value = hexInteger_ptr->value_int;
    }
    else if (curToken.rule[0] == "DEC_INTEGER")
    {
        DEC_INTEGER_Struct *decInteger_ptr = DEC_INTEGER_func();

        ret_ptr->type = Integer_Struct::IntegerType::IntegerType_DEC_INTEGER;
        ret_ptr->subStructPointer.DEC_INTEGER = decInteger_ptr;
        ret_ptr->value = decInteger_ptr->value_int;
    }
    else
    {
        std::cerr << "Integer_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

SysyFrontend::OCT_INTEGER_Struct *SysyFrontend::OCT_INTEGER_func()
{
    ENTRY_GRAMMER(SysyFrontend::OCT_INTEGER_Struct)
    if (curToken.token == "OCT_INTEGER")
    {
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 8);
        for (auto &ch : curToken.rule[0])
        {
            if (ch == '\n')
            {
                line_num++;
            }
        }
    }
    else
    {
        std::cerr << "OCT_INTEGER_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

SysyFrontend::HEX_INTEGER_Struct *SysyFrontend::HEX_INTEGER_func()
{
    ENTRY_GRAMMER(SysyFrontend::HEX_INTEGER_Struct)
    if (curToken.token == "HEX_INTEGER")
    {
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 16);
        for (auto &ch : curToken.rule[0])
        {
            if (ch == '\n')
            {
                line_num++;
            }
        }
    }
    else
    {
        std::cerr << "HEX_INTEGER_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

SysyFrontend::DEC_INTEGER_Struct *SysyFrontend::DEC_INTEGER_func()
{
    ENTRY_GRAMMER(SysyFrontend::DEC_INTEGER_Struct)
    if (curToken.token == "DEC_INTEGER")
    {
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 10);
        for (auto &ch : curToken.rule[0])
        {
            if (ch == '\n')
            {
                line_num++;
            }
        }
    }
    else
    {
        std::cerr << "DEC_INTEGER_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

SysyFrontend::IDENT_Struct *SysyFrontend::IDENT_func()
{
    ENTRY_GRAMMER(SysyFrontend::IDENT_Struct);

    if (curToken.token == "IDENT")
    {
        ret_ptr->identifer = "@" + curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        for (auto &ch : curToken.rule[0])
        {
            if (ch == '\n')
            {
                line_num++;
            }
        }
    }
    else
    {
        std::cerr << "IDENT_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

SysyFrontend::RESERVED_Struct *SysyFrontend::RESERVED_func()
{
    ENTRY_GRAMMER(SysyFrontend::RESERVED_Struct);
    for (auto &ch : curToken.rule[0])
    {
        if (ch == '\n')
        {
            line_num++;
        }
    }
    ret_ptr->reserved = curToken.rule[0];
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}