#空白
WHITESPACE:( |\t|\r)( |\t|\r)*
#注释
LineComment:#.*
#换行
NEWLINE:\n|\r\n
#注释
LineComment://.*
#保留符
RESERVED:,
#标签
LABEL:(\w)(\w)*:
#操作符
OPERATOR:add|sub|xor|or|and|sll|srl|sra|slt|sltu|addi|xori|ori|andi|slli|srli|srai|slti|sltiu|lb|lh|lw|lbu|lhu|sb|sh|sw|beq|bne|blt|bge|bltu|bgeu|jal|jalr|lui|auipc|ecall|ebreak|la|li|beqz|j|ret
#操作对象
OPERAND:((\w)(\w)*)|((0-9)(0-9)*\((\w)(\w)*\))|(\-((\w)(\w)*))|(\-((0-9)(0-9)*\((\w)(\w)*\)))|(".*")
#非终结符
PSEUDO:\.(\w)(\w)*