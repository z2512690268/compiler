#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"
#include "automachine.h" 
#include "lex.h"

struct Grammer {
    std::string nonterminal;
    std::vector<std::string> rule;
};

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <grammer-rule> <lex-output> <grammer-output>" << std::endl;
        return 1;
    }
    std::string tmp = PROJECT_ROOT_DIR;
    std::ifstream gram_in(tmp + "/test/grammer/" + argv[1] + ".y");
    if(!gram_in.is_open())  { std::cout << "gram_in open failed!" << std::endl; return 1; }

    std::ifstream token_in(tmp + "/test/pipeline/" + argv[2] + ".lex");
    if(!token_in.is_open())  { std::cout << "token_in open failed!" << std::endl; return 1; }
    std::ofstream gram_out(tmp + "/test/pipeline/" + argv[3] + ".gram");

    // 解析语法文件
    std::ifstream gram_rule(tmp + "test/lex/gram.l");
    if(!gram_rule.is_open())  { std::cout << "gram_rule open failed!" << std::endl; return 1; }

    std::vector<std::pair<std::string, std::string>> gram_lex;
    int ret = lexer(gram_rule, gram_in, gram_lex);
    if(ret) { std::cout << "ERROR IN LEXER!" << std::endl; return 1; }

    std::vector<Grammer> grammers;
    std::vector<int> gram_stack;
    int grammer_left = 1;
    int grammer_top;
    for(auto& token_match : gram_lex) {
        std::string token = token_match.first;
        std::string match = token_match.second;
        if(token == "WHITESPACE") {
            continue;
        } else if(token == "LineComment") {
            continue;
        } else if(token == "INFER") {
            grammer_left = 0;
        } else if(token == "NONTERMINAL") {
            if(grammer_left) {
                Grammer new_gram;
                new_gram.nonterminal = match;
                grammers.push_back(new_gram);
                grammer_top = grammers.size();
            } else {

            }
        } else if(token == "TERMINAL") {

        } else if(token == "STRING") {

        } else if(token == "RESERVED") {
            if(match == "[") {
                
            } else if(match == "]") {

            } else if(match == "(") {

            } else if(match == ")") {

            } else if(match == "{") {

            } else if(match == "}") {

            } else if(match == "|") {
                
            }
        }
        
    }

}