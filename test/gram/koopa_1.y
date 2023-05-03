// CompUnit
CompUnits ::= CompUnit {CompUnit};

CompUnit  ::= FunDef;

FunDef ::= "fun" SYMBOL "(" ")" [":" Type] "{" {Block} "}";

Type ::= "i32";

Block ::= SYMBOL ":" EndStatement ";";

EndStatement ::= Return;

Return ::= "ret" [Value];