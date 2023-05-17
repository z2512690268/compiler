#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

// Exp         ::= LOrExp;
SysyFrontend::Exp_Struct *SysyFrontend::Exp_func()
{
    ENTRY_GRAMMER(SysyFrontend::Exp_Struct);

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
    ENTRY_GRAMMER(SysyFrontend::UnaryExp_Struct);

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
    ENTRY_GRAMMER(SysyFrontend::UnaryOp_Struct);

    // UnaryOp     ::= "+" | "-" | "!";
    if (curToken.rule[0] == "\"+\"")
    {
        ret_ptr->type = UnaryOp_Struct::UnaryOpType::UnaryOpType_Add;
        RESERVED_func(); //+
    }
    else if (curToken.rule[0] == "\"-\"")
    {
        ret_ptr->type = UnaryOp_Struct::UnaryOpType::UnaryOpType_Sub;
        RESERVED_func();
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
    ENTRY_GRAMMER(SysyFrontend::PrimaryExp_Struct);

    // PrimaryExp  ::= "(" Exp ")" | Number;
    if (curToken.rule[0] == "\"(\"")
    {
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
    ENTRY_GRAMMER(SysyFrontend::MulExp_Struct)

    // MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
    if (curToken.rule[0] == "UnaryExp")
    {
        UnaryExp_Struct *unaryExp_ptr = UnaryExp_func();

        ret_ptr->type = MulExp_Struct::MulExpType::MulExpType_UnaryExp;
        ret_ptr->subStructPointer.UnaryExp = unaryExp_ptr;
        ret_ptr->value = unaryExp_ptr->value;
    }
    else if (curToken.rule[0] == "MulExp")
    {
        MulExp_Struct *mulExp_ptr = MulExp_func();
        RESERVED_func(); // "*" | "/" | "%"
        UnaryExp_Struct *unaryExp_ptr = UnaryExp_func();

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
    ENTRY_GRAMMER(SysyFrontend::AddExp_Struct)

    // AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
    if (curToken.rule[0] == "MulExp")
    {
        MulExp_Struct *mulExp_ptr = MulExp_func();

        ret_ptr->type = AddExp_Struct::AddExpType::AddExpType_MulExp;
        ret_ptr->subStructPointer.MulExp = mulExp_ptr;
        ret_ptr->value = mulExp_ptr->value;
    }
    else if (curToken.rule[0] == "AddExp")
    {
        AddExp_Struct *addExp_ptr = AddExp_func();
        RESERVED_func(); // "+" | "-"
        MulExp_Struct *mulExp_ptr = MulExp_func();

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
    ENTRY_GRAMMER(SysyFrontend::RelExp_Struct)

    // RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
    if (curToken.rule[0] == "AddExp")
    {
        ret_ptr->type = RelExp_Struct::RelExpType::RelExpType_AddExp;
        ret_ptr->subStructPointer.AddExp = AddExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.AddExp->value;
    }
    else if (curToken.rule[0] == "RelExp")
    {
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
    ENTRY_GRAMMER(SysyFrontend::EqExp_Struct)

    // EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
    if (curToken.rule[0] == "RelExp")
    {
        ret_ptr->type = EqExp_Struct::EqExpType::EqExpType_RelExp;
        ret_ptr->subStructPointer.RelExp = RelExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.RelExp->value;
    }
    else if (curToken.rule[0] == "EqExp")
    {
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
    ENTRY_GRAMMER(SysyFrontend::LAndExp_Struct)

    // LAndExp     ::= EqExp | LAndExp "&&" EqExp;
    if (curToken.rule[0] == "EqExp")
    {
        ret_ptr->type = LAndExp_Struct::LAndExpType::LAndExpType_EqExp;
        ret_ptr->subStructPointer.EqExp = EqExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.EqExp->value;
    }
    else if (curToken.rule[0] == "LAndExp")
    {
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
    ENTRY_GRAMMER(SysyFrontend::LOrExp_Struct)

    // LOrExp      ::= LAndExp | LOrExp "||" LAndExp;
    if (curToken.rule[0] == "LAndExp")
    {
        ret_ptr->type = LOrExp_Struct::LOrExpType::LOrExpType_LAndExp;
        ret_ptr->subStructPointer.LAndExp = LAndExp_func();
        ret_ptr->value = ret_ptr->subStructPointer.LAndExp->value;
    }
    else if (curToken.rule[0] == "LOrExp")
    {
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