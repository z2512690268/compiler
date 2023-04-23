oS ::= "a"
    | "(" oT ")";
oT ::= oT "," oS
    | oS;