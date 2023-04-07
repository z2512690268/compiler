#pragma once
// 开关部分
// 打印预处理正则表达式
#define DEBUG_NEWREGEX if(1)
// 打印NFA
#define DEBUG_NFA if(1)
// 打印DFA
#define DEBUG_DFA if(1) 
// 打印匹配过程
#define DEBUG_MATCH if(1)






// 归零条件
#ifndef DEBUG_NEWREGEX
#define DEBUG_NEWREGEX if(0)
#endif
#ifndef DEBUG_DFA
#define DEBUG_DFA if(0)
#endif
#ifndef DEBUG_NFA
#define DEBUG_NFA if(0)
#endif
#ifndef DEBUG_MATCH
#define DEBUG_MATCH if(0)
#endif