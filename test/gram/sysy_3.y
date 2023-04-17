CompUnit  ::= FuncDef;

FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncType  ::= "int";

Block     ::= "{" Stmt "}";

Stmt        ::= "return" Exp ";";

Exp         ::= AddExp;
PrimaryExp  ::= "(" Exp ")" | Number;
Number      ::= Integer;
UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
UnaryOp     ::= "+" | "-" | "!";
Integer     ::= DEC_INTEGER | HEX_INTEGER | OCT_INTEGER;
MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;