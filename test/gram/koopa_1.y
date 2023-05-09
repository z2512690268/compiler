// CompUnit
CompUnits ::= CompUnit {CompUnit};

CompUnit  ::= FunDef;

FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";

Type ::= "i32";

Block ::= SYMBOL ":" {Statement ";"} EndStatement ";";

Statement ::= SymbolDef;

SymbolDef ::= SYMBOL "=" BinaryExpr;

BinaryExpr ::= BINARY_OP Value "," Value;

EndStatement ::= Return;

Return ::= "ret" [Value];

Value ::= SYMBOL | INT | "undef";