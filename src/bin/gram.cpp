#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"
#include "automachine.h" 
#include "lex.h"
#include "debug.h"

struct Grammer {
    std::string nonterminal;
    std::vector<std::string> rule;
};

struct GramTokenNode {
    int id;
    std::string token;
    int is_terminal;
    std::vector<GramTokenNode*> next;
    GramTokenNode(int node_count) {
        is_terminal = 0;
        token = "";
        id = node_count;
    }
    void Print(std::unordered_map<int, int>& flag) {
        flag[id] = 1;
        std::cout << "id: " << id << " is_terminal:" << is_terminal << " token: " << token << std::endl;
        for(int i = 0; i < next.size(); i++) {
            std::cout << "\t" << "next: " << next[i]->id << std::endl;
        }
        for(int i = 0; i < next.size(); i++) {
            if(flag[next[i]->id] == 0)
                next[i]->Print(flag);
        }
    }
    void GetTraces(std::vector<std::vector<std::string>>& grams, std::vector<std::string>& cur_stack) {
        if(next.size() == 0) {
            grams.push_back(cur_stack);
            // std::cout << &grams.back()[0][0] << " " << &cur_stack[0][0];
            return ;
        } 
        for(int i = 0; i < next.size(); ++i) {
            if(next[i]->token != "")
                cur_stack.push_back(next[i]->token);
            next[i]->GetTraces(grams, cur_stack);
            if(next[i]->token != "")
                cur_stack.pop_back();
        }
    }

};

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

    std::vector<std::pair<std::string, std::string>> gram_lex;
    int ret = lexer(gram_rule, gram_in, gram_lex);
    if(ret) { std::cout << "ERROR IN LEXER!" << std::endl; return 1; }

    int grammer_left = 1;
    std::vector<GramTokenNode*> gram_nodes;
    std::vector<GramTokenNode*> start_stack;
    std::vector<GramTokenNode*> end_stack;
    std::string cur_origin_gram;
    int cur_node_count = 1;
    int repeat_count;
    GramTokenNode* cur;
    for(auto& token_match : gram_lex) {
        std::string token = token_match.first;
        std::string match = token_match.second;
        DEBUG_GRAM_TOKENNODE std::cout << "New Pair: " << token << " " << match << std::endl;
        if(token == "WHITESPACE") {
            continue;
        } else if(token == "LineComment") {
            continue;
        } else if(token == "INFER") {
            grammer_left = 0;
        } else if(token == "NONTERMINAL") {
            if(grammer_left) {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                cur = newNode;
                cur->token = match;
                gram_nodes.push_back(newNode);
                cur_origin_gram = match;
                repeat_count = 0;
                start_stack.push_back(cur);
            } else {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                newNode->token = match;
                newNode->is_terminal = 0;   //Non terminal
                cur->next.push_back(newNode);
                cur = newNode;
            }
        } else if(token == "TERMINAL") {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                newNode->token = match;
                newNode->is_terminal = 1;   //terminal
                cur->next.push_back(newNode);
                cur = newNode;
        } else if(token == "STRING") {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                newNode->token = match;
                newNode->is_terminal = 1;   //terminal
                cur->next.push_back(newNode);
                cur = newNode;
        } else if(token == "RESERVED") {
            if(match == "[") {
                start_stack.push_back(cur);
            } else if(match == "]") {
                GramTokenNode* start_top = start_stack.back();
                start_stack.pop_back();
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                cur->next.push_back(newNode);   // 语句执行一次
                start_top->next.push_back(newNode); // 语句执行零次
                cur = newNode;
            } else if(match == "(") {
                start_stack.push_back(cur);
            } else if(match == ")") {
                GramTokenNode* start_top = start_stack.back();
                start_stack.pop_back();
                end_stack.push_back(cur);
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                for(int i = 0; i < start_top->next.size(); ++i) {
                    GramTokenNode* end_top = end_stack.back();
                    end_stack.pop_back();
                    end_top->next.push_back(newNode);
                }
                cur = newNode;
            } else if(match == "{") {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);   //开启新语法
                repeat_count++;
                newNode->token = cur_origin_gram + "$" + std::to_string(repeat_count);
                GramTokenNode* nextNewNode = new GramTokenNode(cur_node_count++);   //新语法左递归自己
                nextNewNode->token = newNode->token;
                newNode->next.push_back(nextNewNode);
                GramTokenNode* nextNewNewNode = new GramTokenNode(cur_node_count++); //递归中止空转移
                nextNewNode->next.push_back(nextNewNewNode);
                newNode->next.push_back(nextNewNewNode);

                GramTokenNode* originNewNode = new GramTokenNode(cur_node_count++); //原语法也要添加该语法节点
                originNewNode->token = newNode->token;
                cur->next.push_back(originNewNode);
                start_stack.push_back(originNewNode);
                gram_nodes.push_back(newNode);
                
                cur = nextNewNewNode;  //先处理嵌套语法
            } else if(match == "}") {
                cur = start_stack.back();   //回归原始语法
                start_stack.pop_back();
            } else if(match == "|") {
                end_stack.push_back(cur);
                cur = start_stack.back();
            } else if(match == ";") {
                grammer_left = 1;
                start_stack.pop_back();
            }
        }
        DEBUG_GRAM_TOKENNODE {
            for(auto& gram_node : gram_nodes) {
                std::unordered_map<int, int> flags;
                gram_node->Print(flags);
                std::cout << std::endl << std::endl;
            }
            std::cout << std::endl << std::endl;
        }
    }

    // for(auto& gram_node : gram_nodes) {
    //     std::unordered_map<int, int> flags;
    //     gram_node->Print(flags);
    //     std::cout << std::endl << std::endl;
    // }

    std::unordered_map<std::string, std::vector<std::vector<std::string>> > grams;
    // 两层数组，元素为string, 表示第i个非终止符的所有导出
    for(auto& gram_node : gram_nodes) {
        std::vector<std::string> temp_stack;
        gram_node->GetTraces(grams[gram_node->token], temp_stack);
    }

    DEBUG_GRAM_DEFTRACE {
        for(int i = 0; i < gram_nodes.size(); ++i) {
            auto& gram = grams[gram_nodes[i]->token];
            for(auto& rule : gram) {
                std::cout << gram_nodes[i]->token << ": ";
                for(auto& single_token : rule) {
                    std::cout << single_token << " ";
                }
                std::cout << std::endl;
            }
        }
    }
}