#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"
#include "gram.h"
#include "path.h"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <grammer-rule> <lex-output> <grammer-output>" << std::endl;
        return 1;
    }
    Path tmp = PROJECT_ROOT_DIR;
    std::ifstream gram_in(tmp.append("test").append("gram").append(argv[1]).add(".y"));
    if(!gram_in.is_open())  { std::cout << "gram_in open failed!" << std::endl; return 1; }

    std::ifstream token_in(tmp.append("test").append("pipeline").append(argv[2]).add(".lex"));
    if(!token_in.is_open())  { std::cout << "token_in open failed!" << std::endl; return 1; }
    std::ofstream gram_out(tmp.append("test").append("pipeline").append(argv[3]).add(".gram"));

    // 解析语法文件
    std::ifstream gram_rule(tmp.append("src").append("res").append("gram.l"));
    if(!gram_rule.is_open())  { std::cout << "gram_rule open failed!" << std::endl; return 1; }

    int ret = grammer(gram_in, gram_rule, token_in, gram_out);
    if(ret != 0) {
        std::cerr << "grammer failed!" << std::endl;
        return 1;
    }
    return 0;
}