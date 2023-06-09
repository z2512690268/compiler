#include "frontend.h"
#include "koopa.h"
#include "transfer.h"

bool isEndStmt(Statement *stmt)
{
    bool res = false;
    if (stmt->type == Statement::StatementType::JUMP)
    {
        res = true;
    }
    else if (stmt->type == Statement::StatementType::BRANCH)
    {
        res = true;
    }
    else if (stmt->type == Statement::StatementType::RETURN)
    {
        res = true;
    }
    std::cout << "isEndStmt: " << res << std::endl;
    return res;
}

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

// CompUnit       ::= Decl | FuncDef;
SysyFrontend::CompUnit_Struct *SysyFrontend::CompUnit_func()
{
    ENTRY_GRAMMER(SysyFrontend::CompUnit_Struct);

    if (curToken.rule[0] == "FuncDef")
    {
        ret_ptr->compUnitType = SysyFrontend::CompUnit_Struct::CompUnitType::CompUnitType_FuncDef;
        ret_ptr->subStructPointer.FuncDef = FuncDef_func();
    }
    else if (curToken.rule[0] == "Decl")
    {
        ret_ptr->compUnitType = SysyFrontend::CompUnit_Struct::CompUnitType::CompUnitType_Decl;
        ret_ptr->subStructPointer.Decl = Decl_func();
    }
    else
    {
        std::cerr << "CompUnit_func: " << curToken << std::endl;
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
        ret_ptr->subStructPointer.Stmt = Stmt_func("");
    }
    else
    {
        std::cerr << "BlockItem_func: " << curToken << std::endl;
        exit(1);
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// Stmt          ::= "if" "(" Exp ")" ElseStmt "else" Stmt
//                 | "if" "(" Exp ")" Stmt
//                 | "while" "(" Exp ")" Stmt
//                 | NoIfStmt;
SysyFrontend::Stmt_Struct *SysyFrontend::Stmt_func(std::string end_label)
{
    ENTRY_GRAMMER(SysyFrontend::Stmt_Struct);

    if (curToken.rule[0] == "\"if\"")
    {
        RESERVED_func(); // if
        RESERVED_func(); // (
        Exp_Struct *condition = Exp_func();
        RESERVED_func(); // )
        if (curToken.rule[4] == "ElseStmt")
        {
            ret_ptr->type = Stmt_Struct::StmtType::StmtType_IfElse;
            std::string if_block_name = koopaIR->GetUniqueName("%ifelse_if");
            std::string else_block_name = koopaIR->GetUniqueName("%ifelse_else");
            BasicBlock *else_block = koopaIR->NewBasicBlock(else_block_name);
            std::string end_block_name = koopaIR->GetUniqueName("%ifelse_end");
            BasicBlock *end_block = koopaIR->NewBasicBlock(end_block_name);

            koopaIR->AddBranchStatement(condition->value, if_block_name, else_block_name);
            bool to_end = false;

            // if
            BasicBlock *if_block = koopaIR->NewBasicBlockAndSetCur(if_block_name);
            koopaIR->ScopeAddBasicBlock(if_block);
            PushNameMap();
            ElseStmt_Struct *if_clause = ElseStmt_func(end_block_name);
            PopNameMap();
            if (koopaIR->curBlock->statements.back()->type == Statement::StatementType::JUMP && koopaIR->curBlock->statements.back()->jumpStmt.label == end_block_name)
            {
                to_end = true;
            }

            RESERVED_func(); // "else"

            // else
            koopaIR->SetCurBlock(else_block);
            koopaIR->ScopeAddBasicBlock(else_block);
            PushNameMap();
            Stmt_Struct *else_clause = Stmt_func(end_block_name);
            PopNameMap();
            if (koopaIR->curBlock->statements.back()->type == Statement::StatementType::JUMP && koopaIR->curBlock->statements.back()->jumpStmt.label == end_block_name)
            {
                to_end = true;
            }

            // end
            if (to_end)
            {
                koopaIR->SetCurBlock(end_block);
                koopaIR->ScopeAddBasicBlock(end_block);
            }

            ret_ptr->subStructPointer.IfElse.Condition = condition;
            ret_ptr->subStructPointer.IfElse.IfClause = if_clause;
            ret_ptr->subStructPointer.IfElse.ElseClause = else_clause;
        }
        else
        {
            ret_ptr->type = Stmt_Struct::StmtType::StmtType_If;
            std::string if_block_name = koopaIR->GetUniqueName("%if_if");
            std::string end_block_name = koopaIR->GetUniqueName("%if_end");
            BasicBlock *end_block = koopaIR->NewBasicBlock(end_block_name);

            koopaIR->AddBranchStatement(condition->value, if_block_name, end_block_name);

            // if
            BasicBlock *if_block = koopaIR->NewBasicBlockAndSetCur(if_block_name);
            koopaIR->ScopeAddBasicBlock(if_block);
            PushNameMap();
            Stmt_Struct *clause = Stmt_func(end_block_name);
            PopNameMap();

            koopaIR->SetCurBlock(end_block);
            koopaIR->ScopeAddBasicBlock(end_block);

            ret_ptr->subStructPointer.If.Condition = condition;
            ret_ptr->subStructPointer.If.Clause = clause;
        }
    }
    else if (curToken.rule[0] == "\"while\"")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_While;
        RESERVED_func(); // while
        RESERVED_func(); // (

        std::string while_entry_name = koopaIR->GetUniqueName("%while_entry");
        std::string while_body_name = koopaIR->GetUniqueName("%while_body");
        std::string end_block_name = koopaIR->GetUniqueName("%while_end");
        BasicBlock *end_block = koopaIR->NewBasicBlock(end_block_name);

        // while entry
        koopaIR->AddJumpStatement(while_entry_name);
        BasicBlock *while_entry = koopaIR->NewBasicBlockAndSetCur(while_entry_name);
        koopaIR->ScopeAddBasicBlock(while_entry);
        Exp_Struct *condition = Exp_func();
        koopaIR->AddBranchStatement(condition->value, while_body_name, end_block_name);
        RESERVED_func(); // )

        // while body
        BasicBlock *while_body = koopaIR->NewBasicBlockAndSetCur(while_body_name);
        koopaIR->ScopeAddBasicBlock(while_body);
        PushNameMap();
        PushWhile(while_entry_name, end_block_name);
        Stmt_Struct *clause = Stmt_func(while_entry_name);
        PopWhile();
        PopNameMap();

        // end
        koopaIR->SetCurBlock(end_block);
        koopaIR->ScopeAddBasicBlock(end_block);
    }
    else if (curToken.rule[0] == "NoIfStmt")
    {
        ret_ptr->type = Stmt_Struct::StmtType::StmtType_NoIf;
        ret_ptr->subStructPointer.NoIfStmt = NoIfStmt_func();
    }
    else
    {
        std::cerr << "Stmt_func: " << curToken << std::endl;
        exit(1);
    }

    if (end_label != "")
    {
        if (koopaIR->curBlock->statements.size() == 0 || !isEndStmt(koopaIR->curBlock->statements.back()))
        {
            std::cout << "add jump to end_label: " << end_label << std::endl;
            koopaIR->AddJumpStatement(end_label);
        }
    }
    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// ElseStmt    ::= "if" "(" Exp ")" ElseStmt "else" ElseStmt
//                 | "while" "(" Exp ")" ElseStmt
//                 | NoIfStmt;
SysyFrontend::ElseStmt_Struct *SysyFrontend::ElseStmt_func(std::string end_label)
{
    ENTRY_GRAMMER(SysyFrontend::ElseStmt_Struct);

    if (curToken.rule[0] == "\"if\"")
    {
        ret_ptr->type = ElseStmt_Struct::ElseStmtType::ElseStmtType_IfElse;
        RESERVED_func(); // if
        RESERVED_func(); // (
        Exp_Struct *condition = Exp_func();
        RESERVED_func(); // )
        std::string if_block_name = koopaIR->GetUniqueName("%ifelse_if");
        std::string else_block_name = koopaIR->GetUniqueName("%ifelse_else");
        BasicBlock *else_block = koopaIR->NewBasicBlock(else_block_name);
        std::string end_block_name = koopaIR->GetUniqueName("%ifelse_end");
        BasicBlock *end_block = koopaIR->NewBasicBlock(end_block_name);

        koopaIR->AddBranchStatement(condition->value, if_block_name, else_block_name);
        bool to_end = false;

        // if
        BasicBlock *if_block = koopaIR->NewBasicBlockAndSetCur(if_block_name);
        koopaIR->ScopeAddBasicBlock(if_block);
        PushNameMap();
        ElseStmt_Struct *if_clause = ElseStmt_func(end_block_name);
        PopNameMap();
        if (koopaIR->curBlock->statements.back()->type == Statement::StatementType::JUMP && koopaIR->curBlock->statements.back()->jumpStmt.label == end_block_name)
        {
            to_end = true;
        }

        RESERVED_func(); // "else"

        // else
        koopaIR->SetCurBlock(else_block);
        koopaIR->ScopeAddBasicBlock(else_block);
        PushNameMap();
        ElseStmt_Struct *else_clause = ElseStmt_func(end_block_name);
        PopNameMap();
        if (koopaIR->curBlock->statements.back()->type == Statement::StatementType::JUMP && koopaIR->curBlock->statements.back()->jumpStmt.label == end_block_name)
        {
            to_end = true;
        }

        // end
        if (to_end)
        {
            koopaIR->SetCurBlock(end_block);
            koopaIR->ScopeAddBasicBlock(end_block);
        }

        ret_ptr->subStructPointer.IfElse.Condition = condition;
        ret_ptr->subStructPointer.IfElse.IfClause = if_clause;
        ret_ptr->subStructPointer.IfElse.ElseClause = else_clause;
    }
    else if (curToken.rule[0] == "\"while\"")
    {
        ret_ptr->type = ElseStmt_Struct::ElseStmtType::ElseStmtType_While;
        RESERVED_func(); // while
        RESERVED_func(); // (
        Exp_Struct *condition = Exp_func();
        RESERVED_func(); // )
        std::string while_entry_name = koopaIR->GetUniqueName("%while_entry");
        std::string while_body_name = koopaIR->GetUniqueName("%while_body");
        std::string end_block_name = koopaIR->GetUniqueName("%while_end");
        BasicBlock *end_block = koopaIR->NewBasicBlock(end_block_name);

        // while entry
        koopaIR->AddJumpStatement(while_entry_name);
        BasicBlock *while_entry = koopaIR->NewBasicBlockAndSetCur(while_entry_name);
        koopaIR->ScopeAddBasicBlock(while_entry);
        koopaIR->AddBranchStatement(condition->value, while_body_name, end_block_name);

        // while body
        BasicBlock *while_body = koopaIR->NewBasicBlockAndSetCur(while_body_name);
        koopaIR->ScopeAddBasicBlock(while_body);
        PushNameMap();
        PushWhile(while_entry_name, end_block_name);
        ElseStmt_Struct *clause = ElseStmt_func(while_entry_name);
        PopWhile();
        PopNameMap();

        // end
        koopaIR->SetCurBlock(end_block);
        koopaIR->ScopeAddBasicBlock(end_block);

        ret_ptr->subStructPointer.While.Condition = condition;
        ret_ptr->subStructPointer.While.Clause = clause;
    }
    else if (curToken.rule[0] == "NoIfStmt")
    {
        ret_ptr->type = ElseStmt_Struct::ElseStmtType::ElseStmtType_NoIf;
        ret_ptr->subStructPointer.NoIfStmt = NoIfStmt_func();
    }
    else
    {
        std::cerr << "ElseStmt_func: " << curToken << std::endl;
        exit(1);
    }

    if (end_label != "")
    {
        if (koopaIR->curBlock->statements.size() == 0 || !isEndStmt(koopaIR->curBlock->statements.back()))
        {
            std::cout << "add jump to end_label: " << end_label << std::endl;
            koopaIR->AddJumpStatement(end_label);
        }
    }

    std::cout << "Exit -- " << curToken.token << std::endl;
    return ret_ptr;
}

// NoIfStmt    ::= LVal "=" Exp ";"
//                 | [Exp] ";"
//                 | Block
//                 | "break" ";"
//                 | "continue" ";"
//                 | "return" [Exp] ";";
SysyFrontend::NoIfStmt_Struct *SysyFrontend::NoIfStmt_func()
{
    ENTRY_GRAMMER(SysyFrontend::NoIfStmt_Struct);

    if (curToken.rule[0] == "\"return\"")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Return;
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
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Exp;
        ret_ptr->subStructPointer.Return = Exp_func();
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "\";\"")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Exp;
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "Block")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Block;
        std::string inner_block = koopaIR->GetUniqueName("%entry");
        koopaIR->AddJumpStatement(inner_block);
        ret_ptr->subStructPointer.Block = Block_func(inner_block);

        if (koopaIR->curBlock->statements.size() == 0 || !isEndStmt(koopaIR->curBlock->statements.back()))
        {
            std::string outer_block = koopaIR->GetUniqueName("%entry");
            koopaIR->AddJumpStatement(outer_block);

            BasicBlock *block = koopaIR->NewBasicBlockAndSetCur(outer_block);
            koopaIR->ScopeAddBasicBlock(block);
        }
    }
    else if (curToken.rule[0] == "LVal")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Assign;
        ret_ptr->subStructPointer.Assign.LVal = LVal_func();
        if (IsConst(ret_ptr->subStructPointer.Assign.LVal->ident))
        {
            std::cerr << "Assign to const variable: " << ret_ptr->subStructPointer.Assign.LVal->ident << std::endl;
            exit(1);
        }
        RESERVED_func(); // =
        if (IsParam(ret_ptr->subStructPointer.Assign.LVal->ident))
        {
            if (ret_ptr->subStructPointer.Assign.LVal->index.size() > 0)
            {
                // 获取值
                ret_ptr->subStructPointer.Assign.Exp = Exp_func();

                // 值存入指针
                KoopaVar param = koopaIR->GetVar(GetIRName(ret_ptr->subStructPointer.Assign.LVal->ident));
                KoopaVar ptr = koopaIR->NewTempVar(param.type);
                koopaIR->AddAllocStatement(ptr);
                koopaIR->AddStoreStatement(ptr, param);
                KoopaVar source = koopaIR->NewTempVar(param.type);
                koopaIR->AddLoadStatement(ptr, source);
                KoopaVar dest = koopaIR->NewTempVar(*source.type.ptrType.type.get());
                koopaIR->AddGetptrStatement(source, ret_ptr->subStructPointer.Assign.LVal->index[0]->value, dest);
                if (ret_ptr->subStructPointer.Assign.LVal->index.size() > 1) {
                    for (int i = 1; i < ret_ptr->subStructPointer.Assign.LVal->index.size(); i++) {
                        source = dest;
                        dest = koopaIR->NewTempVar(*dest.type.arrayType.type.get());
                        koopaIR->AddGetelementptrStatement(source, ret_ptr->subStructPointer.Assign.LVal->index[i]->value, dest);
                    }
                }
                koopaIR->AddStoreStatement(dest, ret_ptr->subStructPointer.Assign.Exp->value);
            }
            else
            {
                if (IsAssigned(ret_ptr->subStructPointer.Assign.LVal->ident))
                {
                    KoopaVar receiver = koopaIR->GetVar(GetIRName(ret_ptr->subStructPointer.Assign.LVal->ident));
                    ret_ptr->subStructPointer.Assign.Exp = Exp_func(&receiver);
                }
                else
                {
                    KoopaVar old_var = koopaIR->GetVar(GetIRName(ret_ptr->subStructPointer.Assign.LVal->ident));
                    std::string temp_name = koopaIR->GetUniqueName(ret_ptr->subStructPointer.Assign.LVal->ident);
                    KoopaVar receiver = koopaIR->NewVar(old_var.type, temp_name);
                    ret_ptr->subStructPointer.Assign.Exp = Exp_func();
                    koopaIR->AddAllocStatement(receiver);
                    koopaIR->AddStoreStatement(receiver, ret_ptr->subStructPointer.Assign.Exp->value);
                    ChangeName(ret_ptr->subStructPointer.Assign.LVal->ident, temp_name);
                }
            }
        }
        else
        {
            if (ret_ptr->subStructPointer.Assign.LVal->index.size() > 0)
            {
                LVal_Struct *lval = ret_ptr->subStructPointer.Assign.LVal;
                KoopaVar dest = koopaIR->GetVar(GetIRName(lval->ident));
                KoopaVarType type = dest.type;
                for (int i = 0; i < lval->index.size(); i++)
                {
                    KoopaVar source = dest;
                    dest = koopaIR->NewTempVar(type);
                    type = *type.arrayType.type.get();
                    koopaIR->AddGetelementptrStatement(source, lval->index[i]->value, dest);
                }
                ret_ptr->subStructPointer.Assign.Exp = Exp_func(&dest);
            }
            else
            {
                KoopaVar receiver = koopaIR->GetVar(GetIRName(ret_ptr->subStructPointer.Assign.LVal->ident));
                ret_ptr->subStructPointer.Assign.Exp = Exp_func(&receiver);
            }
        }
        RESERVED_func(); //;
    }
    else if (curToken.rule[0] == "\"break\"")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Break;
        RESERVED_func(); // break
        RESERVED_func(); // ;
        koopaIR->AddJumpStatement(while_struct->end_block_name);
    }
    else if (curToken.rule[0] == "\"continue\"")
    {
        ret_ptr->type = NoIfStmt_Struct::NoIfStmtType::NoIfStmtType_Continue;
        RESERVED_func(); // continue
        RESERVED_func(); // ;
        koopaIR->AddJumpStatement(while_struct->while_entry_name);
    }
    else
    {
        std::cerr << "NoIfStmt_func: " << curToken << std::endl;
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