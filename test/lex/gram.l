#空白
WHITESPACE:( |\t|\n|\r)( |\t|\n|\r)*
#注释
LineComment:\\\\.*
#推导符
INFER:::=
#字符串常量,用双引号括起来的不包括双引号的任意字符串
STRING:"( |\t|\n|\r|!|#-~)*"
#终结符
TERMINAL:(A-Z|_)(A-Z|_)*
#非终结符
NONTERMINAL:(A-Z|a-z|_)(A-Z|a-z|0-9|_)*