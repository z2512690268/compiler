Program ::= Lines;
Lines ::= Line NewLines {Line NewLines};
NewLines ::= NEWLINE {NEWLINE};
Line ::= LABEL Instruction
        |  LABEL { Directive }
        |  Instruction
        |  Directive { Directive };
Instruction ::= OPERATOR [OPERAND] {"," OPERAND};
Directive ::= PSEUDO {OPERAND};