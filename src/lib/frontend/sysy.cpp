#include "frontend.h"
#include "koopa.h"

// 建树与解析函数
// CompUnits      ::= CompUnit {CompUnit};
SysyFrontend::CompUnits_Struct* SysyFrontend::CompUnits_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::CompUnits_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(curToken.rule[0] == "CompUnit") {
        for(auto rule : curToken.rule) {
            if(rule == "CompUnit") {
                ret_ptr->CompUnit.push_back(CompUnit_func());
            }
        }
        return ret_ptr;
    } else {
        std::cerr << "CompUnits_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    return ret_ptr;
}

// CompUnit       ::= FuncDef;
SysyFrontend::CompUnit_Struct* SysyFrontend::CompUnit_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::CompUnit_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(curToken.rule[0] == "FuncDef") {
        ret_ptr->FuncDef = FuncDef_func();
        return ret_ptr;
    } else {
        std::cerr << "CompUnit_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------

    return ret_ptr; 
}


// FuncDef   ::= FuncType IDENT "(" ")" Block;
SysyFrontend::FuncDef_Struct* SysyFrontend::FuncDef_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::FuncDef_Struct);
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    // 创建函数
    // FuncDef   ::= FuncType IDENT "(" ")" Block;
    if(curToken.rule[0] == "FuncType") {
        //-----------------------------------------------------------------
        // 创建作用域
        Scope* scope = koopaIR->AddNewScope();
        //-----------------------------------------------------------------
    
        //-----------------------------------------------------------------
        ret_ptr->funcRetType = FuncType_func();
        ret_ptr->funcnameIDENT = IDENT_func();
        RESERVED_func();    //(
        RESERVED_func();    //)
        ret_ptr->Block = Block_func(ret_ptr->funcnameIDENT->identifer);
        //-----------------------------------------------------------------
    
        //-----------------------------------------------------------------
        // 退出作用域
        koopaIR->SetScopeFunction(ret_ptr->funcnameIDENT->identifer, 
                                    ret_ptr->funcRetType->type);
        koopaIR->ScopeAddBasicBlock(ret_ptr->Block->block);
        koopaIR->ReturnBackScope();
        //-----------------------------------------------------------------
        
        return ret_ptr;
    } else {
        std::cerr << "FuncDef_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------


    return ret_ptr;
}

// FuncType  ::= "int";
SysyFrontend::FuncType_Struct* SysyFrontend::FuncType_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::FuncType_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // INT32
    // FuncType  ::= "int";
    if(curToken.rule[0] == "\"int\"") {
        ret_ptr->type.topType = KoopaVarType::KOOPA_INT32;
        RESERVED_func();  // "int"
        return ret_ptr;
    } else {
        std::cerr << "FuncType_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    
    return ret_ptr;
}


// Block     ::= "{" Stmt "}";
SysyFrontend::Block_Struct* SysyFrontend::Block_func(std::string block_name) {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Block_Struct);
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // Block     ::= "{" Stmt "}";
    if(curToken.rule[0] == "\"{\"") {
        //-----------------------------------------------------------------
        // 创建基本块，并加入符号表,更新当前块
        BasicBlock* block = koopaIR->NewBasicBlockAndSetCur("%entry");
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 创建语句
        RESERVED_func();    //{
        Stmt_Struct* stmt_ptr = Stmt_func();
        RESERVED_func();    //}
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        // 属性赋值
        ret_ptr->block = block;
        //-----------------------------------------------------------------
        return ret_ptr;
    } else {
        std::cerr << "Block_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    
    return ret_ptr;
}

// Stmt      ::= "return" Number ";";
SysyFrontend::Stmt_Struct* SysyFrontend::Stmt_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Stmt_Struct);
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    // Stmt      ::= "return" Number ";";
    if(curToken.rule[0] == "\"return\"") {
        //-----------------------------------------------------------------
        RESERVED_func();    //return
        ret_ptr->Number = Number_func();
        RESERVED_func();    //;
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        // 添加变量
        KoopaVar ret_var = koopaIR->NewVar(KoopaVarType::KOOPA_INT32, "%return");
        // 赋值语句
        koopaIR->AddOperationStatement("add", 0, ret_ptr->Number->value, ret_var);

        // 返回语句
        koopaIR->AddReturnStatement(ret_var);
        //-----------------------------------------------------------------
        return ret_ptr;
    } else {
        std::cerr << "Stmt_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------

    return ret_ptr;
}

// Number    ::= Integer;
SysyFrontend::Number_Struct* SysyFrontend::Number_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Number_Struct)
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    // Number    ::= Integer;
    if(curToken.rule[0] == "Integer") {
        //-----------------------------------------------------------------
        Integer_Struct* integer_ptr = Integer_func();
        //-----------------------------------------------------------------

        //-----------------------------------------------------------------
        ret_ptr->Integer = integer_ptr;
        ret_ptr->value.SetImm(integer_ptr->value);
        //-----------------------------------------------------------------
    } else {
        std::cerr << "Number_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    return ret_ptr;
}

// Integer   ::= OCT_INTEGER;
SysyFrontend::Integer_Struct* SysyFrontend::Integer_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::Integer_Struct);
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    if(curToken.rule[0] == "OCT_INTEGER") {
        //-----------------------------------------------------------------
        OCT_INTEGER_Struct* octInteger_ptr = OCT_INTEGER_func();
        
        //-----------------------------------------------------------------
        ret_ptr->OCT_INTEGER = octInteger_ptr;
        ret_ptr->value = octInteger_ptr->value_int;
        //-----------------------------------------------------------------
        return ret_ptr;
    } else {
        std::cerr << "Integer_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    return ret_ptr;
}

SysyFrontend::OCT_INTEGER_Struct* SysyFrontend::OCT_INTEGER_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::OCT_INTEGER_Struct)
    //-----------------------------------------------------------------
    if(curToken.token == "OCT_INTEGER") {
        //-----------------------------------------------------------------
        ret_ptr->value = curToken.rule[0];
        ret_ptr->value_int = std::stoi(
            curToken.rule[0].substr(1, curToken.rule[0].size() - 2), nullptr, 8);
        //-----------------------------------------------------------------
        return ret_ptr;
    } else {
        std::cerr << "OCT_INTEGER_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------
    return ret_ptr;
}

SysyFrontend::IDENT_Struct* SysyFrontend::IDENT_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::IDENT_Struct);
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    if(curToken.token == "IDENT") {
        //-----------------------------------------------------------------
        ret_ptr->identifer = "@" + curToken.rule[0].substr(1, curToken.rule[0].size() - 2);
        //-----------------------------------------------------------------
        return ret_ptr;
    } else {
        std::cerr << "IDENT_func: " << curToken << std::endl;
        exit(1);
    }
    //-----------------------------------------------------------------

    return ret_ptr;
}

SysyFrontend::RESERVED_Struct* SysyFrontend::RESERVED_func() {
    //-----------------------------------------------------------------
    ENTRY_GRAMMER(SysyFrontend::RESERVED_Struct);
    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    ret_ptr->reserved = curToken.rule[0];
    //-----------------------------------------------------------------
    return ret_ptr;
}
//******************************************************************************
