#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

// 建树与解析函数
// CompUnits      ::= CompUnit {CompUnit};
SysyFrontend::CompUnits_Struct *SysyFrontend::CompUnits_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::CompUnits_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::CompUnit_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::FuncDef_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // 创建函数
    // FuncDef   ::= FuncType IDENT "(" ")" Block;
    if (curToken.rule[0] == "FuncType")
    {
        //-----------------------------------------------------------------
        // 创建作用域
        Scope *scope = koopaIR->AddNewScope();
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        ret_ptr->funcRetType = FuncType_func();
        ret_ptr->funcnameIDENT = IDENT_func();
        RESERVED_func(); //(
        RESERVED_func(); //)
        ret_ptr->Block = Block_func(ret_ptr->funcnameIDENT->identifer);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 退出作用域
        koopaIR->SetScopeFunction(ret_ptr->funcnameIDENT->identifer,
                                  ret_ptr->funcRetType->type);
        koopaIR->ScopeAddBasicBlock(ret_ptr->Block->block);
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::FuncType_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
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

// Block     ::= "{" Stmt "}";
SysyFrontend::Block_Struct *SysyFrontend::Block_func(std::string block_name)
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Block_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Block     ::= "{" Stmt "}";
    if (curToken.rule[0] == "\"{\"")
    {
        //-----------------------------------------------------------------
        // 创建基本块，并加入符号表,更新当前块
        std::string block_name = koopaIR->GetUniqueName("%entry");
        BasicBlock *block = koopaIR->NewBasicBlockAndSetCur(block_name);
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 创建语句
        RESERVED_func(); //{
        Stmt_Struct *stmt_ptr = Stmt_func();
        RESERVED_func(); //}
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 属性赋值
        ret_ptr->block = block;
        //-----------------------------------------------------------------
    }
    else
    {
        std::cerr << "Block_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Stmt      ::= "return" Exp ";";
SysyFrontend::Stmt_Struct *SysyFrontend::Stmt_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Stmt_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Stmt      ::= "return" Exp ";";
    if (curToken.rule[0] == "\"return\"")
    {
        //-----------------------------------------------------------------
        RESERVED_func(); // return
        ret_ptr->Exp = Exp_func();
        RESERVED_func(); //;
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 返回语句
        koopaIR->AddReturnStatement(ret_ptr->Exp->value);
    }
    else
    {
        std::cerr << "Stmt_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Exp         ::= LOrExp;
SysyFrontend::Exp_Struct *SysyFrontend::Exp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Exp_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Exp         ::= LOrExp;
    if (curToken.rule[0] == "LOrExp")
    {
        ret_ptr->LOrExp = LOrExp_func();
        ret_ptr->value = ret_ptr->LOrExp->value;
    }
    else
    {
        std::cerr << "Exp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
SysyFrontend::UnaryExp_Struct *SysyFrontend::UnaryExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::UnaryExp_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
    if (curToken.rule[0] == "PrimaryExp")
    {
        ret_ptr->type = UnaryExp_Struct::UnaryExpType::UnaryExpType_PrimaryExp;
        ret_ptr->subStructPointer.PrimaryExp = PrimaryExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.PrimaryExp->value;
    }
    else if (curToken.rule[0] == "UnaryOp")
    {
        ret_ptr->type = UnaryExp_Struct::UnaryExpType::UnaryExpType_UnaryExpWithOp;
        SysyFrontend::UnaryOp_Struct *op = UnaryOp_func();
        SysyFrontend::UnaryExp_Struct *exp = UnaryExp_func();
        ret_ptr->subStructPointer.UnaryExpWithOp.UnaryOp = op;
        ret_ptr->subStructPointer.UnaryExpWithOp.UnaryExp = exp;
        if (op->type == UnaryOp_Struct::UnaryOpType::UnaryOpType_Add)
        {
            ret_ptr->value = exp->value;
        }
        else if (op->type == UnaryOp_Struct::UnaryOpType::UnaryOpType_Sub)
        {
            KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
            koopaIR->AddOperationStatement("sub", 0, exp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (op->type == UnaryOp_Struct::UnaryOpType::UnaryOpType_Not)
        {
            KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
            koopaIR->AddOperationStatement("not", 0, exp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else
        {
            std::cerr << "UnaryExp_func: " << curToken << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "UnaryExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// UnaryOp     ::= "+" | "-" | "!";
SysyFrontend::UnaryOp_Struct *SysyFrontend::UnaryOp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::UnaryOp_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // UnaryOp     ::= "+" | "-" | "!";
    if (curToken.rule[0] == "\"+\"")
    {
        ret_ptr->type = UnaryOp_Struct::UnaryOpType::UnaryOpType_Add;
        RESERVED_func(); //+
    }
    else if (curToken.rule[0] == "\"-\"")
    {
        ret_ptr->type = UnaryOp_Struct::UnaryOpType::UnaryOpType_Sub;
        RESERVED_func(); //-
    }
    else if (curToken.rule[0] == "\"!\"")
    {
        ret_ptr->type = UnaryOp_Struct::UnaryOpType::UnaryOpType_Not;
        RESERVED_func(); //!
    }
    else
    {
        std::cerr << "UnaryOp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// PrimaryExp  ::= "(" Exp ")" | Number;
SysyFrontend::PrimaryExp_Struct *SysyFrontend::PrimaryExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::PrimaryExp_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // PrimaryExp  ::= "(" Exp ")" | Number;
    if (curToken.rule[0] == "\"(\"")
    {
        //-----------------------------------------------------------------
        RESERVED_func(); //(
        ret_ptr->type = PrimaryExp_Struct::PrimaryExpType::PrimaryExpType_Exp;
        ret_ptr->subStructPointer.Exp = Exp_func();
        ret_ptr->value = ret_ptr->subStructPointer.Exp->value;
        RESERVED_func(); //)
    }
    else if (curToken.rule[0] == "Number")
    {
        ret_ptr->type = PrimaryExp_Struct::PrimaryExpType::PrimaryExpType_Number;
        ret_ptr->subStructPointer.Number = Number_func();
        ret_ptr->value = ret_ptr->subStructPointer.Number->value;
    }
    else
    {
        std::cerr << "PrimaryExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
SysyFrontend::MulExp_Struct *SysyFrontend::MulExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::MulExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
    if (curToken.rule[0] == "UnaryExp")
    {
        //-----------------------------------------------------------------
        UnaryExp_Struct *unaryExp_ptr = UnaryExp_func();

        //-----------------------------------------------------------------
        ret_ptr->type = MulExp_Struct::MulExpType::MulExpType_UnaryExp;
        ret_ptr->subStructPointer.UnaryExp = unaryExp_ptr;
        ret_ptr->value = unaryExp_ptr->value;
    }
    else if (curToken.rule[0] == "MulExp")
    {
        //-----------------------------------------------------------------
        MulExp_Struct *mulExp_ptr = MulExp_func();
        RESERVED_func(); // "*" | "/" | "%"
        UnaryExp_Struct *unaryExp_ptr = UnaryExp_func();

        //-----------------------------------------------------------------
        ret_ptr->type = MulExp_Struct::MulExpType::MulExpType_MulAndUnary;
        ret_ptr->subStructPointer.MulAndUnary.MulExp = mulExp_ptr;
        ret_ptr->subStructPointer.MulAndUnary.UnaryExp = unaryExp_ptr;
        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);

        if (curToken.rule[1] == "\"*\"")
        {
            ret_ptr->subStructPointer.MulAndUnary.op = MulExp_Struct::OpType::OpType_Mul;
            koopaIR->AddOperationStatement("mul", mulExp_ptr->value, unaryExp_ptr->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\"/\"")
        {
            ret_ptr->subStructPointer.MulAndUnary.op = MulExp_Struct::OpType::OpType_Div;
            koopaIR->AddOperationStatement("div", mulExp_ptr->value, unaryExp_ptr->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\"%\"")
        {
            ret_ptr->subStructPointer.MulAndUnary.op = MulExp_Struct::OpType::OpType_Mod;
            koopaIR->AddOperationStatement("mod", mulExp_ptr->value, unaryExp_ptr->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else
        {
            std::cerr << "MulExp_func: " << curToken << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "MulExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
SysyFrontend::AddExp_Struct *SysyFrontend::AddExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::AddExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
    if (curToken.rule[0] == "MulExp")
    {
        //-----------------------------------------------------------------
        MulExp_Struct *mulExp_ptr = MulExp_func();

        //-----------------------------------------------------------------
        ret_ptr->type = AddExp_Struct::AddExpType::AddExpType_MulExp;
        ret_ptr->subStructPointer.MulExp = mulExp_ptr;
        ret_ptr->value = mulExp_ptr->value;
    }
    else if (curToken.rule[0] == "AddExp")
    {
        //-----------------------------------------------------------------
        AddExp_Struct *addExp_ptr = AddExp_func();
        RESERVED_func(); // "+" | "-"
        MulExp_Struct *mulExp_ptr = MulExp_func();

        //-----------------------------------------------------------------
        ret_ptr->type = AddExp_Struct::AddExpType::AddExpType_AddAndMul;
        ret_ptr->subStructPointer.AddAndMul.AddExp = addExp_ptr;
        ret_ptr->subStructPointer.AddAndMul.MulExp = mulExp_ptr;
        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);

        if (curToken.rule[1] == "\"+\"")
        {
            ret_ptr->subStructPointer.AddAndMul.op = AddExp_Struct::OpType::OpType_Add;
            koopaIR->AddOperationStatement("add", addExp_ptr->value, mulExp_ptr->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\"-\"")
        {
            ret_ptr->subStructPointer.AddAndMul.op = AddExp_Struct::OpType::OpType_Sub;
            koopaIR->AddOperationStatement("sub", addExp_ptr->value, mulExp_ptr->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else
        {
            std::cerr << "AddExp_func: " << curToken << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "AddExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
SysyFrontend::RelExp_Struct *SysyFrontend::RelExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::RelExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
    if (curToken.rule[0] == "AddExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = RelExp_Struct::RelExpType::RelExpType_AddExp;
        ret_ptr->subStructPointer.AddExp = AddExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.AddExp->value;
    }
    else if (curToken.rule[0] == "RelExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = RelExp_Struct::RelExpType::RelExpType_RelAndAdd;
        RelExp_Struct *relExp = RelExp_func();
        RESERVED_func(); // "<" | ">" | "<=" | ">="
        AddExp_Struct *addExp = AddExp_func();

        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
        if (curToken.rule[1] == "\"<\"")
        {
            ret_ptr->subStructPointer.RelAndAdd.op = RelExp_Struct::OpType::OpType_LT;
            koopaIR->AddOperationStatement("lt", relExp->value, addExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\">\"")
        {
            ret_ptr->subStructPointer.RelAndAdd.op = RelExp_Struct::OpType::OpType_GT;
            koopaIR->AddOperationStatement("gt", relExp->value, addExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\"<=\"")
        {
            ret_ptr->subStructPointer.RelAndAdd.op = RelExp_Struct::OpType::OpType_LE;
            koopaIR->AddOperationStatement("le", relExp->value, addExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\">=\"")
        {
            ret_ptr->subStructPointer.RelAndAdd.op = RelExp_Struct::OpType::OpType_GE;
            koopaIR->AddOperationStatement("ge", relExp->value, addExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else
        {
            std::cerr << "RelExp_func: " << curToken << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "RelExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
SysyFrontend::EqExp_Struct *SysyFrontend::EqExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::EqExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
    if (curToken.rule[0] == "RelExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = EqExp_Struct::EqExpType::EqExpType_RelExp;
        ret_ptr->subStructPointer.RelExp = RelExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.RelExp->value;
    }
    else if (curToken.rule[0] == "EqExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = EqExp_Struct::EqExpType::EqExpType_EqAndRel;
        EqExp_Struct *eqExp = EqExp_func();
        RESERVED_func(); // "==" | "!="
        RelExp_Struct *relExp = RelExp_func();

        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
        if (curToken.rule[1] == "\"==\"")
        {
            ret_ptr->subStructPointer.EqAndRel.op = EqExp_Struct::OpType::OpType_EQ;
            koopaIR->AddOperationStatement("eq", eqExp->value, relExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else if (curToken.rule[1] == "\"!=\"")
        {
            ret_ptr->subStructPointer.EqAndRel.op = EqExp_Struct::OpType::OpType_NE;
            koopaIR->AddOperationStatement("ne", eqExp->value, relExp->value, ret_var);
            ret_ptr->value.SetSymbol(ret_var.varName);
        }
        else
        {
            std::cerr << "EqExp_func: " << curToken << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "EqExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// LAndExp     ::= EqExp | LAndExp "&&" EqExp;
SysyFrontend::LAndExp_Struct *SysyFrontend::LAndExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::LAndExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // LAndExp     ::= EqExp | LAndExp "&&" EqExp;
    if (curToken.rule[0] == "EqExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = LAndExp_Struct::LAndExpType::LAndExpType_EqExp;
        ret_ptr->subStructPointer.EqExp = EqExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.EqExp->value;
    }
    else if (curToken.rule[0] == "LAndExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = LAndExp_Struct::LAndExpType::LAndExpType_LAndAndEq;
        LAndExp_Struct *lAndExp = LAndExp_func();
        RESERVED_func(); // "&&"
        EqExp_Struct *eqExp = EqExp_func();

        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
        koopaIR->AddOperationStatement("and", lAndExp->value, eqExp->value, ret_var);
        ret_ptr->value.SetSymbol(ret_var.varName);
    }
    else
    {
        std::cerr << "LAndExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// LOrExp      ::= LAndExp | LOrExp "||" LAndExp;
SysyFrontend::LOrExp_Struct *SysyFrontend::LOrExp_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::LOrExp_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // LOrExp      ::= LAndExp | LOrExp "||" LAndExp;
    if (curToken.rule[0] == "LAndExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = LOrExp_Struct::LOrExpType::LOrExpType_LAndExp;
        ret_ptr->subStructPointer.LAndExp = LAndExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.LAndExp->value;
    }
    else if (curToken.rule[0] == "LOrExp")
    {
        //-----------------------------------------------------------------
        ret_ptr->type = LOrExp_Struct::LOrExpType::LOrExpType_LOrAndLAnd;
        LOrExp_Struct *lOrExp = LOrExp_func();
        RESERVED_func(); // "||"
        LAndExp_Struct *lAndExp = LAndExp_func();

        KoopaVar ret_var = koopaIR->NewTempVar(KoopaVarType::KOOPA_INT32);
        koopaIR->AddOperationStatement("or", lOrExp->value, lAndExp->value, ret_var);
        ret_ptr->value.SetSymbol(ret_var.varName);
    }
    else
    {
        std::cerr << "LOrExp_func: " << curToken << std::endl;
        exit(1);
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Number    ::= Integer;
SysyFrontend::Number_Struct *SysyFrontend::Number_func()
{
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Number_Struct)
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Number    ::= Integer;
    if (curToken.rule[0] == "Integer")
    {
        //-----------------------------------------------------------------
        Integer_Struct *integer_ptr = Integer_func();

        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Integer_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if (curToken.rule[0] == "OCT_INTEGER")
    {
        //-----------------------------------------------------------------
        OCT_INTEGER_Struct *octInteger_ptr = OCT_INTEGER_func();

        //-----------------------------------------------------------------
        ret_ptr->type = Integer_Struct::IntegerType::IntegerType_OCT_INTEGER;
        ret_ptr->subStructPointer.OCT_INTEGER = octInteger_ptr;
        ret_ptr->value = octInteger_ptr->value_int;
    }
    else if (curToken.rule[0] == "HEX_INTEGER")
    {
        //-----------------------------------------------------------------
        HEX_INTEGER_Struct *hexInteger_ptr = HEX_INTEGER_func();

        //-----------------------------------------------------------------
        ret_ptr->type = Integer_Struct::IntegerType::IntegerType_HEX_INTEGER;
        ret_ptr->subStructPointer.HEX_INTEGER = hexInteger_ptr;
        ret_ptr->value = hexInteger_ptr->value_int;
    }
    else if (curToken.rule[0] == "DEC_INTEGER")
    {
        //-----------------------------------------------------------------
        DEC_INTEGER_Struct *decInteger_ptr = DEC_INTEGER_func();

        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::OCT_INTEGER_Struct)
    //-----------------------------------------------------------------
    if (curToken.token == "OCT_INTEGER")
    {
        //-----------------------------------------------------------------
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 8);
        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::HEX_INTEGER_Struct)
    //-----------------------------------------------------------------
    if (curToken.token == "HEX_INTEGER")
    {
        //-----------------------------------------------------------------
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 16);
        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::DEC_INTEGER_Struct)
    //-----------------------------------------------------------------
    if (curToken.token == "DEC_INTEGER")
    {
        //-----------------------------------------------------------------
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 10);
        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::IDENT_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if (curToken.token == "IDENT")
    {
        //-----------------------------------------------------------------
        ret_ptr->identifer = "@" + curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        //-----------------------------------------------------------------
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
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::RESERVED_Struct);
    //-----------------------------------------------------------------
    for (auto &ch : curToken.rule[0])
    {
        if (ch == '\n')
        {
            line_num++;
        }
    }
    //-----------------------------------------------------------------
    ret_ptr->reserved = curToken.rule[0];
    //-----------------------------------------------------------------
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}
//******************************************************************************
