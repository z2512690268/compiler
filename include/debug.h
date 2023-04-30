#pragma once
// lex
// 开关部分
// 打印预处理正则表达式
// #define DEBUG_LEX_NEWREGEX if(1)
// 打印NFA
// #define DEBUG_LEX_NFA if(1)
// 打印NFA转DFA过程
// #define DEBUG_ATM_NFA2DFA if(1)
// 打印DFA
// #define DEBUG_LEX_DFA if(1) 
// 打印匹配过程
// #define DEBUG_LEX_MATCH if(1)


// gram
// 打印解析语法文件得到的转移图
// #define DEBUG_GRAM_TOKENNODE if(1) 
// 打印由转移图得到的语法列表
#define DEBUG_GRAM_DEFTRACE if(1)
// 打印GRAM NFA
// #define DEBUG_GRAM_NFA if(1)
// 打印GRAM DFA
// #define DEBUG_GRAM_DFA if(1)
// 打印GRAM First集求解
// #define DEBUG_GRAM_FIRST if(1)
// 打印GRAM LR1 Table
#define DEBUG_GRAM_LR1TABLE if(1)
// 打印GRAM LR1 Analyze
#define DEBUG_GRAM_LR1ANALYZE if(1)




// 归零条件
// lex
#ifndef DEBUG_LEX_NEWREGEX
#define DEBUG_LEX_NEWREGEX if(0)
#endif
#ifndef DEBUG_LEX_DFA
#define DEBUG_LEX_DFA if(0)
#endif
#ifndef DEBUG_LEX_NFA
#define DEBUG_LEX_NFA if(0)
#endif
#ifndef DEBUG_ATM_NFA2DFA
#define DEBUG_ATM_NFA2DFA if(0)
#endif
#ifndef DEBUG_LEX_MATCH
#define DEBUG_LEX_MATCH if(0)
#endif

// gram
#ifndef DEBUG_GRAM_TOKENNODE
#define DEBUG_GRAM_TOKENNODE if(0)
#endif
#ifndef DEBUG_GRAM_DEFTRACE
#define DEBUG_GRAM_DEFTRACE if(0)
#endif
#ifndef DEBUG_GRAM_NFA
#define DEBUG_GRAM_NFA if(0)
#endif
#ifndef DEBUG_GRAM_DFA
#define DEBUG_GRAM_DFA if(0)
#endif
#ifndef DEBUG_GRAM_FIRST
#define DEBUG_GRAM_FIRST if(0)
#endif
#ifndef DEBUG_GRAM_LR1TABLE
#define DEBUG_GRAM_LR1TABLE if(0)
#endif
#ifndef DEBUG_GRAM_LR1ANALYZE
#define DEBUG_GRAM_LR1ANALYZE if(0)
#endif