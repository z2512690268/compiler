# README

一个编译器项目

## 环境配置

本项目环境基于北大编译原理docker镜像，该环境可确保运行成功，其他环境编译器部分仍可通过编译(目前测试windows/msvc, linux/gcc, linux/clang工具链编译运行成功), 但请自行配置xmake

原教程网站：https://pku-minic.github.io/online-doc/#/

```
docker pull maxxing/compiler-dev
```
 
额外安装xmake进行项目管理

```
wget https://xmake.io/shget.text -O - | bash
```

在.bashrc里允许xmake在root下使用

```
echo "export XMAKE_ROOT=y" >> ~/.bashrc
```

这些操作已经封装在prepare.sh中，在compiler-dev镜像下可一键配置

运行测试部分目前使用qemu-riscv32-static和koopa, llc的工具链，在镜像中已经配置好，若要在其他环境下运行，请自行配置

## 目前进度

- 词法分析(lex) 完成

- 语法分析(gram) 完成

- 语义分析(syntax/frontend) working (sysy working, koopa working)

- 代码生成(backend) working (koopa基本完成, riscv working)

## 约定

### 词法分析规则文件

- 每行均以`<token>:<regex>`的形式编写，其中token为待匹配的词法名，regex为符合下列要求的正则表达式。

- 在每行开头使用'#'标识表示注释，处理规则文件时将忽略该行；空格和tab等同于普通字符处理

- 支持concat(字符直接相连), |(析取运算), *(重复0次或多次), -(展开)运算符，其他运算符可能暂时未支持功能

- ` (, ), *, +, ., ?, \\, {, }, |, -, [, ]`被用作正则表达式运算符，若要表示真实的字符，需要加上`\`前缀，如`\| \* \( \) \. \- \\`

- -运算符用于缩短表达式，自动展开为两符号间所有的字符之或

- 支持通配，\w自动展开为所有数字、字母、下划线字符的或; \d自动展开为所有数字字符的或; .自动展开为所有可打印字符的或

- 支持`\n \t \r`转义字符, 分别表示换行、Tab、回车

- 支持`\xXX`形式的十六进制转义字符，用以表示ASCII码为XX的字符，支持不可打印字符(比如说可以拿来匹配中文，中文一般有多个负数字节作为前缀，具体请查询Utf8编码或其他字符编码)

- 运算优先级`转义/通配 > -展开 > * > concat > |`

example 1:

```
#这里是注释
rule1:abcdefg
rule2:a|b|c|d|e|f|g
rule3:ab|cd|ef|gh
rule4:(a|b)(c|d)
#这里也是注释
rule5:a*
rule6:(ab|a*)*
rule7:(ab|a*)*b
rule8:a((a|b)*|a(c|d)b)*b
#这里还是注释
```

example 2
```
rule1:ab
rule2:ac
rule3:b(c|d)
digit:(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*
```

example 3(语法分析的词法分析文件)
```
#空白
WHITESPACE:( |\t|\n|\r)( |\t|\n|\r)*
#注释
LineComment://.*
#推导符
INFER:::=
#保留符
RESERVED:\[|\]|\{|\}|\(|\)|\||;
#字符串常量,用双引号括起来的不包括双引号的任意字符串
STRING:"( |\t|\n|\r|!|#-~)*"
#终结符
TERMINAL:(A-Z|_)(A-Z|_)*
#非终结符
NONTERMINAL:(A-Z|a-z|_)(A-Z|a-z|0-9|_)*
```

### 语法分析规则文件

- 词法分析规则采用EBNF形式，具体讲解可以参考[这里](https://pku-minic.github.io/online-doc/#/lv1-main/lexer-parser?id=%e7%9c%8b%e6%87%82-ebnf)

- 词法分析的第一条规则将被作为起始规则

- 词法分析中全大写的token和双引号括起来的字符串将被视为终结符，其他token将被视为非终结符

- 所有终结符token应当由上面的词法分析输出，并由本程序读入

example:

```
CompUnit  ::= FuncDef;

FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncType  ::= "int";

Block     ::= "{" Stmt "}";
Stmt      ::= "return" Number ";";
Number    ::= Integer;
Integer   ::= OCT_INTEGER;
```

### 词法分析输出文件

- 每行一个token, 同时每个token以`<token> <match>`的形式展示，其中token为匹配到的token名，match为与其匹配的字符串(前后加上双引号)

- 词法分析过程将匹配最长的语法规则，若有多项语法规则同样长，则选择规则文件中先定义的规则

- 对于未匹配到词法分析规则的输入，将以`'单个字符'`作为token输出token, 其匹配的字符串为该单字符，该种形式的token应在后续操作中与预定义规则同等对待。

- 对于match字符串中的空白字符或不可见字符，将以转义字符形式输出，如`\n \t \r \xXX(十六进制)`

- 为保证每行只有一个空格，match字符串中的空格将以`\s`形式输出

example 1 input:

```
abacbdaaddbbbc1231ab12315ac35135
```

example 1 output:

```
rule1 "ab"
rule2 "ac"
rule3 "bd"
'a' "a"
'a' "a"
'd' "d"
'd' "d"
'b' "b"
'b' "b"
rule3 "bc"
digit "1231"
rule1 "ab"
digit "12315"
rule2 "ac"
digit "35135"

```

example 2 input:

```
int main() {
  // I'm a comment
  return 0;
}
```

example 2 output:

```
RESERVED "int"
WhiteSpace "\s"
IDENT "main"
RESERVED "("
RESERVED ")"
WhiteSpace "\s"
RESERVED "{"
WhiteSpace "\n\s\s"
LineComment "//\sI'm\sa\scomment"
WhiteSpace "\n\s\s"
RESERVED "return"
WhiteSpace "\s"
OCT_INTEGER "0"
RESERVED ";"
WhiteSpace "\n"
RESERVED "}"
```

### 语法树输出文件

- 每行一个语法规则或终结符，语法规则形式为`<规则名> : <规则内容>`， 终结符形式为`<终结符token> = "<终结符match>"`，若终结符为RESERVED保留字，则token和match均为字符串形式

- 语法规则中的终结符均为全大写或者为字符串保留字，非终结符均为大小写混合。

- 语法规则中若出现$字符，说明为临时规则，用以处理原EBNF规则中0次或多次出现的情况，$以前的字符串为原始规则，可以对应参考处理

- 每条语法规则后的规则内容中的每一项对应一颗子树, 从而能够确定每个节点子树的数量; 整个文件以语法树的前序遍历形式给出, 由此能够唯一确定语法树的结构

- 隐藏的根节点$START未打印在语法树结构中，该语法规则定义未$START : <语法描述文件中第一条语法规则>, 因此除去该节点后语法树依然为单根树
```
CompUnit : FuncDef 
	FuncDef : FuncType IDENT "(" ")" Block 
		FuncType : "int" 
			"int" = "int"
		IDENT = "main"
		"(" = "("
		")" = ")"
		Block : "{" Stmt "}" 
			"{" = "{"
			Stmt : "return" Number ";" 
				"return" = "return"
				Number : Integer 
					Integer : OCT_INTEGER 
						OCT_INTEGER = "0"
				";" = ";"
			"}" = "}"

```

### IR约定

本项目IR基于北大Koopa IR, 原始规定文档链接为[koopa](https://pku-minic.github.io/online-doc/#/misc-app-ref/koopa), 但为了符合LR(1)语法分析的要求，对其进行了一定的修改, 为了最小化修改内容，且不影响koopa官方编译器使用，特定空白字符TAB(即'\t')为语法关键字，使其不可随意使用，具体修改如下

- 规则1：每一条空返回值的ret语句中，ret后面必须跟随且仅跟随一个TAB字符

- 规则2: 除了规则1规定的情形外，不得在任何其他地方使用TAB字符


修改示例：
```
//规则1修改示例
fun @main(): i32 {
%entry:
  ret
}
-->
fun @main(): i32 {
%entry:
  ret	//这里有一个tab
}
```

语法规则修改如下:
```
Return ::= "ret" [Value];
-->
Return ::= "ret" (Value | TAB);
```

### 汇编语言约定

汇编语言采用riscv汇编，具体规则参考[这里](https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf)

- 完整支持的指令集为RV32I，不支持RV32M和RV32F

- 支持的简化指令包括`la, li, ret, j, beqz`

- 支持的伪指令仅包括.text, .data, .global, .asciz

- 支持的寄存器包括`x0-x31, ra, sp, gp, tp, t0-t6, s0-s11, a0-a7`

- 所有的指令、伪指令和标签均为全小写, 伪指令以`.`开头, 标签以`:`结尾

示例：
```
  .text
  .globl main
main:
  addi  sp, sp, -16
  sw    ra, 12(sp)
  sw    s0, 8(sp)
  sw    s1, 4(sp)
  la    s0, hello_str
  li    s1, 0
1:
  add   a0, s0, s1
  lbu   a0, 0(a0)
  beqz  a0, 1f
  # call  putch
  addi  s1, s1, 1
  j     1b
2:
  li    a0, 0
  lw    s1, 4(sp)
  lw    s0, 8(sp)
  lw    ra, 12(sp)
  addi  sp, sp, 16
  ret

  .data
hello_str:
  .asciz "Hello, world!\n"
```

### 二进制文件约定

仅支持riscv 32位可执行文件

## 使用方法

目前前期开发阶段，编译器的不同阶段均分别以不同可执行文件进行，统一使用`xmake build`进行编译（基于clang工具链）。

src/res为实际使用的用于解析语法规则的词法分析文件，无需修改

测试阶段所有其他需要读入的源文件均位于`test/`目录下

### lex词法分析程序

lex 程序的执行方式如下：

```
xmake run lex <rule-file> <input-file> <output-file>
```

其中rule-file为定义的词法分析规则，input-file为待处理的字符流文件，output-file为处理好的token的输出文件

<rule_file>自动补全前缀为`test/lex/`, 后缀为`.l`, <input_file>自动补全前缀为`test/pipeline/`, 后缀为`.input`, <output_file>自动补全前缀为`test/pipeline/`,后缀为`.lex`

example:

```
xmake run lex rule1 rule1 rule11
xmake run lex sysy maze maze
```

### gram语法分析程序

gram 程序的执行方式如下：

```
xmake run gram <rule-file> <lex-file> <gram-file>
```

其中rule-file为定义的语法分析规则，input-file为待处理的lex token流文件(由lex程序输出)，output-file为语法树文件

<rule_file>自动补全前缀为`test/gram/`, 后缀为`.y`, <lex_file>自动补全前缀为`test/pipeline/`, 后缀为`.lex`, <gram_file>自动补全前缀为`test/pipeline/`,后缀为`.gram`

example:

```
xmake run gram sysy maze maze
xmake run gram sysy1 sysy_t1 sysy_t1
```

### 语义分析与代码生成器

syntax 程序的执行方式如下：

```
xmake run syntax <frontend-lang> <backend-lang> <filename-base>
```

其中frontend-lang为前端语言，backend-lang为后端语言，filename-base为文件名前缀

输入文件路径为`test/pipeline/<filename-base>.gram`, 输出文件路径为`test/pipeline/<filename-base>.<backend-lang>`

目前支持的前端语言包括`sysy, koopa`, 支持的后端语言包括`koopa, riscv`

example:
```
xmake run syntax sysy koopa sysy_t1
xmake run syntax sysy riscv sysy_t1
xmake run syntax koopa koopa koopa_t1
xmake run syntax koopa riscv koopa_t1
```

## 单元测试

项目采用gtest单元测试，通过`xmake build test`编译测试, 具体单元测试参数和帮助可以通过`xmake run test /?`查询

如需选择特定的测试: gtest提供以下参数：

```
  --gtest_list_tests
      List the names of all tests instead of running them. The name of
      TEST(Foo, Bar) is "Foo.Bar".
  --gtest_filter=POSITIVE_PATTERNS[-NEGATIVE_PATTERNS]
      Run only the tests whose name matches one of the positive patterns but
      none of the negative patterns. '?' matches any single character; '*'
      matches any substring; ':' separates two patterns.
```