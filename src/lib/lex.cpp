#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <algorithm>
#include "defs.h"
#include "debug.h"
#include "transfer.h"
#include "automachine.h"
#include "lex.h"

int lexer(std::istream& fin, std::istream& input, std::vector<std::pair<std::string, std::string>>& output)
{
    // paser rule file to NFA
    // rule regex defination: 
    //  support |, *, (), default concat, \n, \t, \\, \:/**/
    // example:
    //first_rule:a|b
    //second_rule:(a|b)*
    //third_rule:ab(sd)*a|b
    LexNFA NFA;
    NFA.head = NFA.NewNode();

    std::string buffer;

    std::vector<std::string> token_list;
    std::vector<int> lex_operations = {
        '(', ')', '*', '+', '.', '?', '\\', '{', '}', '|', '-', '[', ']'
    };
    int cnt = 1;
    while(std::getline(fin, buffer)) {
        if(buffer.empty() || buffer[0] == '#')
            continue;
        // std::cout << buffer << std::endl;
        int pos = buffer.find(":");
        std::string token = buffer.substr(0, pos);
        token_list.push_back(token);
        std::string regex = buffer.substr(pos+1);

        // regex预处理
        // ()\*|
        // 前后加括号
        regex = "(" + regex + ")";
        DEBUG_LEX_NEWREGEX std::cout << "Add Bracket:" << regex << std::endl;

        // 将所有字符转化为十六进制
        std::string new_regex;
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '\\') {
                if(i == regex.size() - 1) {
                    std::cout << "Error: \\ must be followed by a character" << std::endl;
                    return 1;
                }
                switch(regex[i + 1]) {
                    case 'n':
                        new_regex += TransferHex('\n');
                        break;
                    case 't':
                        new_regex += TransferHex('\t');
                        break;
                    case 'r':
                        new_regex += TransferHex('\r');
                        break;
                    case 'v':
                        new_regex += TransferHex('\v');
                        break;
                    case 'f':
                        new_regex += TransferHex('\f');
                        break;
                    case 'd':
                        new_regex += "(";
                        for(uint8_t ch = '0'; ch <= '9'; ch++) {
                            new_regex += TransferHex(ch);
                            if(ch != '9')
                                new_regex += '|';
                        }
                        new_regex += ")";
                        break;
                    case 'w':
                        new_regex += "(";
                        for(uint8_t ch = 'a'; ch <= 'z'; ch++) {
                            new_regex += TransferHex(ch);
                            new_regex += '|';
                        }
                        for(uint8_t ch = 'A'; ch <= 'Z'; ch++) {
                            new_regex += TransferHex(ch);
                            new_regex += '|';
                        }
                        for(uint8_t ch = '0'; ch <= '9'; ch++) {
                            new_regex += TransferHex(ch);
                            new_regex += '|';
                        }
                        new_regex += TransferHex('_') + ")";
                        break;
                    case 'x':
                        if(i + 3 >= regex.size()) {
                            std::cout << "Error: \\x must be followed by two hex digits" << std::endl;
                            return 1;
                        }
                        if(!isxdigit(regex[i + 2]) || !isxdigit(regex[i + 3])) {
                            std::cout << "Error: \\x must be followed by two hex digits" << std::endl;
                            return 1;
                        }
                        new_regex += toupper(regex[i + 2]) + toupper(regex[i + 3]);
                        i += 2;
                        break;
                    default:
                        new_regex += TransferHex(regex[i + 1]);
                        break;
                }
                i++;
            } else {
                if(std::find(lex_operations.begin(), lex_operations.end(), regex[i]) != lex_operations.end()) {
                    new_regex += regex[i];
                } else {
                    new_regex += TransferHex(regex[i]);
                }
            }
        }
        regex = new_regex;

        DEBUG_LEX_NEWREGEX std::cout << "Transfer:" << regex << std::endl;
        // 处理-运算

        new_regex.clear();
        // 寻找所有的-
        std::vector<int> pos_list;
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '-') {
                pos_list.push_back(i);
            }
        }
        std::cout << pos_list.size() << std::endl;;
        // 处理-运算, 前后必须是十六进制字符，否则报错
        int j = 0;
        for(int i = 0; i < pos_list.size(); ++i) {
            if(pos_list[i] - 4 < 0 || pos_list[i] + 4 >= regex.size()) {
                std::cout << "Error: - must be surrounded by two hex digits" << std::endl;
                return 1;
            }
            if(regex[pos_list[i] - 4] != '\\' || regex[pos_list[i] - 3] != 'x' ||
                !isxdigit(regex[pos_list[i] - 2]) || !isxdigit(regex[pos_list[i] - 1]) ||
                regex[pos_list[i] + 1] != '\\' || regex[pos_list[i] + 2] != 'x' ||
                !isxdigit(regex[pos_list[i] + 3]) || !isxdigit(regex[pos_list[i] + 4])) {
                std::cout << "Error: - must be surrounded by two hex digits" << std::endl;
                return 1;
            }
            for(; j < pos_list[i] - 4; j++) {
                new_regex += regex[j];
            }
            uint8_t start = TransBackHex(regex[pos_list[i] - 2], regex[pos_list[i] - 1]);
            uint8_t end = TransBackHex(regex[pos_list[i] + 3], regex[pos_list[i] + 4]);
            if(start > end) {
                std::cout << "Error: - left must be less than right" << std::endl;
                return 1;
            }
            new_regex += "(";
            for(uint8_t ch = start; ch <= end; ch++) {
                new_regex += TransferHex(ch);
                if(ch != end)
                    new_regex += '|';
            }
            new_regex += ")";
            j = pos_list[i] + 5;
        }
        for(; j < regex.size(); j++) {
            new_regex += regex[j];
        }
        regex = new_regex;
        DEBUG_LEX_NEWREGEX std::cout << "Expand:" << regex << std::endl;


        // 处理通配符.
        new_regex.clear();
        for(int i = 0; i < regex.size(); ++i){
            if(regex[i] == '.') {
                new_regex += "(";
                for(uint8_t ch = 0x01; ch != 0x00; ch++) {
                    new_regex += TransferHex(ch);
                    if(ch != 0xff)
                        new_regex += '|';
                // DEBUG_LEX_NEWREGEX std::cout << ch << " general match sign:" << regex << std::endl;
                }
                new_regex += ")";
            } else {
                new_regex += regex[i];
            }
        }
        regex = new_regex;

        DEBUG_LEX_NEWREGEX std::cout << "general match sign:" << regex << std::endl;
        DEBUG_LEX_NEWREGEX std::cout << "End the regex" << std::endl << std::endl;



        // std::cout << token << " " << regex << std::endl;
        // regex paser to NFA
        std::vector<LexNFANode*> start_stack;
        std::vector<LexNFANode*> end_stack;
        LexNFANode* start, *end;
        LexNFANode* newNode = NFA.NewNode();
        NFA.head->AddNext(newNode);
        start = newNode;
        end = newNode;
        for(int i = 0; i < regex.size(); i++) {
            if(regex[i] == '(') {
                // 左括号压一个start栈
                LexNFANode* newNode = NFA.NewNode();
                end->AddNext(newNode);
                start = newNode;
                end = newNode;
                start_stack.push_back(end);
            } else if(regex[i] == ')') {
                // 右括号弹一个start栈，压入一个end栈，弹len(start->next)个end栈
                LexNFANode* newNode = NFA.NewNode();
                end_stack.push_back(end);
                if(start_stack.empty()) {
                    std::cout << "error: " << buffer << std::endl;
                    return 1;
                }
                start = start_stack.back();
                start_stack.pop_back();
                for(int j = 0; j < start->next.size(); j++) {
                    if(end_stack.empty()) {
                        std::cout << "error: " << buffer << std::endl;
                        return 1;
                    }
                    LexNFANode* endtop = end_stack.back();
                    end_stack.pop_back();
                    endtop->AddNext(newNode);
                }
                end = newNode;
            } else if(regex[i] == '|') {
                // |压入一个end栈
                end_stack.push_back(end);
                end = start_stack.back();
                start = end;
            } else if(regex[i] == '*') {
                // start, end循环
                LexNFANode* newNode1 = NFA.NewNode();
                LexNFANode* newNode2 = NFA.NewNode();
                newNode1->Copy(start);
                start->Copy(newNode2);
                if(start == end) {
                    // 单字符+*
                    start->AddNext(newNode1);
                    start->AddNext(newNode2);
                    newNode1->AddNext(newNode1);
                    newNode1->AddNext(newNode2);
                } else {
                    // 括号+*
                    start->AddNext(newNode1);
                    start->AddNext(newNode2);
                    end->AddNext(newNode1);
                    end->AddNext(newNode2);
                }
                end = newNode2;
                start = end;
            } else if(regex[i] == '\\'){
                // 转义字符, 后跟至少三个字符\xXX
                if(i + 3 >= regex.size()) {
                    std::cout << "error: " << buffer << std::endl;
                    return 1;
                }
                uint8_t ch = TransBackHex(regex[i + 2], regex[i + 3]);

                LexNFANode* newNode = NFA.NewNode();
                newNode->SetInput(ch);
                end->AddNext(newNode);
                end = newNode;
                start = end;
                i += 3;
            }
        }
        // newNode = new LexNode();
        // end->next.push_back(newNode);
        // end = newNode;
        end->token = token;
        // std::cout << "step "<< cnt << std::endl;
        // memset(flag, 0, sizeof(flag));
        // NFA.head->Print(flag);
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // cnt++;
    }
    DEBUG_LEX_NFA
    {
        std::cout << "step final:" << std::endl;
        std::unordered_map<int, int> flag;
        NFA.head->Print(flag);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
        // exit(0);
    // NFA to DFA
 
        // std::cout << "step "<< cnt << std::endl;
        // memset(flag, 0, sizeof(flag));
        // DFA.head->Print(flag);
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // std::cout << std::endl;
        // cnt++;
        // if(cnt >= 10)
        //     exit(0);
    NFA.GenerateDFA();

    DEBUG_LEX_DFA
    { 
        std::cout << "step "<< "final" << std::endl;
        std::unordered_map<int, int> flag;
        NFA.DFA.head->Print(flag);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
    // parse by DFA
    char ch;
    std::string match;
    std::string token;
    LexDFANode* cur = NFA.DFA.head;
    while(true) {
        int i;
        input.get(ch);
        int cur_next_size;
        int flag = 0;
        // 1. 当前字符读取失败， match不为空，输出match
        // 2. 当前字符读取失败 match为空，break
        // 3. 当前字符读取成功，match匹配成功，加入match继续读取
        // 4. 当前字符读取成功，match匹配失败，输出match， unget当前字符
        if(input.fail())    {
            if(match == ""){
                DEBUG_LEX_MATCH std::cout << "End of input file!" << std::endl;
                break;
            } else {
                flag = 2;
            }
        } else {
            cur_next_size = cur->next.size();
            // 还能匹配就继续匹配
            DEBUG_LEX_MATCH std::cout << std::endl << std::endl;
            DEBUG_LEX_MATCH std::cout << "Round Start:" << "Id:" << cur->id << " matched:" << match << std::endl;
            DEBUG_LEX_MATCH std::cout << "New Char:" << ch << std::endl;
            for(i = 0; i < cur->next.size(); i++) {
                // std::cout << "cur->next[i]->GetInput():" << char(cur->next[i]->GetInput()) << std::endl;
                if(cur->next[i]->GetInput() == ch) {
                    cur = cur->next[i];
                    match += ch;
                    // std::cout << i << std::endl;
                    break;
                }
            }

            if(i == cur_next_size) {
                // 不能匹配了
                flag = 4;
            } else {
                // 还能匹配， 继续匹配
                continue;
            }
        }
        // std::cout << "flag:" << flag << std::endl;
        if(flag) {
            // 进行匹配输出
            DEBUG_LEX_MATCH std::cout << "Unmatched! StateId:" << cur->id << " NFAStates:" << cur->PrintToken() << std::endl;
            std::vector<LexNFANode*> state_vec;
            NFA.State2Vector(cur->GetToken(), state_vec);
            std::unordered_set<std::string> token_vec;
            DEBUG_LEX_MATCH std::cout << "Token List: " << std::endl;
            for(int j = 0; j < state_vec.size(); j++) {
                if(state_vec[j]->token != "") {
                    DEBUG_LEX_MATCH std::cout << "\tNFAState:" << state_vec[j]->id << " token:" << state_vec[j]->token << std::endl;
                    token_vec.insert(state_vec[j]->token);
                }
            }
            int j;
            for(j = 0; j < token_list.size(); j++) {
                if(token_vec.find(token_list[j]) != token_vec.end()) {
                    token = token_list[j];
                    break;
                }
            }
            if(j == token_list.size()) {    // 未匹配到任何token，输出单个字符
                // std::cout << "ssd2" << std::endl;
                DEBUG_LEX_MATCH std::cout << "Match Single Char" << std::endl;
                if(flag == 4) match += ch;
                int cnt = match.size();
                while(cnt > 1) {
                    DEBUG_LEX_MATCH std::cout << "Unget char:" << ch << std::endl;
                    input.unget();
                    cnt--;
                }
                token = "'";
                token.push_back(match[0]);
                token += "'";
                match = match.substr(0, 1);
            } else if(flag == 4){   
                // 匹配到token，输出token
                DEBUG_LEX_MATCH std::cout << "Unget char:" << ch << std::endl; 
                input.unget();
            }
            // std::cout << Unprint2Trans(token) << " " << "\"" << Unprint2Trans(match) << "\"" << std::endl;
            output.push_back(std::make_pair(token, match));
            DEBUG_LEX_MATCH std::cout << "Matched!!!, token:" << token << " matched:" << match << std::endl;
            cur = NFA.DFA.head;
            match = "";
            token = "";
            // if(!input.eof()) {
            //     char testch = input.get();
            //     input.unget();
            //     std::cout << "#13# "<< testch << std::endl;
            // }
        }
    }
    return 0;
}
