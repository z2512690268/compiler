#TAB
TAB:(\t)
#空白
WhiteSpace:( |\n|\r)( |\n|\r)*
#注释
LineComment://.*
#标识符
SYMBOL:(@|%)((a-z)|(A-Z)|_)(\w)*
#整数
INT:(0|(1-9)(0-9)*)|(\-(0|(1-9)(0-9)*))
#二元运算符
BINARY_OP:(ne)|(eq)|(gt)|(lt)|(ge)|(le)|(add)|(sub)|(mul)|(div)|(mod)|(and)|(or)|(xor)|(shl)|(shr)|(sar)
#保留串
RESERVED:(i32)|(undef)|(zeroinit)|(global)|(alloc)|(load)|(store)|(getptr)|(getelemptr)|(jump)|(br)|(call)|(ret)|(fun)|(decl)|(\[|\]|,|\*|\(|\)|:|\{|\}|=|;)