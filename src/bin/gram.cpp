#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"
#include "gram.h"

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <grammer-rule> <lex-output> <grammer-output>" << std::endl;
        return 1;
    }
    std::string tmp = PROJECT_ROOT_DIR;
    std::ifstream gram_in(tmp + "/test/gram/" + argv[1] + ".y");
    if(!gram_in.is_open())  { std::cout << "gram_in open failed!" << std::endl; return 1; }

    std::ifstream token_in(tmp + "/test/pipeline/" + argv[2] + ".lex");
    if(!token_in.is_open())  { std::cout << "token_in open failed!" << std::endl; return 1; }
    std::ofstream gram_out(tmp + "/test/pipeline/" + argv[3] + ".gram");

    // 解析语法文件
    std::ifstream gram_rule(tmp + "test/lex/gram.l");
    if(!gram_rule.is_open())  { std::cout << "gram_rule open failed!" << std::endl; return 1; }

    int ret = grammer(gram_in, gram_rule, token_in, gram_out);
    if(ret != 0) {
        std::cerr << "grammer failed!" << std::endl;
        return 1;
    }
    return 0;
}