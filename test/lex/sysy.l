#空白
#WhiteSpace:( |\t|\n|\r)( |\t|\n|\r)*
#注释
LineComment:\\\\.*
#标识符
Identifier:((a-z)|(A-Z)|_)(\w)*
#整数字面量(十，八，十六进制)
Decimal:(1-9)(0-9)*
Octal:0(0-7)*
Hexadecimal:0(x|X)(0-9|a-f|A-F)(0-9|a-f|A-F)*