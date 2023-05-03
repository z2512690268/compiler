CompUnits      ::= CompUnit {CompUnit};
CompUnit       ::= Decl | FuncDef;

Decl          ::= ConstDecl | VarDecl;
ConstDecl     ::= "const" Type ConstDef {"," ConstDef} ";";
ConstDef      ::= IDENT "=" ConstInitVal;
ConstInitVal  ::= ConstExp;
VarDecl       ::= Type VarDef {"," VarDef} ";";
VarDef        ::= IDENT | IDENT "=" InitVal;
InitVal       ::= Exp;

FuncDef       ::= Type IDENT "(" [FuncFParams] ")" Block;
Type          ::= "void" | "int";
FuncFParams   ::= FuncFParam {"," FuncFParam};
FuncFParam    ::= Type IDENT;

Block         ::= "{" {BlockItem} "}";
BlockItem     ::= Decl | Stmt;
Stmt          ::= "if" "(" Exp ")" ElseStmt "else" Stmt
                | "if" "(" Exp ")" Stmt
                | "while" "(" Exp ")" Stmt
                | NoIfStmt;
                
ElseStmt    ::= "if" "(" Exp ")" ElseStmt "else" ElseStmt
                | "while" "(" Exp ")" ElseStmt
                | NoIfStmt;

NoIfStmt    ::= LVal "=" Exp ";"
                | [Exp] ";"
                | Block
                | "break" ";"
                | "continue" ";"
                | "return" [Exp] ";";

Exp           ::= LOrExp;
LVal          ::= IDENT;
PrimaryExp    ::= "(" Exp ")" | LVal | Number;
Number        ::= Integer;
UnaryExp      ::= PrimaryExp | IDENT "(" [FuncRParams] ")" | UnaryOp UnaryExp;
UnaryOp       ::= "+" | "-" | "!";
FuncRParams   ::= Exp {"," Exp};
MulExp        ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp        ::= MulExp | AddExp ("+" | "-") MulExp;
RelExp        ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
EqExp         ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp       ::= EqExp | LAndExp "&&" EqExp;
LOrExp        ::= LAndExp | LOrExp "||" LAndExp;
ConstExp      ::= Exp;

Integer       ::= DEC_INTEGER | HEX_INTEGER | OCT_INTEGER;