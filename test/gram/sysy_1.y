CompUnits      ::= CompUnit {CompUnit};
CompUnit       ::= FuncDef;

FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncType  ::= "int";

Block     ::= "{" Stmt "}";
Stmt      ::= "return" Number ";";
Number    ::= Integer;
Integer   ::= OCT_INTEGER;