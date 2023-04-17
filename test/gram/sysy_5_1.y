CompUnit  ::= FuncDef;

FuncDef   ::= "int" IDENT "(" ")" Block;

Exp      ::= DEC_INTEGER | Exp "+" DEC_INTEGER;


Decl          ::= "const" "int" IDENT "=" Exp {"," ConstDef} ";";

Block         ::= "{" Decl "}";