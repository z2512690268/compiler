// CompUnit
CompUnits ::= CompUnit {CompUnit};

CompUnit  ::= GlobalSymbolDef | FunDecl | FunDef;

// Type
Type ::= "i32" | ArrayType | PointerType | FunType;
ArrayType ::= "[" Type "," INT "]";
PointerType ::= "*" Type;
FunType ::= "(" [Type {"," Type}] ")" [":" Type];

// Value
Value ::= SYMBOL | INT | "undef";
InitializerBlock ::= "{" Initializer "}";
Initializer      ::= INT | "undef" | Aggregate | "zeroinit";
Aggregate ::= "{" Initializer {"," Initializer} "}";

//Symbol
SymbolDef ::= SYMBOL "=" (MemoryDeclaration | Load | GetPointer | GetElementPointer | BinaryExpr | FunCall);
GlobalSymbolDef ::= "global" SYMBOL "=" GlobalMemoryDeclaration;

// Memory
MemoryDeclaration ::= "alloc" Type;
GlobalMemoryDeclaration ::= "alloc" Type "," InitializerBlock;

// Load
Load ::= "load" SYMBOL;
Store ::= "store" (Value | InitializerBlock) "," SYMBOL;

// GetPointer
GetPointer ::= "getptr" SYMBOL "," Value;
GetElementPointer ::= "getelemptr" SYMBOL "," Value;    //2

// BinaryExpr
BinaryExpr ::= BINARY_OP Value "," Value;

//Branch
Branch ::= "br" Value "," SYMBOL [BlockArgList] "," SYMBOL [BlockArgList];
Jump ::= "jump" SYMBOL [BlockArgList];
BlockArgList ::= "(" Value {"," Value} ")";

//Function Call
FunCall ::= "call" SYMBOL "(" [Value {"," Value}] ")";
Return ::= "ret" [Value];

//Function Body
FunDef ::= "fun" SYMBOL "(" [FunParams] ")" [":" Type] "{" {Block} "}";
FunParams ::= SYMBOL ":" Type {"," SYMBOL ":" Type};
Block ::= SYMBOL [BlockParamList] ":" {Statement ";"} EndStatement ";";
BlockParamList ::= "(" SYMBOL ":" Type {"," SYMBOL ":" Type} ")";
Statement ::= SymbolDef | Store | FunCall;
EndStatement ::= Branch | Jump | Return;

//Function Decl
FunDecl ::= "decl" SYMBOL "(" [FunDeclParams] ")" [":" Type];
FunDeclParams ::= Type {"," Type};

