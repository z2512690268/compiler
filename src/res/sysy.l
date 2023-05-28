#空白
WhiteSpace:( |\t|\n|\r)( |\t|\n|\r)*
#注释
LineComment:(//.*)|(/\*(((((\x20-\x29)|(\x2B-\x7E)).)|(\*((\x20-\x2E)|(\x30-\x7E))))|\n)*\*/)
#语法字符串
RESERVED:(const)|(int)|(void)|(if)|(while)|(break)|(continue)|(return)|(else)|(\+|\-|=|!|\*|/|%|>|<|<=|>=|==|!=|&&|\|\|)|(\{|\}|\[|\]|\(|\))|(,|;)
#const , ; int [ ] = { } ( ) void if while break continue return "+" "-" "!" "*" "/" "%" ">" "<" "<=" ">=" "==" "!=" "&&" "||"
#标识符
IDENT:((a-z)|(A-Z)|_)(\w)*
#整数字面量(十，八，十六进制)
DEC_INTEGER:(1-9)(0-9)*
OCT_INTEGER:0(0-7)*
HEX_INTEGER:0(x|X)(0-9|a-f|A-F)(0-9|a-f|A-F)*