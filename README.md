# README

一个编译器项目

## 环境配置

本项目环境基于北大编译原理docker镜像

```
docker pull maxxing/compiler-dev
```

额外安装xmake进行项目管理

```
wget https://xmake.io/shget.text -O - | bash
```

## 目前进度

- 词法分析(lex) 完成

## 约定

### 词法分析规则文件

- 每行均以`<token>:<regex>`的形式编写，其中token为待匹配的词法名，regex为符合下列要求的正则表达式。

- 在每行开头使用'#'标识表示注释，处理规则文件时将忽略该行；空格和tab等同于普通字符处理

- 支持concat(字符直接相连), |(析取运算), *(重复0次或多次)运算符

- `|, *, (, ), ., -, \`被用作正则表达式运算符，若要表示真实的字符，需要加上`\`前缀，如`\| \* \( \) \. \- \\`

- -运算符用于缩短表达式，当且仅当符号两侧为大写、小写或数字中的同类别字符时解析通过，且自动展开为两符号间所有的字符之或

- 支持通配，\w自动展开为所有数字、字母、下划线字符的或; \d自动展开为所有数字字符的或; .自动展开为所有可打印字符的或

- 支持`\n \t \r`转义字符, 分别表示换行、Tab、回车

- 运算优先级`转义/通配 > - > * > concat > |`

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

### 词法分析输出文件

- 每行一个token, 同时每个token以`<token> <match>`的形式展示，其中token为匹配到的token名，match为与其匹配的字符串(前后加上双引号)

- 词法分析过程将匹配最长的语法规则，若有多项语法规则同样长，则选择规则文件中先定义的规则

- 对于未匹配到词法分析规则的输入，将以`'单个字符'`作为token输出token, 其匹配的字符串为该单字符，该种形式的token应在后续操作中与预定义规则同等对待。

example input:

```
abacbdaaddbbbc1231ab12315ac35135
```

example output:

```
rule1 ab
rule2 ac
rule3 bd
'a' a
'a' a
'd' d
'd' d
'b' b
'b' b
rule3 bc
digit 1231
rule1 ab
digit 12315
rule2 ac
digit 351355
```

### IR约定



## 使用方法

目前前期开发阶段，编译器的不同阶段均分别以不同可执行文件进行，统一使用`xmake build`进行编译（基于clang工具链）。

### lex

lex 程序的执行方式如下：

```
xmake run lex <rule-file> <input-file> <output-file>
```

其中rule-file为定义的词法分析规则，input-file为待处理的字符流文件，output-file为处理好的token的输出文件, 各文件均以xmake.lua所在的项目根目录为基础计算相对路径。

example:

```
xmake run lex test/lex/rule1.txt test/lex/input.txt test/lex/output1.txt
xmake run lex test/lex/sysy.l test/lex/sysy.input test/lex/sysy.output
```

## 单元测试

项目采用gtest单元测试，通过`xmake build test`编译测试, 具体单元测试参数和帮助可以通过`xmake run test /?`查询