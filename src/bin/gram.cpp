#include <iostream>
#include <string>
#include <fstream>
#include "defs.h"
#include "automachine.h" 
#include "lex.h"
#include "gram.h"
#include "debug.h"
#include "table.h"

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

void FindFirstSet(const std::string token, const std::unordered_map<std::string, std::vector<std::vector<std::string>> >& grams, 
                    std::unordered_map<std::string, std::vector<std::string>>& firsts) {
    if(firsts.find(token) != firsts.end()) {
        // 若已经计算过，则直接返回
        return ;
    }
    if(grams.find(token) == grams.end()) {
        // 若是终结符，则直接返回
        firsts[token].push_back(token);
        return ;
    }
    for(auto& gram : grams.at(token)) {
        // 若是非终结符，则递归计算
        if(gram[0] == token) {
            // 若是自身，则跳过
            continue;
        }
        FindFirstSet(gram[0], grams, firsts);
        for(auto& first_token : firsts[gram[0]]) {
            if(std::find(firsts[token].begin(), firsts[token].end(), first_token) == firsts[token].end())
                firsts[token].push_back(first_token);
        }
    }

}

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
    std::unordered_map<std::string, int> terminal_map;

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
                terminal_map[match] = 0;
                cur->next.push_back(newNode);
                cur = newNode;
            }
        } else if(token == "TERMINAL") {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                newNode->token = match;
                newNode->is_terminal = 1;   //terminal
                terminal_map[match] = 1;
                cur->next.push_back(newNode);
                cur = newNode;
        } else if(token == "STRING") {
                GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
                newNode->token = match;
                newNode->is_terminal = 1;   //terminal
                terminal_map[match] = 1;
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
                terminal_map[newNode->token] = 0;
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
    terminal_map["$"] = 1;


    GramTokenNode* startNode = new GramTokenNode(cur_node_count++);
    startNode->token = "$START";
    startNode->is_terminal = 0;
    GramTokenNode* newNode = new GramTokenNode(cur_node_count++);
    newNode->token = gram_nodes[0]->token;
    newNode->is_terminal = 0;
    startNode->next.push_back(newNode);
    gram_nodes.push_back(startNode);
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
    // 计算first集, follow集
    std::unordered_map<std::string, std::vector<std::string>> firsts;
    std::unordered_map<std::string, std::vector<std::string>> follows;

    firsts["$"].push_back("$");
    for(auto& gram : grams) {
        FindFirstSet(gram.first, grams, firsts);
    }

    DEBUG_GRAM_FIRST {
        std::cout << std::endl << std::endl;
        for(auto& first : firsts) {
            std::cout << first.first << ": ";
            for(auto& token : first.second) {
                std::cout << token << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
    
    // 计算follow集

    // follows["$START"].push_back("$");
    // for(auto& gram : grams) {
    //     FindFollowSet(gram.first, grams, firsts, follows);
    // }

    // DEBUG_GRAM_FOLLOW {
    //     std::cout << std::endl << std::endl;
    //     for(auto& follow : follows) {
    //         std::cout << follow.first << ": ";
    //         for(auto& token : follow.second) {
    //             std::cout << token << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }


    // 构造NFA
    std::unordered_map<std::string,                                    //cur_token
        std::unordered_map<int,                                        //gram_id
        std::unordered_map<std::string, GramNFANode*>>> token2nodes;  // next_token -> node
    GramNFA gramNFA;
    GramNFANode* start = gramNFA.NewNode();
    start->token.cur_token = "$START";
    start->token.next_token = "$";    // 初始的next_token为$
    start->token.stack_pos = 0;
    start->token.gram_id = 0;
    gramNFA.head = start;
    token2nodes["$START"][0]["$"] = start;

    std::unordered_map<int, int> flags;
    gramNFA.head->Print(grams, flags);
    std::cout << std::endl << std::endl;

    std::queue<GramNFANode*> q;
    q.push(start);
    while(!q.empty()) {
        GramNFANode* cur = q.front();
        q.pop();
        // std::cout << cur->id << ":start" << std::endl;
        // flags.clear();
        // gramNFA.head->Print(grams, flags);
        // std::cout << std::endl << std::endl;
        const LR1Item& cur_item = cur->token;
        const std::vector<std::string>& gram = grams[cur_item.cur_token][cur_item.gram_id];
        if(cur_item.stack_pos == gram.size()){
            continue;
        }
        std::string input_token = gram[cur_item.stack_pos];
        if(terminal_map[input_token] != 0) {
            //若为terminal
            GramNFANode* newNode = gramNFA.NewNode();
            newNode->str = input_token;
            newNode->token.cur_token = cur_item.cur_token;
            newNode->token.next_token = cur_item.next_token;
            newNode->token.stack_pos = cur_item.stack_pos + 1;
            newNode->token.gram_id = cur_item.gram_id;
            cur->next.push_back(newNode);
            q.push(newNode);   
        } else {
            GramNFANode* newNode = gramNFA.NewNode();
            newNode->str = input_token;
            newNode->token.cur_token = cur_item.cur_token;
            newNode->token.next_token = cur_item.next_token;
            newNode->token.stack_pos = cur_item.stack_pos + 1;
            newNode->token.gram_id = cur_item.gram_id;
            cur->next.push_back(newNode);
            q.push(newNode);
            
            std::string next_input_token = cur_item.stack_pos + 1 < gram.size() ? gram[cur_item.stack_pos + 1] : cur_item.next_token; 
            std::vector<std::string>& next_input_firsts = firsts[next_input_token];
            // std::cout << "next_input_token:" << next_input_token << std::endl;
            // std::cout << "next_input_firsts:" << std::endl;
            // for(auto& next_input_first : next_input_firsts) {
            //     std::cout << next_input_first << " ";
            // }
            // std::cout << std::endl;
            for(auto& next_input_first : next_input_firsts) {
                for(int j = 0; j < grams[input_token].size(); ++j) {
                    if(token2nodes[input_token][j].find(next_input_first) != token2nodes[input_token][j].end()) {
                        GramNFANode* newNode2 = token2nodes[input_token][j][next_input_first];
                        cur->next.push_back(newNode2);
                        continue;
                    }
                    GramNFANode* newNode2 = gramNFA.NewNode();
                    newNode2->str = "";
                    newNode2->token.cur_token = input_token;
                    newNode2->token.next_token = next_input_first;
                    newNode2->token.stack_pos = 0;
                    newNode2->token.gram_id = j;
                    cur->next.push_back(newNode2);
                    q.push(newNode2);
                    token2nodes[input_token][j][next_input_first] = newNode2;
                }
            }

        }
        // flags.clear();
        // gramNFA.head->Print(grams, flags);
        // std::cout << std::endl << std::endl;
    }
    std::cout << "NFA states number : " << gramNFA.count << std::endl;
    flags.clear();
    gramNFA.head->Print(grams, flags);
    std::cout << std::endl << std::endl;

    gramNFA.GenerateDFA();

    flags.clear();
    gramNFA.DFA.head->Print(grams, flags, gramNFA);
    // gramNFA.DFA.head->Print(flags);

    // 生成LR1分析表
    // State-Input
    std::unordered_map<int,         // state id
        std::unordered_map<std::string, //Input String 
        std::tuple<int, std::string, int> >> LR1_table;      // tuple<type, gram_name, rule_id> | tuple<type, -1, next_state_id>
    // type: 4: shift, 1: reduce, 2: accept, 3: goto

    // // 初始化LR1分析表

    for(auto& id2node : gramNFA.DFA.id2node) {
        int state_id = id2node.first;
        GramDFANode<GramNFANode>* cur_node = id2node.second;
        std::string input_string = cur_node->str;

        std::string cur_node_state = cur_node->GetToken();
        // std::cout << "cur_node_state:" << cur_node_state << std::endl;

        std::vector<GramNFANode*> cur_node_states_vec;
        gramNFA.State2Vector(cur_node_state, cur_node_states_vec);
        for(auto& nfa_node : cur_node_states_vec) {
            const LR1Item& cur_item = nfa_node->token;
            const std::vector<std::string>& gram = grams[cur_item.cur_token][cur_item.gram_id];
            if(cur_item.stack_pos == gram.size()) {
                // reduce
                if(cur_item.cur_token == "$START" && cur_item.next_token == "$") {
                    // accept
                    LR1_table[state_id]["$"] = std::make_tuple(2, "", -1);
                } else {
                    LR1_table[state_id][cur_item.next_token] = std::make_tuple(1, cur_item.cur_token, cur_item.gram_id);
                }
            }
        }

        for(auto& next_node : cur_node->next) {
            if(terminal_map[next_node->str] != 0) {
                // shift
                LR1_table[state_id][next_node->str] = std::make_tuple(4, "", next_node->id);
            } else {
                // goto
                LR1_table[state_id][next_node->str] = std::make_tuple(3, "", next_node->id);
            }
        }

    }

    // 打印LR1分析表
    std::cout << std::endl << std::endl << "LR1_table:" << std::endl;
    Table table(gramNFA.DFA.count + 5, terminal_map.size() + 5);
    table.Set(0, 0, "state\\input");
    int tablei = 1;
    int tablej = 0;
    for(auto& terminal : terminal_map) {
        if(terminal.second != 0) {
            table.Set(tablej, tablei, terminal.first);
            ++tablei;
        }
    }
    
    for(auto& terminal : terminal_map) {
        if(terminal.second == 0) {
            table.Set(tablej, tablei, terminal.first);
            ++tablei;
        }
    }
    ++tablej;
    tablei = 0;
    for(auto& state2input : LR1_table) {
        table.Set(tablej, tablei, std::to_string(state2input.first));
        ++tablei;
        for(auto& terminal : terminal_map) {
            if(terminal.second != 0) {
                std::tuple<int, std::string, int>& t = state2input.second[terminal.first];
                if(std::get<0>(t) == 4) {
                    // std::cout << "shift " << std::get<2>(t) << "\t";
                    table.Set(tablej, tablei, "shift " + std::to_string(std::get<2>(t)));
                } else if(std::get<0>(t) == 1) {
                    // std::cout << "reduce " << std::get<1>(t) << " " << std::get<2>(t) << "\t";
                    table.Set(tablej, tablei, "reduce " + std::get<1>(t) + " " + std::to_string(std::get<2>(t)));
                } else if(std::get<0>(t) == 2) {
                    // std::cout << "accept\t";
                    table.Set(tablej, tablei, "accept");
                } else {
                    // std::cout << "error\t";
                    table.Set(tablej, tablei, "error");
                }
                ++tablei;
            }
        }
        for(auto& terminal : terminal_map) {
            if(terminal.second == 0) {
                std::tuple<int, std::string, int>& t = state2input.second[terminal.first];
                if(std::get<0>(t) == 3) {
                    // std::cout << "goto " << std::get<2>(t) << "\t";
                    table.Set(tablej, tablei, "goto " + std::to_string(std::get<2>(t)));
                } else {
                    // std::cout << "error\t";
                    table.Set(tablej, tablei, "error");
                }
                ++tablei;
            }
        }
        // std::cout << state2input.first << std::endl;
        ++tablej;
        tablei = 0;
    }

    table.Print();

    // LR1分析
    std::vector<std::string>    symbol_stack;
    std::vector<int>            state_stack;
    
}